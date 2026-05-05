/*
* MorceNOX CAT-Reader (C-Art Text Reader)™
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
#include "colors.h"
#include "ANSI-color-codes.h"

void change_fgcolor(char **fgcolor, int *fgColorCode, int increment, int highlight, int style) {
    // Fix modulo math for negative numbers: ((val % 8) + 8) % 8
    int new_code = (*fgColorCode + increment) % 8;
    if (new_code < 0) new_code += 8;
    
    *fgColorCode = new_code; // Update the actual integer in main
    int ansi_val = new_code + 30;

    if (highlight) ansi_val += 60;
    
    // Now safely writing to heap memory
    snprintf(*fgcolor, 32, "\e[%d;%dm", style, ansi_val);
}

void change_bgcolor(char **bgcolor, int *bgColorCode, int *fgColorCode, int increment) {

    int new_code = (*bgColorCode + increment) % 8;
    if (new_code < 0) new_code += 8;
    
    *bgColorCode = new_code;

    if (*bgColorCode == *fgColorCode) {
        new_code = (*bgColorCode + increment) % 8;
        if (new_code < 0) 
            new_code += 8;
        
        *bgColorCode = new_code;
    }
    int ansi_val = new_code + 40;
    
    snprintf(*bgcolor, 32, "\e[%dm", ansi_val);
}


void set_color(char **fgcolor, int *fgColorCode, char **bgcolor, int *bgColorCode, int increment, int highlight, int style) {
    change_fgcolor(fgcolor, fgColorCode, increment, highlight, style);

    int fg[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    int bg[8] = {7, 2, 1, 4, 3, 6, 5, 0};

    *bgColorCode = bg[*fgColorCode];

    int ansi_val = *bgColorCode + 40;
    
    snprintf(*bgcolor, 32, "\e[%dm", ansi_val);

}


void change_fgcolor_ext(char **bgcolor, int *bgColorCode, int *fgColorCode, int increment) {

    int new_code = (*bgColorCode + increment) % 256;
    if (new_code < 0) new_code += 256;
    
    *bgColorCode = new_code;

    if (*bgColorCode == *fgColorCode) {
        new_code = (*bgColorCode + increment) % 256;
        if (new_code < 0) 
            new_code += 256;
        
        *bgColorCode = new_code;
    }
    int ansi_val = new_code;
    
    snprintf(*bgcolor, 32, "\e[38;5;%dm", ansi_val);
}

void change_bgcolor_ext(char **bgcolor, int *bgColorCode, int *fgColorCode, int increment) {

    int new_code = (*bgColorCode + increment) % 256;
    if (new_code < 0) new_code += 256;
    
    *bgColorCode = new_code;

    if (*bgColorCode == *fgColorCode) {
        new_code = (*bgColorCode + increment) % 256;
        if (new_code < 0) 
            new_code += 256;
        
        *bgColorCode = new_code;
    }
    int ansi_val = new_code;
    
    snprintf(*bgcolor, 32, "\e[48;5;%dm", ansi_val);
}