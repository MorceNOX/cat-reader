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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include "app.h"

void setup_config_dir(const char *app_name, char config_path[]) {
    const char *xdg_config = getenv("XDG_CONFIG_HOME");

    if (xdg_config) {
        // Se XDG_CONFIG_HOME estiver definida
        snprintf(config_path, 512, "%s/%s", xdg_config, app_name);
    } else {
        // Caso contrário, usa ~/.config
        const char *home = getenv("HOME");
        if (!home) {
            home = getpwuid(getuid())->pw_dir; // Fallback mais seguro
        }
        snprintf(config_path, 512, "%s/.config/%s", home, app_name);
    }

    // Cria o diretório (755 = rwxr-xr-x)
    struct stat st = {0};
    if (stat(config_path, &st) == -1) {
        if (mkdir(config_path, 0755) == 0) {
            printf("Pasta de configuração criada em: %s\n", config_path);
        }
    }
}