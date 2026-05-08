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

#ifndef COLORS_H
#define COLORS_H

#define BLACK_CODE 0
#define RED_CODE 1
#define GREEN_CODE 2
#define YELLOW_CODE 3
#define BLUE_CODE 4
#define MAGENTA_CODE 5
#define CYAN_CODE 6
#define WHITE_CODE 7

#define NEXT_COLOR 1
#define PREVIOUS_COLOR -1
#define SAME_COLOR 0

typedef struct {
    int fgColor;
    int bgColor;
    int style;
} ColorEnv;

void change_fgcolor(char **fgcolor, int *fgColorCode, int increment, int highlight, int style);
void change_bgcolor(char **bgcolor, int *bgColorCode, int *fgColorCode, int increment);
void set_color(char **fgcolor, int *fgColorCode, char **bgcolor, int *bgColorCode, int increment, int highlight, int style);
void change_fgcolor_ext(char **bgcolor, int *bgColorCode, int *fgColorCode, int increment);
void change_bgcolor_ext(char **bgcolor, int *bgColorCode, int *fgColorCode, int increment);
#endif