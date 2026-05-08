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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "string_helper.h"


// Helper: Converts UTF-8 char* to wide char* (Unicode-aware)
// This allows us to iterate through actual characters, not just bytes.
wchar_t* utf8_to_wchar(const char* str) {
    if (!str) return NULL;
    setlocale(LC_ALL, ""); // Ensure locale is set for conversion
    
    mbstate_t state = {0};
    // Determine required size for wide string
    size_t len = mbsrtowcs(NULL, &str, 0, &state);
    if (len == (size_t)-1) return NULL;

    wchar_t* wstr = malloc((len + 1) * sizeof(wchar_t));
    if (!wstr) return NULL;

    // Perform the actual conversion
    mbsrtowcs(wstr, &str, len + 1, &state);
    return wstr;
}

// Helper: Converts wide char* back to UTF-8 char*
char* wchar_to_utf8(const wchar_t* wstr) {
    if (!wstr) return NULL;
    setlocale(LC_ALL, "");
    
    mbstate_t state = {0};
    size_txt:
    size_t len = wcslen(wstr);
    
    // Allocate enough space (max 4 bytes per char + null)
    char* str = malloc((len * 4) + 1);
    if (!str) return NULL;

    char* dest = str;
    for (size_t i = 0; i < len; i++) {
        // wcrtomb converts one wide char to multibyte and returns bytes written
        size_t bytes_written = wcrtomb(dest, wstr[i], &state);
        if (bytes_written == (size_t)-1) {
            *dest = '\0'; 
            break;
        }
        dest += bytes_written; // Move the pointer by the ACTUAL number of bytes used
    }
    *dest = '\0'; // Null terminate
    return str;
}



void get_substring(const char *source, int start, int len, char *buffer) {
    // Copy 'len' characters starting from source[start]
    strncpy(buffer, source + start, len);
    // Manually null-terminate the new string
    buffer[len] = '\0';
}
  
// New, Unicode-aware substring function
void get_substring_safe(const char *source, int start, int len, char *buffer) {
    if (!source || !buffer) return;

    // 1. Convert the UTF-8 source to wide characters (Unicode-aware)
    wchar_t* wsrc = utf8_to_wchar(source);
    if (!wsrc) return;

    // 2. Calculate the end position in terms of wide characters
    // We use 'start' as the character index, not byte index
    size_t wlen = wcslen(wsrc);
    if ((size_t)start >= wlen) {
        buffer[0] = '\0';
        free(wsrc);
        return;
    }

    size_t end = (size_t)start + len;
    if (end > wlen) end = wlen;

    // 3. Extract the wide-character part
    size_t part_len = end - start;
    wchar_t* wpart = malloc((part_len + 1) * sizeof(wchar_t));
    if (wpart) {
        wcsncpy(wpart, wsrc + start, part_len);
        wpart[part_len] = L'\0';

        // 4. Convert the part back to UTF-8 for the buffer
        char* utf8_part = wchar_to_utf8(wpart);
        if (utf8_part) {
            strcpy(buffer, utf8_part);
            free(utf8_part);
        }
        free(wpart);
    }

    free(wsrc);
}

void removeConsecutiveSpaces(char* str) {
  if (str == NULL) return;

  char* src = str;  // Source pointer to read
  char* dst = str;  // Destination pointer to write
  int lastWasSpace = 0;

  while (*src != '\0') {
    if (*src == ' ') {
      if (!lastWasSpace) {
        *dst++ = *src;  // Copy only the first space found
        lastWasSpace = 1;
      }
    } else {
      *dst++ = *src;  // Copy all non-space characters
      lastWasSpace = 0;
    }
    src++;
  }

  *dst = '\0';  // Null-terminate the new shorter string
}

int is_null_or_whitespace(char* str) {
  if (str == NULL || str[0] == '\0') return 1;

  while (*str) {
    if (!isspace((unsigned char)*str) && !isblank((unsigned char)*str))
      return 0;
    str++;
  }
  return 1;
}

char* trim(char* s) {
  if (s == NULL) return NULL;  // Handle NULL pointer
  if (*s == '\0') return s;    // Handle empty string

  // 1. Trim trailing space
  char* end = s + strlen(s) - 1;
  while (end >= s && isspace((unsigned char)*end)) {
    end--;
  }
  *(end + 1) = '\0';  // Truncate

  // 2. Trim leading space
  char* start = s;
  while (*start && isspace((unsigned char)*start)) {
    start++;
  }

  // 3. Shift remaining string
  if (start != s) {
    memmove(s, start, strlen(start) + 1);
  }

  return s;
}

void print_repeat_char(char c, int n) {
  char buffer[n + 1];
  memset(buffer, c, n);
  buffer[n] = '\0';
  printf("%s", buffer);
}

void print_repeat_str(const char* s, int n) {
  for (int i = 0; i < n; i++) {
    printf("%s", s);
  }
}

int text_to_argv(const char* text, char** argv, int max_args) {
  if (text == NULL || argv == NULL || max_args <= 0) {
    return 0;
  }

  int argc = 0;
  char* temp_text = strdup(text);  // Make a copy to work with
  char* saveptr = NULL;

  // Tokenize the text by spaces
  char* token = strtok_r(temp_text, " ", &saveptr);
  while (token != NULL && argc < max_args - 1) {
    argv[argc] = strdup(token);
    argc++;
    token = strtok_r(NULL, " ", &saveptr);
  }

  argv[argc] = NULL;  // Null terminate the array

  free(temp_text);
  temp_text = NULL;
  return argc;
}
