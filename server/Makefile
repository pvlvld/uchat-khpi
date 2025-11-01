CC = clang

# Path for header files
CFLAGS = -std=c11 \
         -Wall \
         -Wextra \
         -Werror \
         -Wpedantic \
         -I/usr/include \
         -I./libmx/inc \
         -I/usr/include/postgresql \
         -I./inc \
         $(shell pkg-config --cflags sqlite3) \
         $(shell pkg-config --cflags libcjson) \
         $(shell pkg-config --cflags openssl)

# Path for libraries - using pkg-config for portability
LDFLAGS = $(shell pkg-config --libs sqlite3) \
          $(shell pkg-config --libs libcjson) \
          $(shell pkg-config --libs openssl) \
          -lpq

SRC_DIR = src
OBJ_DIR = obj
SRC = $(shell find $(SRC_DIR) -name '*.c')
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

TARGET = uchat_server

# Default target
all: $(TARGET)

# Create the obj folder and its subfolders
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D) # Create the necessary subdirectories in obj
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
