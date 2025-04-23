# Compilador e flags
CC = gcc
PKGCONFIG = pkg-config

# Flags do GTK+
GTK_CFLAGS := $(shell $(PKGCONFIG) --cflags gtk+-3.0 cairo)
GTK_LIBS := $(shell $(PKGCONFIG) --libs gtk+-3.0 cairo)

# Flags de compilação
CFLAGS = -Wall -Wextra -g $(GTK_CFLAGS)
LIBS = $(GTK_LIBS) -lm

# Diretórios
SRC_DIR = src
UI_DIR = $(SRC_DIR)/ui
BUILD_DIR = build
DATA_DIR = data

# Arquivos fonte
SOURCES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(UI_DIR)/*.c)
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJECTS:.o=.d)

# Nome do executável
TARGET = sistema_gestao

# Regras
.PHONY: all clean directories

all: directories $(BUILD_DIR)/$(TARGET)

# Criar diretórios necessários
directories:
	@mkdir -p $(BUILD_DIR)/$(SRC_DIR) $(BUILD_DIR)/$(UI_DIR) $(DATA_DIR)

# Regra para o executável
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# Regra para objetos
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Incluir dependências geradas
-include $(DEPS)

# Limpar arquivos gerados
clean:
	rm -rf $(BUILD_DIR)

# Regra para instalar dependências no Ubuntu/Debian
install-deps-debian:
	sudo apt-get update
	sudo apt-get install -y build-essential pkg-config libgtk-3-dev

# Regra para instalar dependências no Fedora
install-deps-fedora:
	sudo dnf install -y gcc pkg-config gtk3-devel

# Regra para instalar dependências no Arch Linux
install-deps-arch:
	sudo pacman -S --needed base-devel pkg-config gtk3

# Regra para executar o programa
run: all
	./$(BUILD_DIR)/$(TARGET)

# Regra para criar backup
backup:
	@mkdir -p backup
	tar czf backup/$(TARGET)-$(shell date +%Y%m%d-%H%M%S).tar.gz \
		$(SRC_DIR) Makefile README.md

# Regra para instalar o programa
install: all
	sudo install -m 755 $(BUILD_DIR)/$(TARGET) /usr/local/bin/
	sudo mkdir -p /usr/local/share/$(TARGET)
	sudo cp -r $(DATA_DIR) /usr/local/share/$(TARGET)/

# Regra para desinstalar o programa
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	sudo rm -rf /usr/local/share/$(TARGET)

# Mensagem de ajuda
help:
	@echo "Makefile para o Sistema de Gestão"
	@echo ""
	@echo "Comandos disponíveis:"
	@echo "  make              - Compila o programa"
	@echo "  make clean        - Remove arquivos gerados"
	@echo "  make run          - Compila e executa o programa"
	@echo "  make install      - Instala o programa no sistema"
	@echo "  make uninstall    - Remove o programa do sistema"
	@echo "  make backup       - Cria um backup do código fonte"
	@echo "  make install-deps-debian  - Instala dependências no Ubuntu/Debian"
	@echo "  make install-deps-fedora  - Instala dependências no Fedora"
	@echo "  make install-deps-arch    - Instala dependências no Arch Linux"
