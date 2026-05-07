/*
* MorceNOX Art-Reader™
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

#ifndef PRINTER_H
#define PRINTER_H

#define MAX_LINES 7
#define MAX_CHARS 1000
#define MAX_LINE_LENGTH 2048
#define PRINT_LINE_DELAY 0L
#define LETTER_LINES 6

const char **get_char_array(wchar_t c);
int calculate_approx_width(const char* str);
void print_split_word(const char* str, int max_width);
void print_words(const char *word);
unsigned short get_terminal_width(void);
int printer(int argc, char *argv[], int line_num, int n_lines, const char *language, float speed, float volume);
#endif
