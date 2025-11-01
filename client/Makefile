CC = clang

# Path for header files, including GTK3+ via pkg-config
CFLAGS = -std=c11 \
        -Wall \
        -Wextra \
        -Werror \
        -Wpedantic \
        -I/usr/include \
        -I./inc \
        $(shell pkg-config --cflags gtk+-3.0) \
        $(shell pkg-config --cflags sqlite3) \
        $(shell pkg-config --cflags libcjson) \
        $(shell pkg-config --cflags openssl) \
        -I/usr/include/postgresql

# Path for libraries, including GTK3+ via pkg-config
LDFLAGS = -lsqlite3 \
        -lcjson \
        -lssl \
        -lcrypto \
        -lpq \
        $(shell pkg-config --libs gtk+-3.0)

SRC_DIR = src
OBJ_DIR = obj
SRC = $(shell find $(SRC_DIR) -name '*.c')
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

TARGET = uchat

# Create the obj folder and its subfolders
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D) # Create the necessary subdirectories in obj
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET)

# Build target after checking GTK3+ and libnotify
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Successfully built target: $@"

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Cleaned up build files."

.PHONY: all clean
