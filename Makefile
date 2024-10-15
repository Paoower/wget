TARGET=wget
TESTS_TARGET=test

# src
BUILD_DIR=build
INCLUDE_DIR=include
SRC_DIR=src
# tests
TESTS_BUILD_DIR=tests_build
TESTS_DIR=tests

# includes
INCLUDE_DIRS = $(shell find $(INCLUDE_DIR) -type d)
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

# src
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# tests
TESTS_FILES = $(shell find $(TESTS_DIR) -name '*.c')
TESTS_OBJ_FILES = $(patsubst $(TESTS_DIR)/%.c,$(TESTS_BUILD_DIR)/%.o,$(TESTS_FILES))
TESTS_OBJ_FILES_NO_SRC_MAIN = $(TESTS_OBJ_FILES) $(filter-out $(BUILD_DIR)/main.o, $(OBJ_FILES))

CC=gcc
PREV_COMPILATION_FLAGS=-Wall -Wextra -Werror
SSL_FLAGS=-lssl -lcrypto
CHECK_FLAGS=-lcheck -lsubunit -lm
OTHER_FLAGS=
RM=rm -rf

# src
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PREV_COMPILATION_FLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(SSL_FLAGS)

all: $(TARGET)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)

re: fclean all

re-noflags: PREV_COMPILATION_FLAGS=
re-noflags: fclean all

re-debug: PREV_COMPILATION_FLAGS=-g
re-debug: fclean all

# tests
$(TESTS_BUILD_DIR)/%.o: $(TESTS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PREV_COMPILATION_FLAGS) $(INCLUDES) -c $< -o $@

$(TESTS_TARGET): $(TESTS_OBJ_FILES_NO_SRC_MAIN)
	$(CC) $(TESTS_OBJ_FILES_NO_SRC_MAIN) -o $@ $(CHECK_FLAGS) $(SSL_FLAGS)

test-clean:
	$(RM) $(TESTS_BUILD_DIR)

test-fclean: fclean test-clean
	$(RM) $(TESTS_TARGET)

test-re: test-fclean $(TESTS_TARGET)

test-noflags: PREV_COMPILATION_FLAGS=
test-noflags: $(TESTS_TARGET)

.PHONY: all tests clean fclean re re-noflags re-debug \
		test-clean test-fclean test-re
