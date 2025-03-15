# Compilation and linking tools
COMPILER = gcc
DEBUG_COMPILER = gdb
CFLAGS = -c -g
DEBUG_FLAG = -DDEBUG -DLTEST
LTEST_FLAG = -c -D_XOPEN_SOURCE=600 -std=c99 -Wall -Wextra

# Program files
HEADER_FILES := lexer.h lexerDef.h symbol_table.h parser.h parserDef.h hash_map.h stack.h
SRC_FILES := driver.c lexer.c symbol_table.c parser.c hash_map.c stack.c
OBJ_FILES := $(SRC_FILES:%.c=%.o)
TARGET = stage1exe

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(COMPILER) -o $@ $^

%.o: %.c
	$(COMPILER) $(CFLAGS) -o $@ $<

$(TEST_OBJ): $(TEST_SRC) $(TEST_HEADER)
	mkdir -p $(OBJ_DIR)
	$(COMPILER) $(LTEST_FLAG) -o $@ $<

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ)
	rm -f $(TARGET) $(TEST_TARGET)
