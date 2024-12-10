CC = clang

# Path for header files, including GTK3+ via pkg-config and static glibc
CFLAGS = -std=c11 \
        -Wall \
        -Wextra \
        -Werror \
        -Wpedantic \
        -I/usr/include \
        -I../libraries/sqlite/inc \
        -I../libraries/cJSON/inc \
        -I../libraries/openssl \
        -I../libraries/libpq \
        -I./inc \
        $(shell pkg-config --cflags gtk+-3.0) \
        # -I../libraries/glibc/include \

# Path for libraries, including GTK3+ via pkg-config and static glibc
LDFLAGS = -L../libraries/sqlite/lib \
        -L../libraries/cJSON/lib \
        -L../libraries/precompiled/ubuntu/lib \
        -lsqlite3 \
        -lcjson \
        -lssl \
        -lcrypto \
        -lpq \
        $(shell pkg-config --libs gtk+-3.0) \
        -Wl,-rpath,../libraries/sqlite/lib \
        -Wl,-rpath,../libraries/cJSON/lib \
        -Wl,-rpath,../libraries/precompiled/ubuntu/lib \
        -Wl,-rpath,../libraries/glibc/lib
        # -lc \
        # -L../libraries/glibc/lib \

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
