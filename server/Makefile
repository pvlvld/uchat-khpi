CC = clang

export CPATH=/usr/include/postgresql:$CPATH
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH

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

# Path for libraries
LDFLAGS = -L/usr/lib/x86_64-linux-gnu \
          -lsqlite3 \
          -lcjson \
          -lssl \
          -lcrypto \
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
