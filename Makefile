CC = gcc
LDFLAGS = -Llib/libpiper -Llib/libpiper/lib64 -Wl,-rpath,'$(LIBEXECDIR)/libpiper:$(LIBEXECDIR)/libpiper/lib64'
CFLAGS = -O3 -std=gnu11 -Wextra
LIBS    = -ljson-c -lpiper -lonnxruntime -lSDL2 -lpthread
SRCDIR = src
INCDIR = include
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

APP_NAME = cat-reader

TARGET = $(APP_NAME)-engine
WRAPPER = $(APP_NAME).sh
SPLITTER = split_sentences.py

PREFIX     ?= /usr/local
BINDIR     ?= $(PREFIX)/bin
LIBEXECDIR ?= $(PREFIX)/libexec/$(APP_NAME)
DATADIR = $(HOME)/.config/$(APP_NAME)

# Release/Packaging Variables
VERSION = 1.4.2
PACKAGE_NAME = $(APP_NAME)-$(VERSION)
DIST_DIR = release/$(PACKAGE_NAME)

CFLAGS += -DDATADIR=\"$(DATADIR)\"
CFLAGS += -DTARGET=\"$(TARGET)\"
CFLAGS += -DAPP_NAME=\"$(APP_NAME)\"

.PHONY: all clean run install uninstall install-user copy-libs package

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Standard system installation (for /usr/local)
install: $(TARGET)
    # 1. Cria diretórios
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(LIBEXECDIR)
	install -d $(DESTDIR)$(LIBEXECDIR)/libpiper/lib64

    # 2. Instala o binário C no LIBEXEC (escondido)
	install -m 755 $(TARGET) $(DESTDIR)$(LIBEXECDIR)/$(TARGET)
    # cp $(TARGET) $(DESTDIR)$(BINDIR)/

    # 3. Instala o splitter_sentences.py script no LIBEXEC (escondido)
	install -m 755 $(SPLITTER) $(DESTDIR)$(LIBEXECDIR)/$(SPLITTER)

    # 4. Instala o Wrapper Bash injetando os caminhos corretos
	sed -e 's|BASE_PATH=.*|BASE_PATH="$(PREFIX)"|' \
	    -e 's|INSTALL_PREFIX=.*|INSTALL_PREFIX="$(PREFIX)"|' \
		-e 's|APP_NAME=.*|APP_NAME="$(APP_NAME)"|' \
		-e 's|VERSION=.*|VERSION="$(VERSION)"|' \
	    $(WRAPPER) > $(DESTDIR)$(BINDIR)/$(APP_NAME)
	chmod +x $(DESTDIR)$(BINDIR)/$(APP_NAME)

    # 5. Instala as bibliotecas
	cp -r lib/libpiper/* $(DESTDIR)$(LIBEXECDIR)/libpiper/

# User-space installation (for $HOME/.config/cat-reader)
install-user: $(TARGET)
	mkdir -p $(DATADIR)/models
	mkdir -p $(DATADIR)/out_txt
	
#	Copy espeak-ng-data from your project's lib dir to the user's DATADIR
	cp -r lib/espeak-ng-data $(DATADIR)/

#	Copy the models from your project's models dir to the user's DATADIR
	cp -r assets/models/* $(DATADIR)/models/

#	Copy the image.png file from your project's assets dir to the user's DATADIR
	cp -r assets/image.png $(DATADIR)/

#	Copy the help.txt file from your project's assets dir to the user's DATADIR
	cp -r assets/help.txt $(DATADIR)/

#	Copy default configs from your project's config dir to the user's DATADIR
	if [ -d config ]; then cp -r config/* $(DATADIR)/ 2>/dev/null || true; fi

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(APP_NAME)
	rm -rf $(DESTDIR)$(LIBEXECDIR)

# --- PORTABLE RELEASE GENERATION ---
# This creates a single folder containing everything needed to run without installing.
package: clean $(TARGET)
	@echo "Packaging $(PACKAGE_NAME)..."
	mkdir -p $(DIST_DIR)/bin
	mkdir -p $(DIST_DIR)/libexec
	mkdir -p $(DIST_DIR)/lib/libpiper/lib64
	mkdir -p $(DIST_DIR)/assets/models
	mkdir -p $(DIST_DIR)/config

#	1. Copy the engine and tools
	cp $(TARGET) $(DIST_DIR)/libexec/$(TARGET)
	cp $(SPLITTER) $(DIST_DIR)/libexec/$(SPLITTER)

#	2. Copy the wrapper (we don't use sed here, we want it to be relocatable)
	cp $(WRAPPER) $(DIST_DIR)/bin/$(APP_NAME)
	chmod +x $(DIST_DIR)/bin/$(APP_NAME)

#	3. Copy runtime libraries
	cp -r lib/libpiper/* $(DIST_DIR)/lib/libpiper/

#	4. Copy assets
	cp -r assets/models/* $(DIST_DIR)/assets/models/ 2>/dev/null || true
	cp -r assets/image.png $(DIST_DIR)/assets/ 2>/dev/null || true
	cp -r assets/help.txt $(DIST_DIR)/assets/ 2>/dev/null || true
	cp -r config/* $(DIST_DIR)/config/ 2>/dev/null || true

#	5. Compress
	tar -czvf $(PACKAGE_NAME).tar.gz -C release $(PACKAGE_NAME)
	@echo "Done! Upload $(PACKAGE_NAME).tar.gz to GitHub Releases."