COMPILER = gcc
FLAGS = -c
DEBUG_FLAG = -DDEBUG
LTEST_FLAG = -DLTEST -DDEBUG

CUR_DIR = $(shell pwd)
SRC_DIR = $(CUR_DIR)/src/
OBJ_DIR = $(CUR_DIR)/obj/
HEADER_DIR = $(CUR_DIR)/header/
BUILD_DIR = $(CUR_DIR)/build/
TEST_DIR = $(CUR_DIR)/tests/

HEADER_FILES = $(HEADER_DIR)lexer.h $(HEADER_DIR)parser.h
SRC_FILES = $(SRC_DIR)lexer.c $(SRC_DIR)parser.c $(SRC_DIR)main.c
OBJ_FILES = $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR)%.o, $(SRC_FILES))
TARGET = $(BUILD_DIR)exe

DEBUG_HEADER = $(HEADER_DIR)debug.h
TEST_HEADER = $(HEADER_DIR)tester.h
TEST_SRC = $(TEST_DIR)tester.c
TEST_OBJ = $(TEST_DIR)tester.o
TEST_TARGET = $(BUILD_DIR)test_exe

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(COMPILER) -o $@ $^

$(TEST_TARGET): $(OBJ_FILES) $(TEST_OBJ)
	mkdir -p $(BUILD_DIR)
	$(COMPILER) -o $@ $^

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(HEADER_FILES)
	mkdir -p $(OBJ_DIR)
	$(COMPILER) $(FLAGS) -o $@ $<

$(TEST_OBJ): $(TEST_SRC) $(TEST_HEADER)
	$(COMPILER) $(FLAGS) -o $@ $<

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ)
	rm -f $(TARGET) $(TEST_TARGET)
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

test: $(TEST_TARGET) $(TARGET)
	./$(TEST_TARGET) $(TARGET) $(TEST_DIR)

debug: FLAGS += $(DEBUG_FLAG)
debug: $(TARGET)

ltest: FLAGS += $(LTEST_FLAG)
ltest: $(TEST_TARGET) $(TARGET)
	./$(TEST_TARGET) $(TARGET) $(TEST_DIR)
