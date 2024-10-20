CC = clang

# Path for header files
CFLAGS = -std=c11 \
         -Wall \
         -Wextra \
         -Werror \
         -Wpedantic \
         -I/usr/include \
         -I../libraries/sqlite/include \
         -I../libraries/cJSON/build/include \
         -I./libmx/inc \
         -I./inc

# Path for libraries
LDFLAGS = -L../libraries/sqlite/lib \
          -L../libraries/cJSON/build/lib \
          -lsqlite3 \
          -lssl \
          -lcrypto \
          -lcjson \
          -Wl,-rpath,../libraries/sqlite/lib \
          -Wl,-rpath,../libraries/cJSON/build/lib

SRC_DIR = src
OBJ_DIR = obj
SRC = $(shell find $(SRC_DIR) -name '*.c')
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

TARGET = uchat_server

# Create the obj folder and its subfolders
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D) # Create the necessary subdirectories in obj
	$(CC) $(CFLAGS) -c $< -o $@

# Main goal: install OpenSSL first, then build the project
all: openssl $(TARGET)

# Verify and install OpenSSL if not found
openssl:
	@if ! openssl version > /dev/null 2>&1; then \
	    echo "OpenSSL not found. Installing..."; \
	    sudo apt-get update && sudo apt-get install -y libssl-dev; \
	else \
	    echo "OpenSSL is already installed."; \
	fi

# Build target after checking OpenSSL
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean openssl
