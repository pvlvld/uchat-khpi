CC = clang

export CPATH=/usr/include/postgresql:$CPATH
export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LIBRARY_PATH

LDFLAGS += -L/usr/lib/x86_64-linux-gnu
LIBS += -lpq

CFLAGS += -I/usr/include/postgresql

# Path for header files
CFLAGS = -std=c11 \
         -Wall \
         -Wextra \
         -Werror \
         -Wpedantic \
         -I/usr/include \
         -I../libraries/sqlite/inc \
         -I../libraries/cJSON/inc \
         -I../libraries/openssl \
         -I./libmx/inc \
         -I./inc

# Path for libraries
LDFLAGS = -L../libraries/sqlite/lib \
          -L../libraries/cJSON/lib \
          -L../libraries/precompilied/ubuntu/lib \
          -lsqlite3 \
          -lssl \
          -lcrypto \
          -lcjson \
          -Wl,-rpath,../libraries/sqlite/lib \
          -Wl,-rpath,../libraries/cJSON/lib \
          -Wl,-rpath,../libraries/precompilied/ubuntu/lib \
          -lpq #TODO: install locally

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
all: $(TARGET)

# Build target after checking OpenSSL
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean openssl
