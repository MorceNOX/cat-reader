CC = gcc
# Note que alteramos o rpath para onde as libs realmente estarão no sistema
LDFLAGS = -Llib/libpiper -lpiper -Llib/libpiper/lib64 -lonnxruntime -Wl,-rpath,'$(LIBEXECDIR)/libpiper:$(LIBEXECDIR)/libpiper/lib64'
CFLAGS = -O3 -std=gnu99
LIBS = -ljson-c -Llib/libpiper -lpiper -Llib/libpiper/lib64 -lonnxruntime
SRCDIR = src
INCDIR = include
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)

APP_NAME = cat-reader

TARGET = $(APP_NAME)-engine
WRAPPER = $(APP_NAME).sh
SPLITTER = split_sentences.py

# Convenções de caminhos Linux
PREFIX     ?= /usr/local
BINDIR     ?= $(PREFIX)/bin
LIBEXECDIR ?= $(PREFIX)/libexec/$(APP_NAME)
DATADIR    ?= $(PREFIX)/share/$(APP_NAME)

CFLAGS += -DDATADIR=\"$(DATADIR)\"
CFLAGS += -DTARGET=\"$(TARGET)\"

.PHONY: all clean run install uninstall

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	# 1. Cria diretórios
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(LIBEXECDIR)/libpiper/lib64
	install -d $(DESTDIR)$(DATADIR)/models
	install -d $(DESTDIR)$(DATADIR)/out
	install -d $(DESTDIR)$(DATADIR)/out_txt

	# 2. Instala o binário C no LIBEXEC (escondido)
	install -m 755 $(TARGET) $(DESTDIR)$(LIBEXECDIR)/$(TARGET)

    # 3. Instala o splitter_sentences.py script no LIBEXEC (escondido)
	install -m 755 $(SPLITTER) $(DESTDIR)$(LIBEXECDIR)/$(SPLITTER)

	# 4. Instala o Wrapper Bash no BIN renomeando para o nome oficial
	sed 's|BASE_PATH=.*|BASE_PATH="$(PREFIX)"|' $(WRAPPER) > $(DESTDIR)$(BINDIR)/$(APP_NAME)
	chmod +x $(DESTDIR)$(BINDIR)/$(APP_NAME)

	# 5. Copia as Bibliotecas e Assets
	cp -r lib/libpiper/* $(DESTDIR)$(LIBEXECDIR)/libpiper/
	cp -r lib/espeak-ng-data $(DESTDIR)$(DATADIR)/
	cp -r assets/models/* $(DESTDIR)$(DATADIR)/models/
	
	# 6. Instala configurações globais (opcional)
	if [ -d config ]; then cp -r config/* $(DESTDIR)$(DATADIR)/ 2>/dev/null || true; fi

	# Injeta o PREFIX real dentro do script bash ao instalar
	sed -i 's|INSTALL_PREFIX=.*|INSTALL_PREFIX=$(PREFIX)|' $(DESTDIR)$(BINDIR)/$(APP_NAME)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(APP_NAME)
	rm -rf $(DESTDIR)$(LIBEXECDIR)
	rm -rf $(DESTDIR)$(DATADIR)
