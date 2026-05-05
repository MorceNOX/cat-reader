#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include "app.h"

void setup_config_dir(const char *app_name, char **config_path) {
    const char *xdg_config = getenv("XDG_CONFIG_HOME");

    if (xdg_config) {
        // Se XDG_CONFIG_HOME estiver definida
        snprintf(config_path, sizeof(config_path), "%s/%s", xdg_config, app_name);
    } else {
        // Caso contrário, usa ~/.config
        const char *home = getenv("HOME");
        if (!home) {
            home = getpwuid(getuid())->pw_dir; // Fallback mais seguro
        }
        snprintf(config_path, sizeof(config_path), "%s/.config/%s", home, app_name);
    }

    // Cria o diretório (755 = rwxr-xr-x)
    struct stat st = {0};
    if (stat(config_path, &st) == -1) {
        if (mkdir(config_path, 0755) == 0) {
            printf("Pasta de configuração criada em: %s\n", config_path);
        }
    }
}