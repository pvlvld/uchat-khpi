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
         -I./inc \
         `pkg-config --cflags gtk4`

# Path for libraries
LDFLAGS = -L../libraries/sqlite/lib \
          -L../libraries/cJSON/build/lib \
          -lsqlite3 \
          -lssl \
          -lcjson \
          `pkg-config --libs gtk4` \
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

# Main goal: install GTK4 first, then build the project
all: gtk4 $(TARGET)

# Verify and install GTK4 if not found
gtk4:
	@if ! pkg-config --exists gtk4; then \
	    echo "GTK4 not found. Installing..."; \
	    sudo apt-get update && sudo apt-get install -y libgtk-4-dev; \
	else \
	    echo "GTK4 is already installed."; \
	fi

# Build target after checking GTK4
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean gtk4
