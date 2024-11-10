CC = clang

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
         -I/usr/include/postgresql \
         -I./inc

# Path for libraries
LDFLAGS = -L../libraries/sqlite/lib \
          -L../libraries/cJSON/lib \
          -L../libraries/precompilied/ubuntu/lib \
          -lsqlite3 \
          -lssl \
          -lcrypto \
          -Wl,-rpath,../libraries/sqlite/lib \
          -Wl,-rpath,../libraries/cJSON/lib \
          -Wl,-rpath,../libraries/precompilied/ubuntu/lib \
          -lpq

SRC_DIR = src
CJSON_DIR = ../libraries/cJSON
CJSON_SRC = $(CJSON_DIR)/cJSON.c
OBJ_DIR = obj
SRC = $(shell find $(SRC_DIR) -name '*.c')
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
CJSON_OBJ = $(OBJ_DIR)/cJSON.o

TARGET = uchat_server

# Default target
all: $(TARGET)

# Create the obj folder and its subfolders
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D) # Create the necessary subdirectories in obj
	$(CC) $(CFLAGS) -c $< -o $@

# Compile cJSON
$(CJSON_OBJ): $(CJSON_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(CJSON_DIR)/libcjson.so: $(CJSON_OBJ)
	$(CC) -shared -o $@ $< -fPIC

$(TARGET): $(OBJ) $(CJSON_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(CJSON_DIR)/libcjson.so

.PHONY: all clean openssl
