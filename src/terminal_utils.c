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

#include "terminal_utils.h"
#include <unistd.h>

void enable_raw_mode(int tty_fd, struct termios *orig_termios) {
    if (tcgetattr(tty_fd, orig_termios) < 0) return;
    
    struct termios raw = *orig_termios;
    
    // ICANON: Disable buffered input (No more waiting for Enter!)
    // ECHO: Disable echoing characters to the current terminal
    raw.c_lflag &= ~(ICANON | ECHO);
    
    tcsetattr(tty_fd, TCSAFLUSH, &raw);
}

void reset_terminal_mode(int tty_fd, struct termios *orig_termios) {
    tcsetattr(tty_fd, TCSAFLUSH, orig_termios);
}
