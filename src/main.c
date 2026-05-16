/*
* MorceNOX C Art Text Reader™
*
* Copyright (C) 2026 Amilcar Antonio Mesquita Rizk <amilcar.rizk@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <piper.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <poll.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <dirent.h>

#include "talker.h"
#include "printer.h"
#include "json_helper.h"
#include "environment.h"
#include "ANSI-color-codes.h"
#include "colors.h"
#include "string_helper.h"
#include "terminal_utils.h"
#include "commands.h"
#include "app.h"

// --- Structures ---
typedef struct {
    char **lines;
    int count;
} FileContent;

typedef struct {
    const char *symbol;
    const char *name;
} SymbolMap;

// --- Globals ---
struct termios orig_termios;
int tty_fd = STDIN_FILENO;
pid_t current_audio_pgid = 0;
bool paused = false;
char *fgcolor = NULL;
char *bgcolor = NULL;
char *text = NULL;
FILE *input_file = NULL;
FileContent content = {NULL, 0};
struct dirent **langlist;
int n_languages;
char *lang = NULL;



static const SymbolMap ASTRO_MAP[] = {
    {"☉", "Sol"},
    {"☾", "Luna"},
    {"☿", "Mercurius"},
    {"♀", "Venus"},
    {"♂", "Mars"},
    {"♃", "Jupiter"},
    {"♄", "Saturnus"},
    {"♅", "Uranus"},
    {"♆", "Neptunus"},
    {"⯓", "Pluto"},
    {"♇", "Pluto"},
    {"🜨", "Terra"},
    {"🝴", "Fortuna"},
    {"℞", "Retrogradus"},
    {"☊", "Caput Draconis"},
    {"☋", "Cauda Draconis"},
    {"🜍", "Sulphur"},
    {"🜔", "Sal"},
    {"🜏", "Sulphur Nigrum"},
    {"🜁", "Aer"},
    {"🜃", "Terra"},
    {"🜂", "Ignis"},
    {"🜄", "Aqua"},
    {"☌", "Conjunctio"},
    {"☍", "Oppositio"},
    {"△", "Trigonus"},
    {"⚹", "Sextilis"},
    {"□", "Quadratura"},
    {NULL, NULL} 
};


char* sanitize_for_speech(const char *input) {
    if (!input) return NULL;

    // Start with a buffer large enough to hold the expansion
    // We use a dynamic approach to ensure we don't overflow
    size_t buffer_size = strlen(input) * 2 + 1; 
    char *output = malloc(buffer_size);
    if (!output) return NULL;
    
    output[0] = '\0';
    const char *cursor = input;

    while (*cursor != '\0') {
        bool matched = false;

        for (int i = 0; ASTRO_MAP[i].symbol != NULL; i++) {
            size_t sym_len = strlen(ASTRO_MAP[i].symbol);
            
            // Check if the current cursor position matches the symbol
            if (strncmp(cursor, ASTRO_MAP[i].symbol, sym_len) == 0) {
                size_t name_len = strlen(ASTRO_MAP[i].name);
                
                // Ensure buffer has space: current length + new name + null terminator
                if (strlen(output) + name_len + 1 > buffer_size) {
                    buffer_size *= 2;
                    output = realloc(output, buffer_size);
                }

                strcat(output, ASTRO_MAP[i].name);
                cursor += sym_len; // Move cursor past the symbol
                matched = true;
                break;
            }
        }

        if (!matched) {
            // If no symbol matched, just copy the current byte
            if (strlen(output) + 2 > buffer_size) {
                buffer_size *= 2;
                output = realloc(output, buffer_size);
            }
            size_t current_len = strlen(output);
            output[current_len] = *cursor;
            output[current_len + 1] = '\0';
            cursor++;
        }
    }

    return output;
}

// --- Audio Process Management ---
void stop_current_audio() {
    if (current_audio_pgid > 0) {
        kill(-current_audio_pgid, SIGKILL);
        waitpid(-current_audio_pgid, NULL, WNOHANG);
        current_audio_pgid = 0;
    }
}

void safe_free(void **ptr_ptr) {
    if (ptr_ptr != NULL && *ptr_ptr != NULL) {
        free(*ptr_ptr);
        *ptr_ptr = NULL;
    }
}

int clean_directory(const char *dir_path) {
    struct dirent *entry;
    DIR *dp;

    // 1. Open the directory
    dp = opendir(dir_path);
    if (dp == NULL) {
        perror("opendir");
        return 1;
    }

    // 2. Change working directory so unlink can find the files by name
    if (chdir(dir_path) != 0) {
        perror("chdir");
        closedir(dp);
        return 1;
    }

    // 3. Loop through files and unlink them
    while ((entry = readdir(dp)) != NULL) {
        // Skip special directory entries "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // unlink() removes the file entry from the directory
        if (unlink(entry->d_name) == 0) {
            fprintf(stderr, "Deleted: %s\n", entry->d_name);
        } else {
            perror(entry->d_name);
        }
    }

    closedir(dp);
    return 0;
}


void free_resources() {
    free(fgcolor);
    free(bgcolor);
    
    for (int i = 0; i < n_languages; i++) {
        free(langlist[i]);
    }
    free(langlist);
    free(lang);

    safe_free((void**)&text);

    if (input_file) {
        if (input_file != stdin) {
            fclose(stdin);
        }
    }

    for (int i = 0; i < content.count; i++) 
            free(content.lines[i]);

    free(content.lines);

    reset_terminal_mode(tty_fd, &orig_termios);
}

void cleanup_resources() {    
    stop_current_audio();
}

void handle_signal(int sig) {
    free_resources(); 
    exit(0); 
}


FileContent load_file_to_memory(FILE *fp) {
    FileContent content = {NULL, 0};
    
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    // First pass: count lines so we can allocate exactly what we need
    rewind(fp);
    
    while ((read = getline(&line, &len, fp)) != -1) {
        content.count++;
    }
    
    if (content.count > 0) {
        content.lines = malloc(sizeof(char*) * content.count);
        rewind(fp);
        int idx = 0;
        while ((read = getline(&line, &len, fp)) != -1) {
            content.lines[idx] = strdup(line);
            content.lines[idx][strcspn(content.lines[idx], "\n")] = 0;
            idx++;
        }
    }
    free(line);
    if (fp != stdin)
        fclose(fp);
    return content;
}


FileContent load_stream_to_memory(FILE *stream) {
    FileContent content = {NULL, 0};
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    
    while ((read = getline(&line, &len, stream)) != -1) {
        // Since we can't rewind, we grow the array as we go
        content.lines = realloc(content.lines, sizeof(char*) * (content.count + 1));
        if (content.lines == NULL) break; 
    
        content.lines[content.count] = strdup(line);
        content.lines[content.count][strcspn(content.lines[content.count], "\n")] = 0;
        content.count++;
    }
    free(line);
    return content;
}


// NEW: Function to wrap a single string argument into the FileContent structure
FileContent wrap_text_to_content(char *text) {
    FileContent content = {NULL, 1};
    content.lines = malloc(sizeof(char*) * 1);
    content.lines[0] = strdup(text);
    return content;
}


int get_logical_key(int fd) {
    unsigned char c;
    if (read(fd, &c, 1) <= 0) return -1;

    if (c == '\x1b') { // Escape character detected
        unsigned char seq[2];
        
        struct pollfd fds[1];
        fds[0].fd = fd;
        fds[0].events = POLLIN;

        // set 20 ms to detect keys
        if (poll(fds, 1, 20) > 0 && (fds[0].revents & POLLIN)) {
            if (read(fds[0].fd, &seq[0], 1) > 0 && read(fds[0].fd, &seq[1], 1) > 0) {
                if (seq[0] == '[') {
                    if (seq[1] == 'A') return '\x01'; // Up Arrow
                    if (seq[1] == 'B') return '\x02'; // Down Arrow
                    if (seq[1] == 'C') return '\x04'; // Right Arrow
                    if (seq[1] == 'D') return '\x03'; // Left Arrow

                    // Page Up and Page Down
                    // Sequence: ESC [ 5 ~ (Page Up) or ESC [ 6 ~ (Page Down)
                    if (seq[1] == '5' || seq[1] == '6') {
                        unsigned char extra;
                        // Read the trailing '~'
                        if (read(fd, &extra, 1) > 0 && extra == '~') {
                            if (seq[1] == '5') return '\x05'; // Page Up
                            if (seq[1] == '6') return '\x06'; // Page Down
                        }
                    }
                }
                
            }
        }
        return '\x1b'; // Literal Escape key
    }
    return c;
}


int process_single_line(char *line, const char *model_path, const char *json_path, 
                        int speaker, const char *espeak_path, float speed, 
                        const char *fgcolor, const char *bgcolor, int tty_fd,
                        int line_num, const char *language, int n_lines, float volume) {
    if (!line || strlen(line) == 0) return 0;

    // --- PAUSE LOGIC ---
    if (paused) {
        
        struct pollfd fds[1];
        fds[0].fd = tty_fd;
        fds[0].events = POLLIN;

        // set 100 ms to detect keys
        if (poll(fds, 1, 100) > 0 && (fds[0].revents & POLLIN)) {
            int key = get_logical_key(tty_fd); // Use the new helper
            if (key != -1) {
                if (key == 'p' || key == 'P' || key == ' ') {
                    paused = false; 
                    return CMD_PLAY_PAUSE;
                }
                if (key == 'a' || key == 'A' || key == '\x03') return CMD_BACK;
                if (key == 'd' || key == 'D' || key == '\x04') return CMD_FORWARD;
                if (key == 'q' || key == 'Q' || key == '\x1b') return CMD_QUIT;
                if (key == 's' || key == 'S' || key == '\x02') return CMD_BIG_BACK; // 's' or Down
                if (key == 'w' || key == 'W' || key == '\x01') return CMD_BIG_FORWARD; // 'w' or Up
                if (key == '\x05') return CMD_JUMP_FORWARD; // Page Up
                if (key == '\x06') return CMD_JUMP_BACK; // Page Down
            }
        }
        return CMD_PLAY_PAUSE;
    }

    trim(line);

    char *sanitized_line = sanitize_for_speech(line);
    if (!sanitized_line) sanitized_line = strdup(line); // Fallback
    
    char* printer_argv[512];
    int printer_argc = text_to_argv(line, printer_argv, 512);
    
    stop_current_audio();

    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        free(sanitized_line);
        return -1;
    } else if (pid == 0) {
        setpgid(0, 0); 
        talker(model_path, json_path, speaker, espeak_path, speed, sanitized_line, volume);
        free(sanitized_line);
        exit(0);
    } else {
        current_audio_pgid = pid;

        if (fgcolor && bgcolor) {
            char col[20];
            snprintf(col, sizeof(col), "%s%s", fgcolor, bgcolor);
            printf("%s", col);
        }
        printer(printer_argc, printer_argv, line_num, n_lines, language, speed, volume);
        if (fgcolor && bgcolor) printf(COLOR_RESET);

        for (int i = 0; i < printer_argc; i++) {
            if (printer_argv[i]) {
                free(printer_argv[i]);
            }
        }

        int status;
        while (waitpid(pid, &status, WNOHANG) == 0) {
            
            struct pollfd fds[1];
            fds[0].fd = tty_fd;
            fds[0].events = POLLIN;

            // set 50 ms to detect keys
            if (poll(fds, 1, 50) > 0 && (fds[0].revents & POLLIN)) {
                int key = get_logical_key(tty_fd); // Use the helper
                if (key != -1) {
                    // 1. Navigation (Single Step)
                    if (key == 'a' || key == 'A' || key == '\x03') return CMD_BACK;    // 'a' or Left
                    if (key == 'd' || key == 'D' || key == '\x04') return CMD_FORWARD; // 'd' or Right
                    
                    // 2. Navigation (Large Step)
                    if (key == 's' || key == 'S' || key == '\x02') return CMD_BIG_BACK; // 's' or Down
                    if (key == 'w' || key == 'W' || key == '\x01') return CMD_BIG_FORWARD; // 'w' or Up

                    if (key == '\x05') return CMD_JUMP_FORWARD; // Page Up
                    if (key == '\x06') return CMD_JUMP_BACK; // Page Down
    
                    // 3. Playback/Quit
                    if (key == 'p' || key == 'P' || key == ' ') {
                        paused = true;
                        stop_current_audio();
                        return CMD_PLAY_PAUSE;
                    }
                    if (key == 'q' || key == 'Q' || key == '\x1b') return CMD_QUIT;
                    
                    // 4. Speed & Settings
                    if (key == ']') return CMD_INC_SPEED;
                    if (key == '[') return CMD_DEC_SPEED;
                    if (key == '{') return CMD_HALF_SPEED;
                    if (key == '}') return CMD_DOUBLE_SPEED;
                    if (key == 'r' || key == 'R' || key == '\x7f' || key == '\x08') return CMD_SPEED_RESET; // 'r' or 'backspace'
                    if (key == '\\') return CMD_CHANGE_BG_COLOR_EXT_NEXT;
                    if (key == '|') return CMD_CHANGE_BG_COLOR_EXT_PREV;                    
                    if (key == 'z' || key == 'Z') return CMD_DEFAULT_COLOR;
                    if (key == 'x' || key == 'X') return CMD_CHANGE_BG_COLOR;
                    if (key == 'c' || key == 'C') return CMD_NEXT_COLOR_SCHEME; 
                    if (key == 'b' || key == 'B') return CMD_TOGGLE_BOLD;
                    if (key == 'n' || key == 'N') return CMD_TOGGLE_DIM;
                    if (key == 'h' || key == 'H') return CMD_TOGGLE_HIGHLIGHT;
                    if (key == 'l' || key == 'L') return CMD_CYCLE_LANGUAGES;
                    if (key == 'h' || key == 'H') return CMD_TOGGLE_HIGHLIGHT;
                    if (key == '-' || key == '/') return CMD_LOWER_VOLUME;
                    if (key == '=' || key == '+' || key == '*') return CMD_RAISE_VOLUME;
                    if (key == 'k' || key == 'K') return CMD_COPY_TEXT; // prints the text in normal letters
                }
            }

            usleep(10000); 
        }
        free(sanitized_line);
        current_audio_pgid = 0;
        return 0;
    }
}

int filter_dots(const struct dirent *entry) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        return 0;
    }
    return 1;
}



int main(int argc, char *argv[]) {

    tty_fd = open("/dev/tty", O_RDONLY);
    if (tty_fd < 0) tty_fd = STDIN_FILENO;
    
    const char *home_dir = getenv("HOME");
    char config_path[512];
    snprintf(config_path, 512, "%s/%s/%s", home_dir, ".config", APPLICATION_NAME);

    atexit(cleanup_resources);
    signal(SIGINT, handle_signal);  // Ctrl+C
    signal(SIGTERM, handle_signal); // Kill command
    signal(SIGHUP, handle_signal);  // Terminal closed

    printf("\e[48;5;16m");
    system("clear");
    enable_raw_mode(tty_fd, &orig_termios);

    fprintf(stderr, "Program name is %s\n", APPLICATION_NAME);

    // Parse command line arguments
    float speed = NAN;
    const char *language = NULL;
    char *color = NULL;    

    int fgColorCode = 7; // White
    int bgColorCode = 0;
    int bold = 0;
    int dim = 0;
    int highlight = 0;
    int lang_index = 0;
    float volume = NAN;

    int read_from_file = 0;
      
    
    // Process arguments
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "--speed") == 0 || strcmp(argv[i], "-s") == 0) {
            if (i + 1 < argc) {
                speed = atof(argv[i + 1]);
                i += 2;
            } else {
                fprintf(stderr, "Error: --speed/-s requires a value\n");
                return 1;
            }
        }
        if (strcmp(argv[i], "--volume") == 0 || strcmp(argv[i], "-v") == 0) {
            if (i + 1 < argc) {
                volume = atof(argv[i + 1]);
                i += 2;
            } else {
                fprintf(stderr, "Error: --volume/-v requires a value\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "--language") == 0 || strcmp(argv[i], "-l") == 0) {
            if (i + 1 < argc) {
                language = argv[i + 1];
                i += 2;
            } else {
                fprintf(stderr, "Error: --language/-l requires a value\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "--color") == 0 || strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) {
                color = argv[i + 1];
                i += 2;
            } else {
                fprintf(stderr, "Error: --color/-c requires a value\n");
                return 1;
            }
        }
        else {
            if (text == NULL && i == argc - 1) {                
                // Check if the last argument is a valid filename
                struct stat buffer;
                if (stat(argv[i], &buffer) == 0 && S_ISREG(buffer.st_mode)) {
                    // Read the file content
                    input_file = fopen(argv[i], "r");
                    if (input_file != NULL) {
                        read_from_file = 1;

                        fprintf(stderr, "Reading from file: %s\n", argv[i]);

                        break;
                    } else {
                        fprintf(stderr, "Error: Could not open file %s\n", argv[i]);
                        return 1;
                    }
                }
            }
        
            // Check if we have text
            if (text == NULL && read_from_file == 0) {
                // First non-flag argument is the start of text
                size_t total_length = 0;
                int j = i;
                while (j < argc) {
                    total_length += strlen(argv[j]) + 1; // +1 for space
                    j++;
                }
                text = malloc(total_length);
                if (!text) {
                    fprintf(stderr, "Error: Failed to allocate memory for text\n");
                    return 1;
                }
                text[0] = '\0';
                j = i;
                while (j < argc) {
                    strcat(text, argv[j]);
                    if (j < argc - 1) strcat(text, " ");
                    j++;
                }
                break;
            } else if (read_from_file == 0) {
                // Append to existing text
                size_t current_length = strlen(text);
                size_t new_length = current_length + strlen(argv[i]) + 1;
                char *new_text = realloc(text, new_length + 1);
                if (!new_text) {
                    fprintf(stderr, "Error: Failed to reallocate memory for text\n");
                    free(text);
                    return 1;
                }
                text = new_text;
                strcat(text, " ");
                strcat(text, argv[i]);
            }
            i++;
        }
    }
    
    
    char env_path[256];
    
    if (language != NULL) {
        fprintf(stderr, "Language: %s\n", language);
        snprintf(env_path, sizeof(env_path), "%s/%s%s", config_path, language, ".env");

    } else {
        language = "en";
        fprintf(stderr, "Language = default language: %s\n", language);
        snprintf(env_path, sizeof(env_path), "%s/%s%s", config_path, "en", ".env");
    }
           
    struct stat buffer;
    if (stat(env_path, &buffer) != 0) {
        fprintf(stderr, "Error: env file not found at %s\n", env_path);
        return 1;
    }
        
    load_env_file(env_path);
    
    const float default_speed = get_env_float("DEFAULT_SPEED", 1.0f);
    const float default_volume = get_env_float("DEFAULT_VOLUME", 2.0f);
    const char *model_path = getenv("PIPER_VOICE_MODEL");
    const char *json_path = getenv("PIPER_VOICE_JSON");
    const char *espeak_path = getenv("ESPEAK_DATA_PATH");
    const int speaker = get_env_int("SPEAKER_ID", 0);
    
    char models_path[256];
    snprintf(models_path, sizeof(models_path), "%s/%s", config_path, "models");

    lang = strdup(language);

    // get the available languages
    n_languages = scandir(models_path, &langlist, filter_dots, alphasort);
    if (n_languages < 0) {
        perror("scandir");
    } else {
        fprintf(stderr, "List of available languages loaded\n");

        for (int i = 0; i < n_languages; i++) {
            fprintf(stderr, "%s\n", langlist[i]->d_name); 
            if (strcmp(langlist[i]->d_name, lang) == 0) {
                lang_index = i;
            }
        }
    }

    // Check if files exist    
    if (stat(model_path, &buffer) != 0) {
        fprintf(stderr, "Error: Model file not found at %s\n", model_path);
        return 1;
    }
    if (stat(json_path, &buffer) != 0) {
        fprintf(stderr, "Error: JSON file not found at %s\n", json_path);
        return 1;
    }
    if (stat(espeak_path, &buffer) != 0) {
        fprintf(stderr, "Error: espeak data directory not found at %s\n", espeak_path);
        return 1;
    }
    
    fprintf(stderr, "Voice model file: %s\n", model_path);
    
    if (isnan(speed)) {
        speed = default_speed;
    }

    if (isnan(volume)) {
        volume = default_volume;
    }
    
    if (color != NULL) {
        if (strcmp(color, "Black") == 0 || strcmp(color, "black") == 0) {
            fgcolor = strdup(BLK);
            bgcolor = strdup(WHTB);
            fgColorCode = BLACK_CODE;
            bgColorCode = WHITE_CODE;
        }
        else if (strcmp(color, "Red") == 0 || strcmp(color, "red") == 0) {
            fgcolor = strdup(RED);
            bgcolor = strdup(GRNB);
            fgColorCode = RED_CODE;
            bgColorCode = GREEN_CODE;
        }
        else if (strcmp(color, "Green") == 0 || strcmp(color, "green") == 0) {
            fgcolor = strdup(GRN);
            bgcolor = strdup(REDB);
            fgColorCode = GREEN_CODE;
            bgColorCode = RED_CODE;
        }
        else if (strcmp(color, "Yellow") == 0 || strcmp(color, "yellow") == 0) {
            fgcolor = strdup(YEL);
            bgcolor = strdup(BLUB);
            fgColorCode = YELLOW_CODE;
            bgColorCode = BLUE_CODE;
        }
        else if (strcmp(color, "Blue") == 0 || strcmp(color, "blue") == 0) {
            fgcolor = strdup(BLU);
            bgcolor = strdup(YELB);
            fgColorCode = BLUE_CODE;
            bgColorCode = YELLOW_CODE;
        }
        else if (strcmp(color, "Magenta") == 0 || strcmp(color, "magenta") == 0) {
            fgcolor = strdup(MAG);
            bgcolor = strdup(CYNB);
            fgColorCode = MAGENTA_CODE;
            bgColorCode = CYAN_CODE;
        }
        else if (strcmp(color, "Cyan") == 0 || strcmp(color, "cyan") == 0) {
            fgcolor = strdup(CYN);
            bgcolor = strdup(MAGB);
            fgColorCode = CYAN_CODE;
            bgColorCode = MAGENTA_CODE;
        }
        else if (strcmp(color, "White") == 0 || strcmp(color, "white") == 0) {
            fgcolor = strdup(WHT);
            bgcolor = strdup(BLKB);
            fgColorCode = WHITE_CODE;
            bgColorCode = BLACK_CODE;
        }
        else {
            fgcolor = strdup(GRN);
            bgcolor = strdup("\e[48;5;17m");
            fgColorCode = GREEN_CODE;
            bgColorCode = 17;
        }
    }
    else {
        fgcolor = strdup(GRN);
        bgcolor = strdup("\e[48;5;17m");
        fgColorCode = GREEN_CODE;
        bgColorCode = 17;
    }
    
    // If no text or file was provided, read from stdin
    if (text == NULL && read_from_file == 0) {
        read_from_file = 1;
        input_file = stdin;
        
        printf("Reading from stdin\n");
        
    }

    // 1. Unified Loading
    if (read_from_file) {
        if (input_file == stdin) {
            content = load_stream_to_memory(stdin);
        } else {
            content = load_file_to_memory(input_file);
        }
    } else if (text != NULL) {
        content = wrap_text_to_content(text);

    } else {
        fprintf(stderr, "No text or file provided!\n");
        return 1;
    }

    // 2. The Navigation Loop
    if (content.count > 0) {
        int current_idx = 0;

        print_repeat_str("─", get_terminal_width());
        
        while (current_idx >= 0 && current_idx < content.count) {

            // Check if the current line is an image link: [Image: path/to/image]
            if (strncmp(content.lines[current_idx], "[Image: ", 8) == 0) {
                char *closing_bracket = strchr(content.lines[current_idx], ']');
                
                if (closing_bracket != NULL) {
                    // Extract the path between "[Image: " and "]"
                    char image_path[512];
                    const char *start_of_path = content.lines[current_idx] + 8;
                    size_t path_len = closing_bracket - start_of_path;

                    if (path_len < sizeof(image_path)) {
                        strncpy(image_path, start_of_path, path_len);
                        image_path[path_len] = '\0';

                        // Prepare the command for ascii-image-converter
                        // We wrap the path in quotes in case the filename contains spaces
                        char cmd[1024];
                        snprintf(cmd, sizeof(cmd), 
                                 "ascii-image-converter -b --threshold 96 -C --color-bg \"%s/%s/%s\" 2>/dev/null", 
                                 config_path, "out_txt/images", image_path);

                        // Execute the converter
                        system(cmd);
                        
                        //printf("\nImage: file://%s/%s/%s\n", config_path, "out_txt/images", image_path);
                        printf("\nImage: file://");
                        print_encoded(config_path);
                        printf("/");
                        print_encoded("out_txt/images"); // Opcional, caso mude no futuro
                        printf("/");
                        print_encoded(image_path);
                        printf("\n");
                        
                        // Advance the index and skip the text processing for this line
                        current_idx++;
                        continue; 
                    }
                }
            }

            int result = process_single_line(content.lines[current_idx], 
                                             getenv("PIPER_VOICE_MODEL"), 
                                             getenv("PIPER_VOICE_JSON"), 
                                             get_env_int("SPEAKER_ID", 0), 
                                             espeak_path, 
                                             speed, 
                                             fgcolor, 
                                             bgcolor, 
                                             tty_fd, 
                                             current_idx + 1, 
                                             lang, 
                                             content.count,
                                             volume);
            
            if (result == CMD_BACK) { // Back
                if (current_idx > 0) 
                    current_idx--;
                else {
                    printf("\n[Start of file]\n");
                }
            } else if (result == CMD_FORWARD) { // Forward
                current_idx++;
                if (current_idx >= content.count) 
                    printf("\n[End of file]\n");
                        
            } else if (result == CMD_QUIT) { // Quit
                stop_current_audio();
                printf("\nExiting...\n");
                break;

            } else if (result == CMD_PLAY_PAUSE) { 
                // Pause mode: Do not increment current_idx, just loop again
                continue;

            } else if (result == CMD_BIG_BACK) { // Back+
                if (current_idx > 10) 
                    current_idx -= 10;
                else {
                    current_idx = 0;
                    printf("\n[Start of file]\n");
                }

            } else if (result == CMD_BIG_FORWARD) { // Forward+
                current_idx += 10;
                if (current_idx >= content.count) 
                    printf("\n[End of file]\n");

            } else if (result == CMD_JUMP_BACK) { // Back++
                if (current_idx > 25) 
                    current_idx -= 25;
                else {
                    current_idx = 0;
                    printf("\n[Start of file]\n");
                }

            } else if (result == CMD_JUMP_FORWARD) { // Forward++
                current_idx += 25;
                if (current_idx >= content.count) 
                    printf("\n[End of file]\n");

            } else if (result == CMD_DEC_SPEED) {
                speed = speed / 1.1;
                printf("\n[Speed= ×%.2f]\n", speed);

            } else if (result == CMD_INC_SPEED) {
                speed = speed * 1.1;
                printf("\n[Speed= ×%.2f]\n", speed);

            } else if (result == CMD_HALF_SPEED) {
                speed = speed / 2;
                printf("\n[Speed= ×%.2f]\n", speed);

            } else if (result == CMD_DOUBLE_SPEED) {
                speed = speed * 2;
                printf("\n[Speed= ×%.2f]\n", speed);

            } else if (result == CMD_SPEED_RESET) {
                speed = 1.0;
                printf("\n[Speed= ×%.2f]\n", speed);

            } else if (result == CMD_CHANGE_BG_COLOR) {
                change_bgcolor(&bgcolor, &bgColorCode, &fgColorCode, NEXT_COLOR);

            } else if (result == CMD_CHANGE_BG_COLOR_EXT_PREV) {
                change_bgcolor_ext(&bgcolor, &bgColorCode, &fgColorCode, PREVIOUS_COLOR);

            } else if (result == CMD_CHANGE_BG_COLOR_EXT_NEXT) {
                change_bgcolor_ext(&bgcolor, &bgColorCode, &fgColorCode, NEXT_COLOR);

            } else if (result == CMD_DEFAULT_COLOR) { // reset color scheme
                fgColorCode = GREEN_CODE;
                bgColorCode = 17;
                snprintf(fgcolor, 32, "\e[0;%dm", fgColorCode + 30);
                snprintf(bgcolor, 32, "\e[48;5;%dm", bgColorCode);
                bold = 0;
                dim = 0;
                highlight = 0;
            
            } else if (result == CMD_NEXT_COLOR_SCHEME) {
                set_color(&fgcolor, &fgColorCode, &bgcolor, &bgColorCode, NEXT_COLOR, highlight, bold);

            } else if (result == CMD_TOGGLE_BOLD) {
                bold = (bold == 1) ? 0 : 1;
                change_fgcolor(&fgcolor, &fgColorCode, SAME_COLOR, highlight, bold);

            } else if (result == CMD_TOGGLE_DIM) {
                dim = (dim == 2) ? 0 : 2;
                change_fgcolor(&fgcolor, &fgColorCode, SAME_COLOR, highlight, dim);

            } else if (result == CMD_TOGGLE_HIGHLIGHT) {
                highlight = (highlight == 1) ? 0 : 1;
                change_fgcolor(&fgcolor, &fgColorCode, SAME_COLOR, highlight, 0);

            } else if (result == CMD_CYCLE_LANGUAGES) {
                lang_index++;
                if (lang_index == n_languages) {
                    lang_index = 0;
                }
                snprintf(lang, sizeof(lang), "%s", langlist[lang_index]->d_name);
                char env_path2[256];
                //snprintf(env_path2, sizeof(env_path2), "%s/%s/%s%s", home_dir, program_invocation_short_name, lang, ".env");
                snprintf(env_path2, sizeof(env_path2), "%s/%s%s", config_path, lang, ".env");
                
                load_env_file(env_path2);

            } else if (result == CMD_LOWER_VOLUME) {
                volume = volume -0.1;
                if (volume < 0) {
                    volume = 0;
                }
                printf("\n[Volume= ×%.1f]\n", volume);

            } else if (result == CMD_RAISE_VOLUME) {
                volume = volume + 0.1;
                if (volume > 10) {
                    volume = 10;
                }
                printf("\n[Volume= ×%.1f]\n", volume);

            } else if (result == CMD_COPY_TEXT) {
                printf("\nCurrent sentence:\n\n%s\n\n", content.lines[current_idx]);
            
            } else if (result == 0) {
                // Line finished naturally
                current_idx++;
            }
        }        
    }
    free(lang);

    for (int i = 0; i < content.count; i++) 
        free(content.lines[i]);
    
    free(content.lines);

    free(fgcolor);
    free(bgcolor);

    for (int i = 0; i < n_languages; i++) {
        free(langlist[i]); // Free individual entries
    }
    free(langlist);   

    safe_free((void**)&text);

    reset_terminal_mode(tty_fd, &orig_termios);
    return 0;
}
