CC = clang
CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -fPIC
OBJ_DIR = obj
LIB_DIR = lib
LIB_NAME = libsqlite3.a

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))

$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(LIB_DIR))

all: $(LIB_DIR)/$(LIB_NAME)

$(LIB_DIR)/$(LIB_NAME): $(OBJS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	rm -f $(LIB_DIR)/$(LIB_NAME) $(OBJ_DIR)/*.o
	rmdir $(OBJ_DIR) || true
