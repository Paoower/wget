TARGET=wget

BUILD_DIR=build
INCLUDE_DIR=include
SRC_DIR=src

SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

CC=gcc
WARN_FLAGS=-Wall -Wextra -Werror
RM=rm -rf

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(WARN_FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

all: $(TARGET)

test: WARN_FLAGS=
test: $(TARGET)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)

re: fclean all

.PHONY: all clean fclean re
