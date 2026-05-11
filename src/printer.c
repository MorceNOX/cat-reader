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

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

#include "letters.h"
#include "printer.h"
#include "string_helper.h"
#include "ANSI-color-codes.h"

#define SCREEN_PADDING 3

// Function to get the 7-line array for a character
const char** get_char_array(wchar_t c) {
    setlocale(LC_ALL, "");
    switch (c) {
        case L'0': return zero_7lines;
        case L'1': return one_7lines;
        case L'2': return two_7lines;
        case L'3': return three_7lines;
        case L'4': return four_7lines;
        case L'5': return five_7lines;
        case L'6': return six_7lines;
        case L'7': return seven_7lines;
        case L'8': return eight_7lines;
        case L'9': return nine_7lines;
        case L'A': return A_7lines;
        case L'Á': return acute_A_7lines;
        case L'Â': return circumflex_A_7lines;
        case L'À': return grave_A_7lines;
        case L'Ã': return tilde_A_7lines;
        case L'Å': return ring_A_7lines;
        case L'Ǎ': return caron_A_7lines;
        case L'Æ': return AE_7lines;
        case L'Ä': return trema_A_7lines;
        case L'B': return B_7lines;
        case L'C': return C_7lines;
        case L'Č': return caron_C_7lines;
        case L'Ç': return C_cedilla_7lines;
        case L'D': return D_7lines;
        case L'E': return E_7lines;
        case L'Ë': return trema_E_7lines;
        case L'É': return acute_E_7lines;
        case L'Ê': return circumflex_E_7lines;
        case L'F': return F_7lines;
        case L'G': return G_7lines;
        case L'H': return H_7lines;
        case L'I': return I_7lines;
        case L'Í': return acute_I_7lines;
        case L'Ǐ': return caron_I_7lines;
        case L'Î': return circumflex_I_7lines;
        case L'Ï': return trema_I_7lines;
        case L'J': return J_7lines;
        case L'K': return K_7lines;
        case L'L': return L_7lines;
        case L'Ł': return L_cortado_7lines;
        case L'M': return M_7lines;
        case L'N': return N_7lines;
        case L'Ñ': return tilde_N_7lines;
        case L'O': return O_7lines;
        case L'Ó': return acute_O_7lines;
        case L'Ô': return circumflex_O_7lines;
        case L'Õ': return tilde_O_7lines;
        case L'Ö': return trema_O_7lines;
        case L'Ø': return O_cortado_7lines;
        case L'P': return P_7lines;
        case L'Q': return Q_7lines;
        case L'R': return R_7lines;
        case L'S': return S_7lines;
        case L'Ś': return acute_S_7lines;
        case L'Š': return caron_S_7lines;
        case L'ẞ': return scharfes_S_7lines;
        case L'Ș': return S_cedilla_7lines;
        case L'T': return T_7lines;
        case L'Ț': return T_cedilla_7lines;
        case L'U': return U_7lines;
        case L'Ú': return acute_U_7lines;
        case L'Ü': return trema_U_7lines;
        case L'V': return V_7lines;
        case L'W': return W_7lines;
        case L'X': return X_7lines;
        case L'Y': return Y_7lines;
        case L'Ÿ': return Y_trema_7lines;
        case L'Z': return Z_7lines;
        case L'Ž': return caron_Z_7lines;
        case L'a': return a_7lines;
        case L'á': return acute_a_7lines;
        case L'â': return circumflex_a_7lines;
        case L'à': return grave_a_7lines;
        case L'ã': return tilde_a_7lines;
        case L'å': return ring_a_7lines;
        case L'ǎ': return caron_a_7lines;
        case L'æ': return ae_7lines;
        case L'ä': return trema_a_7lines;
        case L'b': return b_7lines;
        case L'c': return c_7lines;
        case L'ç': return c_cedilla_7lines;
        case L'č': return caron_c_7lines;
        case L'd': return d_7lines;
        case L'e': return e_7lines;
        case L'ë': return trema_e_7lines;
        case L'é': return acute_e_7lines;
        case L'ê': return circumflex_e_7lines;
        case L'f': return f_7lines;
        case L'g': return g_7lines;
        case L'h': return h_7lines;
        case L'i': return i_7lines;
        case L'í': return acute_i_7lines;
        case L'ǐ': return caron_i_7lines;
        case L'î': return circumflex_i_7lines;
        case L'ï': return trema_i_7lines;
        case L'j': return j_7lines;
        case L'k': return k_7lines;
        case L'l': return l_7lines;
        case L'ł': return l_cortado_7lines;
        case L'm': return m_7lines;
        case L'n': return n_7lines;
        case L'ñ': return tilde_n_7lines;
        case L'o': return o_7lines;
        case L'ó': return acute_o_7lines;
        case L'ô': return circumflex_o_7lines;
        case L'õ': return tilde_o_7lines;
        case L'ö': return trema_o_7lines;
        case L'ø': return o_cortado_7lines;
        case L'p': return p_7lines;
        case L'q': return q_7lines;
        case L'r': return r_7lines;
        case L's': return s_7lines;
        case L'ś': return acute_s_7lines;
        case L'š': return caron_s_7lines;
        case L'ß': return scharfes_s_7lines;
        case L'ș': return s_cedilla_7lines;
        case L't': return t_7lines;
        case L'ț': return t_cedilla_7lines;
        case L'u': return u_7lines;
        case L'ú': return acute_u_7lines;
        case L'ü': return trema_u_7lines;
        case L'v': return v_7lines;
        case L'w': return w_7lines;
        case L'x': return x_7lines;
        case L'y': return y_7lines;
        case L'ÿ': return y_trema_7lines;
        case L'z': return z_7lines;
        case L'ž': return caron_z_7lines;
        case L'.': return dot_7lines;
        case L',': return comma_7lines;
        case L':': return colon_7lines;
        case L';': return semicolon_7lines;
        case L'?': return question_7lines;
        case L'¿': return inverted_question_7lines;
        case L'!': return exclamation_7lines;
        case L'¡': return inverted_exclamation_7lines;
        case L'ƒ': return function_7lines;
        case L'@': return at_7lines;
        case L'#': return hash_7lines;
        case L'$': return dolar_7lines;
        case L'%': return percent_7lines;
        case L'&': return and_7lines;
        case L'°': return degree_7lines;
        case L'º': return ordinal_7lines;
        case L'ª': return ordinalA_7lines;
        case L'*': return asterisk_7lines;
        case L'•': return bullet_7lines;
        case L' ': return space_7lines;
        case L'+': return plus_7lines;
        case L'-': return minus_7lines;
        case L'±': return plus_minus_7lines;
        case L'÷': return division_7lines;
        case L'×': return times_7lines;
        case L'√': return square_root_7lines;
        case L'∛': return cubic_root_7lines;
        case L'∜': return forth_root_7lines;
        case L'²': return two_super_7lines;
        case L'³': return three_super_7lines;
        case L'¹': return one_super_7lines;
        case L'⁴': return four_super_7lines;
        case L'½': return one_half_7lines;
        case L'⅓': return one_third_7lines;
        case L'¼': return one_fourth_7lines;
        case L'μ': return mu_7lines;
        case L'α': return alpha_7lines;
        case L'β': return beta_7lines;
        case L'δ': return delta_7lines;
        case L'θ': return theta_7lines;
        case L'γ': return gama_7lines;
        case L'ι': return iota_7lines;
        case L'λ': return lambda_7lines;
        case L'π': return pi_7lines;
        case L'ρ': return ro_7lines;
        case L'ω': return omega_7lines;
        case L'_': return underline_7lines;
        case L'—': return em_dash_7lines;
        case L'–': return en_dash_7lines;
        case L'=': return equal_7lines;
        case L'≅': return aprox_equal_7lines;
        case L'(': return open_parenthesis_7lines;
        case L')': return close_parenthesis_7lines;
        case L'[': return open_bracket_7lines;
        case L']': return close_bracket_7lines;
        case L'{': return open_brace_7lines;
        case L'}': return close_brace_7lines;
        case L'<': return less_than_7lines;
        case L'>': return greater_than_7lines;
        case L'\'': return single_quote_7lines;
        case L'"': return double_quote_7lines;
        case L'`': return grave_7lines;
        case L'^': return circumflex_7lines;
        case L'ˇ': return caron_7lines;
        case L'˚': return ring_7lines;
        case L'~': return tilde_7lines;
        case L'¨': return trema_7lines;
        case L'\\': return backslash_7lines;
        case L'/': return slash_7lines;
        case L'|': return pipe_7lines;
        case L'│': return box_single_vertical_7lines;
        case L'─': return box_single_horizontal_7lines;
        case L'§': return section_7lines;
        case L'¶': return paragraph_7lines;
        case L'£': return pound_7lines;
        case L'¥': return yen_7lines;
        case L'€': return euro_7lines;
        case L'¢': return cent_7lines;
        case L'©': return copyright_7lines;
        case L'™': return tm_7lines;
        case L'´': return acute_7lines;
        case L'‘': return open_single_quote_7lines;
        case L'“': return open_double_quote_7lines;
        case L'’': return close_single_quote_7lines;
        case L'”': return close_double_quote_7lines;
        case L'↓': return arrow_down_7lines;
        case L'←': return arrow_left_7lines;
        case L'↑': return arrow_up_7lines;
        case L'→': return arrow_right_7lines;
        case L'☉': return sol_7lines;
        case L'☾': return lua_7lines;
        case L'☿': return mercury_7lines;
        case L'♀': return venus_7lines;
        case L'♂': return marte_7lines;
        case L'♃': return jupiter_7lines;
        case L'♄': return saturno_7lines;
        case L'⯓': return pluto_7lines;
        case L'♇': return Pluto_7lines;
        case L'♅': return urano_7lines;
        case L'♆': return netuno_7lines;
        case L'🜨': return terra_7lines;
        case L'🝴': return fortuna_7lines;
        case L'℞': return retrograde_7lines;
        case L'☌': return conjunction_7lines;
        case L'△': return trine_7lines;
        case L'☍': return opposition_7lines;
        case L'⚹': return sextile_7lines;
        case L'□': return square_7lines;
        case L'☊': return caput_draconis_7lines; // ascending node
        case L'☋': return cauda_draconis_7lines; // descending node
        case L'\u2648': // Aries
            return aries_7lines;
        case L'\u2649': // Taurus
            return taurus_7lines;
        case L'\u264A': // Gemini
            return gemini_7lines;
        case L'\u264B': // Cancer
            return cancer_7lines;
        case L'\u264C': // Leo
            return leo_7lines;
        case L'\u264D': // Virgo
            return virgo_7lines;
        case L'\u264E': // Libra
            return libra_7lines;
        case L'\u264F': // Scorpio
            return scorpio_7lines;
        case L'\u2650': // Sagittarius
            return sagittarius_7lines;
        case L'\u2651': // Capricorn
            return capricorn_7lines;
        case L'\u2652': // Aquarius
            return aquarius_7lines;
        case L'\u2653': // Pisces
            return pisces_7lines;
        case L'🜍': // Sulphur
            return sulphur_7lines;
        case L'🜔': // Salt
            return salt_7lines;
        case L'🜄': // water
            return water_7lines;
        case L'🜂': // fire
            return fire_7lines;
        case L'🜁': // air
            return air_7lines;
        case L'🜃': // earth
            return earth_7lines;
        case L'🜏': // black sulphur
            return black_sulphur_7lines;
        case L'○': // white_circle
            return white_circle_7lines;
        case L'●': // black_circle
            return white_circle_7lines;
        case L'■': // black_square
            return white_square_7lines;
        case L'∴': // therefore
            return therefore_7lines;
        case L'…': // ellipsis
            return ellipsis_7lines;
        default: return unknown_7lines;
    }
}


// 1. Unicode-aware width calculation
int calculate_approx_width(const char* str) {
    if (!str) return 0;

    wchar_t* wstr = utf8_to_wchar(str);
    if (!wstr) return 0;

    int str_len = wcslen(wstr);

    int result = 0;

    for (int i = 0; i < str_len; i++) {
        wchar_t c = wstr[i];

        int letter_width = strlen((get_char_array(c))[0]);

        result = result + letter_width;
    }

    // for padding 
    result = result + SCREEN_PADDING;

    free(wstr);
    return result;
}

// 2. Unicode-safe splitting
void print_split_string_logic(const wchar_t* wstr, size_t start, size_t end) {
    size_t len = end - start;
    wchar_t* wpart = malloc((len + 1) * sizeof(wchar_t));
    if (!wpart) return;

    wcsncpy(wpart, wstr + start, len);
    wpart[len] = L'\0';

    char* utf8_part = wchar_to_utf8(wpart);
    if (utf8_part) {
        print_words(utf8_part);
        free(utf8_part);
    }
    free(wpart);
}

void print_split_word(const char* str, int max_width) {
    wchar_t* wstr = utf8_to_wchar(str);
    if (!wstr) {
        print_words(str);
        return;
    }

    if (calculate_approx_width(str) < max_width) {
        print_words(str);
    } else {
        size_t len = wcslen(wstr);
        size_t mid = len / 2;

        if (mid * 4 + SCREEN_PADDING + 5 < (size_t)max_width) {

            // Instead of manual malloc/strncpy, use the helper above
            // to split the wide string and convert it back to UTF-8
            print_split_string_logic(wstr, 0, mid);
            printf("\n");
            
            print_words("-"); 
            printf("\n");

            print_split_string_logic(wstr, mid, len);
        } else {

            size_t third = len / 3;

            print_split_string_logic(wstr, 0, third);
            printf("\n");
            
            print_words("-"); 
            printf("\n");

            print_split_string_logic(wstr, third, len - third);

            print_words("-"); 
            printf("\n");

            print_split_string_logic(wstr, len - third, len);
        }
    }
    free(wstr);
}


// 3. The Printer
int printer(int argc, char* argv[], int line_num, int n_lines, const char *language, float speed, float volume) {
    setlocale(LC_ALL, "");
    int terminal_width = get_terminal_width();

    // --- NEW: Print the Status Bar Footer in ASCII art ---
    char bar_str[100];
    snprintf(bar_str, sizeof(bar_str), "%s│× %.2f│v %.1f│¶ %d/%d", language, speed, volume, line_num, n_lines);
    
    
    // We use a stable buffer to accumulate words for the current line
    char current_line[MAX_CHARS];
    strcpy(current_line, argv[0]); 

    // Start loop from the second argument
    for (int i = 1; i < argc; i++) {
        char* next_word = argv[i];
        
        // Create a temporary string to test the "combined" width
        char combined[MAX_CHARS];
        
        snprintf(combined, sizeof(combined), "%s %s", current_line, next_word);

        if (calculate_approx_width(combined) < terminal_width) {
            // It fits! Update our current line buffer
            strcpy(current_line, combined);
        } else {
            // It doesn't fit. Print what we have (with potential split)
            print_split_word(current_line, terminal_width);
            printf("\n");

            // Start a new line with the next word
            strcpy(current_line, next_word);
        }

        // (nanosleep logic here)
    }

    // Print the final remaining part of the line
    print_split_word(current_line, terminal_width);
    
    
    printf("\n\n\n");

    printf("\e[0m");
    printf("\e[48;5;16m");
    print_repeat_str("─", terminal_width);
    
    printf(HBLK);
    printf("\e[48;5;16m");
    print_repeat_str(" ", terminal_width);
    printf("\r");
    
    print_words(bar_str); // This renders the number in big letters!
    
    printf("\e[0m");
    printf("\e[48;5;16m");
    print_repeat_str("─", terminal_width);
    
    
    printf("\e[37;40m");
    print_repeat_str(" ", terminal_width);
    printf("\r");
    printf(" ◀◀(S)◀(A) │ (P)▶⏸ │ (D)▶(W)▶▶ ┃ (Q)(⎋) Quit ┃ (B)Bold (N)Dim (H)High ┃ 🐇 (])+ ([)- (⌫)⮌ ┃ 🔊(+/-) ┃ (X)(C)🎨 ┃ (L)🌐\n");
    
    printf("\e[0m");
    printf("\e[48;5;16m");
    print_repeat_str("─", terminal_width);
    printf("\n");
    printf("\e[0m");

    return 0;
}





// Print ASCII art for letters
void print_words(const char* word) {
    setlocale(LC_ALL, "");
    mbstate_t state = {0};

    int terminal_width = get_terminal_width();
    
    // Use a pointer to the pointer for mbsrtowcs
    const char* ptr = word; 
    size_t len = mbsrtowcs(NULL, &ptr, 0, &state);
    
    if (len == (size_t)-1) {
        fprintf(stderr, "Error converting string to wide characters\n");
        return;
    }

    wchar_t* wide_word = malloc((len + 1) * sizeof(wchar_t));
    if (!wide_word) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    mbsrtowcs(wide_word, &word, len + 1, &state);

    // Initialize result lines
    char lines[7][MAX_LINE_LENGTH];
    for (int i = 0; i < LETTER_LINES; i++) {
        lines[i][0] = '\0';
        
    }

    // Process each wide character
    for (size_t i = 0; i < len; i++) {
        wchar_t c = wide_word[i];
        const char** current_char_array = get_char_array(c);

        // Combine with existing lines
        for (int j = 0; j < LETTER_LINES; j++) {
            strcat(lines[j], current_char_array[j]);
            // Every ASCII art letter already contains a space, so if you add this
            // extra space the text will not be wrapped correctly!

            // // Add space for most characters except specific ones
            // if (c != L'm' && c != L'w' && c != L'Ł' && c != L'ẞ') {
            //     strcat(lines[j], "");
            // }
        }
    }

    // Print the result
    for (int i = 0; i < LETTER_LINES; i++) {
        // Fill each line with the bg color to avoid spaces without color
        print_repeat_str(" ", terminal_width);
        printf("\r");

        // print padding spaces on the beggining of the terminal screen
        for (int j = 0; j < SCREEN_PADDING; j++) {
          printf(" ");
        }

        printf("%s\n", lines[i]);

        
    }

    free(wide_word);
    wide_word = NULL;
}

unsigned short get_terminal_width(void) {
    struct winsize ws;
    // Try STDIN_FILENO, STDOUT_FILENO, and STDERR_FILENO in case one is
    // redirected
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == 0) {
        return ws.ws_col;
    } else if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        return ws.ws_col;
    } else if (ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) == 0) {
        return ws.ws_col;
    }

    return 80;
}
