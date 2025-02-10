SRC_DIR := ./src
OBJ_DIR := ./obj
HEADER_DIR := ./include
BUILD_DIR := ./build
TEST_CASE_DIR := ./tests/test_cases
TEST_OUTP_DIR := ./tests/expected_output

COMPILER = gcc
CFLAGS = -c -I$(HEADER_DIR)
DEBUG_FLAG = -DDEBUG
LTEST_FLAG = -DLTEST -DDEBUG

HEADER_FILES := $(HEADER_DIR)/lexer.h $(HEADER_DIR)/parser.h
SRC_FILES := lexer.c parser.c main.c
OBJ_FILES := $(SRC_FILES:%.c=$(OBJ_DIR)/%.o)
TARGET = $(BUILD_DIR)/prog.exe

DEBUG_HEADER := $(HEADER_DIR)/debug.h
TEST_HEADER := $(HEADER_DIR)/tester.h
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
	$(COMPILER) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ)
	rm -f $(TARGET) $(TEST_TARGET)
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

test: $(TEST_TARGET) $(TARGET)
	$(TEST_TARGET) $(TARGET) $(TEST_CASE_DIR) $(TEST_OUTP_DIR)

debug: CFLAGS += $(DEBUG_FLAG)
debug: $(TARGET)

ltest: CFLAGS += $(LTEST_FLAG)
ltest: $(TEST_TARGET) $(TARGET)
	$(TEST_TARGET) $(TARGET) $(TEST_CASE_DIR) $(TEST_OUTP_DIR)
