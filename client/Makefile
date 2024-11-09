CC = clang

# Path for header files, including GTK3+ via pkg-config
CFLAGS = -std=c11 \
         -Wall \
         -Wextra \
         -Werror \
         -Wpedantic \
         -I/usr/include \
         -I../libraries/sqlite/inc \
         -I../libraries/cJSON/inc \
         -I../libraries/openssl \
         -I./inc \
         $(shell pkg-config --cflags gtk+-3.0) \
         $(shell pkg-config --cflags libnotify)

# Path for libraries, including GTK3+ via pkg-config
LDFLAGS = -L../libraries/sqlite/lib \
          -L../libraries/cJSON/lib \
          -L../libraries/precompilied/ubuntu/lib \
          -lsqlite3 \
          -lcjson \
          -lssl \
          -lcrypto \
          $(shell pkg-config --libs gtk+-3.0) \
          $(shell pkg-config --libs libnotify) \
          -Wl,-rpath,../libraries/sqlite/lib \
          -Wl,-rpath,../libraries/cJSON/lib \
          -Wl,-rpath,../libraries/precompilied/ubuntu/lib

SRC_DIR = src
OBJ_DIR = obj
SRC = $(shell find $(SRC_DIR) -name '*.c')
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

TARGET = uchat

# Create the obj folder and its subfolders
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D) # Create the necessary subdirectories in obj
	$(CC) $(CFLAGS) -c $< -o $@

all: check_dependencies $(TARGET)

# Check OS type and set the appropriate dependency check
check_dependencies:
	@OS=$$(uname); \
	if [ "$$OS" = "Darwin" ]; then \
	    $(MAKE) check_dependencies_macos; \
	elif [ "$$OS" = "Linux" ]; then \
	    $(MAKE) check_dependencies_ubuntu; \
	else \
	    echo "Unsupported OS: $$OS"; \
	    exit 1; \
	fi

# Verify and install dependencies for Ubuntu
check_dependencies_ubuntu:
	@echo "Checking for dependencies on Ubuntu..."
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config is not installed. Install it to continue."; exit 1; }
	@if ! pkg-config --exists gtk+-3.0; then \
		echo "GTK3 is not installed. I'm installing it.."; \
		sudo apt install libgtk-3-dev -y; \
	else \
		echo "GTK3 is already installed."; \
	fi
	@if ! pkg-config --exists libnotify; then \
		echo "libnotify is not installed. I'm installing it.."; \
		sudo apt install libnotify-dev -y; \
	else \
		echo "libnotify is already installed."; \
	fi

# Verify and install dependencies for macOS
check_dependencies_macos:
	@echo "Checking for dependencies on macOS..."
	@command -v brew >/dev/null 2>&1 || { echo >&2 "Homebrew is not installed. Install it to continue."; exit 1; }
	@if ! pkg-config --exists gtk+-3.0; then \
		echo "GTK3 is not installed. Installing..."; \
		brew install gtk+3; \
	fi
	@if ! pkg-config --exists libnotify; then \
		echo "libnotify is not installed. I'm installing it.."; \
		brew install libnotify; \
	fi

# Build target after checking GTK3+ and libnotify
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Successfully built target: $@"

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Cleaned up build files."

.PHONY: all clean check_dependencies check_dependencies_ubuntu check_dependencies_macos
