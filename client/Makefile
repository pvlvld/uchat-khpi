CC = clang

# Path for header files, including GTK3+ via pkg-config
CFLAGS = -std=c11 \
         -Wall \
         -Wextra \
         -Werror \
         -Wpedantic \
         -I/usr/include \
         -I../libraries/sqlite/include \
         -I../libraries/cJSON/build/include \
         -I./inc \
         $(shell pkg-config --cflags gtk+-3.0)  # Добавляем флаги GTK3+

# Path for libraries, including GTK3+ via pkg-config
LDFLAGS = -L../libraries/sqlite/lib \
          -L../libraries/cJSON/build/lib \
          -lsqlite3 \
          -lcjson \
          $(shell pkg-config --libs gtk+-3.0) \
          -Wl,-rpath,../libraries/sqlite/lib \
          -Wl,-rpath,../libraries/cJSON/build/lib

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

# Build target after checking GTK3+
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
