CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -fstack-protector -O2

SRC_DIR = src
INC_DIR = include
SOURCE_FILES := $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/shapes/*.c)
OBJECT_FILES := $(patsubst $(SRC_DIR)/%.c,build/%.o,$(SOURCE_FILES))
TARGET = svg_coda

all: build

build: build/$(TARGET)

build/$(TARGET): $(OBJECT_FILES) | build/
	$(CC) $(CFLAGS) $(OBJECT_FILES) -o $@

build/%.o: $(SRC_DIR)/%.c $(wildcard $(INC_DIR)/*.h $(INC_DIR)/shapes/*.h $(SRC_DIR)/*.h $(SRC_DIR)/shapes/*.h) | build/
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -f $(OBJECT_FILES) build/$(TARGET)

fclean: clean
	rm -rf build

re: fclean all

.PHONY: all build clean fclean re

build/:
	mkdir -p build