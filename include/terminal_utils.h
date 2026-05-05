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

#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <termios.h>

// Enables raw mode for the provided file descriptor
void enable_raw_mode(int tty_fd, struct termios *orig_termios);

// Restores the terminal to its original state
void reset_terminal_mode(int tty_fd, struct termios *orig_termios);

#endif