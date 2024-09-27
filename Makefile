TARGET=wget
TESTS_TARGET=test

# src
BUILD_DIR=build
INCLUDE_DIR=include
SRC_DIR=src
# tests
TESTS_BUILD_DIR=tests_build
TESTS_DIR=tests

# src
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# tests
TESTS_FILES := $(shell find $(TESTS_DIR) -name '*.c')
TESTS_OBJ_FILES := $(patsubst $(TESTS_DIR)/%.c,$(TESTS_BUILD_DIR)/%.o,$(TESTS_FILES))

CC=gcc
WARN_FLAGS=-Wall -Wextra -Werror
SSL_FLAGS=-lssl -lcrypto
RM=rm -rf

# src
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(SSL_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(WARN_FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

all: $(TARGET)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)

re: fclean all

re_noflags: WARN_FLAGS=
re_noflags: fclean all

# tests
$(TESTS_TARGET): $(TESTS_OBJ_FILES)
	$(CC) $(TESTS_OBJ_FILES) -o $@ $(SSL_FLAGS)

$(TESTS_BUILD_DIR)/%.o: $(TESTS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(WARN_FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

tests_clean:
	$(RM) $(TESTS_BUILD_DIR)

tests_fclean: tests_clean
	$(RM) $(TESTS_TARGET)

tests_re: fclean $(TESTS_TARGET)

.PHONY: all tests clean fclean re re_noflags \
		tests_clean tests_fclean tests_re
