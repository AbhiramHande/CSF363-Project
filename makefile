# List of directories 
# TEST_CASE_DIR := ./tests/test_cases
# TEST_OUTP_DIR := ./tests/expected_output

# Compilation and linking tools
COMPILER = gcc
DEBUG_COMPILER = gdb
CFLAGS = -c -g
DEBUG_FLAG = -DDEBUG -DLTEST
LTEST_FLAG = -c -D_XOPEN_SOURCE=600 -std=c99 -Wall -Wextra

# Program files
HEADER_FILES := lexer.h lexerDef.h symbolTable.h parse.h parserDef.h hash_map.h stack.h
SRC_FILES := driver.c new_lexer.c symbolTable.c parse.c hash_map.c stack.c
OBJ_FILES := $(SRC_FILES:%.c=%.o)
TARGET = stage1exe

# Debug and test files
# DEBUG_HEADER := $(HEADER_DIR)/debug.h
# TEST_SRC := $(SRC_DIR)/tester.c
# TEST_OBJ := $(OBJ_DIR)/tester.o
# TEST_TARGET := $(BUILD_DIR)/test.exe

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(COMPILER) -o $@ $^

# $(TEST_TARGET): $(TEST_OBJ)
# 	mkdir -p $(BUILD_DIR)
# 	$(COMPILER) -o $@ $^ 

%.o: %.c
	$(COMPILER) $(CFLAGS) -o $@ $<

$(TEST_OBJ): $(TEST_SRC) $(TEST_HEADER)
	mkdir -p $(OBJ_DIR)
	$(COMPILER) $(LTEST_FLAG) -o $@ $<

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ)
	rm -f $(TARGET) $(TEST_TARGET)

# test: $(TEST_TARGET) $(TARGET)
# 	$(TEST_TARGET) $(TARGET) $(TEST_CASE_DIR) $(TEST_OUTP_DIR)

# debug: CFLAGS += $(DEBUG_FLAG)
# debug: $(TARGET)

# ltest: LTEST_FLAG += $(DEBUG_FLAG)
# ltest: $(TEST_TARGET) $(TARGET)
# 	$(TEST_TARGET) $(TARGET) $(TEST_CASE_DIR) $(TEST_OUTP_DIR)
