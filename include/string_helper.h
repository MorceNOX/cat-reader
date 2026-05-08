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

#ifndef STRING_HELPER_H
#define STRING_HELPER_H

wchar_t* utf8_to_wchar(const char* str);
char* wchar_to_utf8(const wchar_t* wstr);
void get_substring(const char *source, int start, int len, char *buffer);
void get_substring_safe(const char *source, int start, int len, char *buffer);
void removeConsecutiveSpaces(char* str);
int is_null_or_whitespace(char* str);
char* trim(char* s);
void print_repeat_char(char c, int n);
void print_repeat_str(const char* s, int n);
int text_to_argv(const char* text, char** argv, int max_args);
#endif
