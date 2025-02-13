# List of directories 
SRC_DIR := ./src
OBJ_DIR := ./obj
HEADER_DIR := ./include
BUILD_DIR := ./build
TEST_CASE_DIR := ./tests/test_cases
TEST_OUTP_DIR := ./tests/expected_output

# Compilation and linking tools
COMPILER = gcc
DEBUG_COMPILER = gdb
CFLAGS = -c -g -I$(HEADER_DIR)
DEBUG_FLAG = -DDEBUG -DLTEST
LTEST_FLAG = -c -D_XOPEN_SOURCE=600 -std=c99 -Wall -Wextra -I$(HEADER_DIR)

# Program files
HEADER_FILES := $(HEADER_DIR)/lexer.h $(HEADER_DIR)/parser.h
SRC_FILES := lexer.c parser.c main.c
OBJ_FILES := $(SRC_FILES:%.c=$(OBJ_DIR)/%.o)
TARGET = $(BUILD_DIR)/prog.exe

# Debug and test files
DEBUG_HEADER := $(HEADER_DIR)/debug.h
TEST_SRC := $(SRC_DIR)/tester.c
TEST_OBJ := $(OBJ_DIR)/tester.o
TEST_TARGET := $(BUILD_DIR)/test.exe

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(COMPILER) -o $@ $^

$(TEST_TARGET): $(TEST_OBJ)
	mkdir -p $(BUILD_DIR)
	$(COMPILER) -o $@ $^ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_FILES)
	mkdir -p $(OBJ_DIR)
	$(COMPILER) $(CFLAGS) -o $@ $<

$(TEST_OBJ): $(TEST_SRC) $(TEST_HEADER)
	mkdir -p $(OBJ_DIR)
	$(COMPILER) $(LTEST_FLAG) -o $@ $<

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ)
	rm -f $(TARGET) $(TEST_TARGET)
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

test: $(TEST_TARGET) $(TARGET)
	$(TEST_TARGET) $(TARGET) $(TEST_CASE_DIR) $(TEST_OUTP_DIR)

debug: CFLAGS += $(DEBUG_FLAG)
debug: $(TARGET)

ltest: LTEST_FLAG += $(DEBUG_FLAG)
ltest: $(TEST_TARGET) $(TARGET)
	$(TEST_TARGET) $(TARGET) $(TEST_CASE_DIR) $(TEST_OUTP_DIR)
