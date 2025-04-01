# Directories
INCLUDE_DIR := ./include
TEST_SRC_DIR := ./src/test
SRC_DIR := ./src/main
OBJ_DIR := ./build
EXEC_DIR := ./bin

# Files for testing
TEST_CASES := "./tests/provided_test_cases/test1.txt" "./tests/provided_test_cases/test2.txt"
TEST_OUTP := "./tests/expected_output/test1.txt" "./tests/expected_output/test2.txt"

# Compilation and linking variables
COMPILER = gcc
DEBUG_COMPILER = gdb
CFLAGS = -c -g -I$(INCLUDE_DIR)
TEST_FLAG = -c -D_XOPEN_SOURCE=600 -std=c99 -Wall -Wextra

# Program files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
MAIN_SRC_FILES = $(filter-out $(SRC_DIR)/driver.c, $(SRC_FILES))
SRC_TEST_TARGET := $(TEST_SRC_DIR)/test_lexer.c
SRC_TESTER := $(TEST_SRC_DIR)/tester.c
SRC_DRIVER := $(SRC_DIR)/driver.c

MAIN_OBJ_FILES := $(MAIN_SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_TEST_TARGET := $(OBJ_DIR)/test_lexer.o
OBJ_TESTER := $(OBJ_DIR)/tester.o
OBJ_DRIVER := $(OBJ_DIR)/driver.o

HEADER_FILES := $(wildcard $(INCLUDE_DIR)/*.h)

# Targets
TARGET = $(EXEC_DIR)/prog.exe
TEST_TARGET = $(EXEC_DIR)/test_target.exe
TESTER = $(EXEC_DIR)/tester.exe

.PHONY: docs clean all debug test help

# Build main executable
all: $(TARGET)

debug: $(TARGET)
	$(DEBUG_COMPILER) $(TARGET)

test: $(TESTER) $(TEST_TARGET)
	@$(foreach i, $(shell seq 1 $(words $(TEST_CASES))), \
		echo ""; \
		echo "\e[1mRunning test $(i)\e[0m: $(word $(i), $(TEST_CASES))"; \
		$(TESTER) $(TEST_TARGET) $(word $(i), $(TEST_CASES)) $(word $(i), $(TEST_OUTP)); \
	)
	
$(TARGET): $(MAIN_OBJ_FILES) $(OBJ_DRIVER)
	@mkdir -p $(EXEC_DIR)
	$(COMPILER) -o $@ $^

$(TEST_TARGET): $(MAIN_OBJ_FILES) $(OBJ_TEST_TARGET)
	@mkdir -p $(EXEC_DIR)
	$(COMPILER) -o $@ $^

$(TESTER): $(OBJ_TESTER)
	@mkdir -p $(EXEC_DIR)
	$(COMPILER) -o $@ $^


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_FILES)
	@mkdir -p $(OBJ_DIR)
	$(COMPILER) $(CFLAGS) -o $@ $<

$(OBJ_TEST_TARGET) : $(SRC_TEST_TARGET)
	@mkdir -p $(OBJ_DIR)
	$(COMPILER) $(CFLAGS) -o $@ $<

$(OBJ_TESTER) : $(SRC_TESTER)
	@mkdir -p $(OBJ_DIR)
	$(COMPILER) $(TEST_FLAG) -o $@ $<

# Clean up build artifacts
clean:
	rm -f $(OBJ_FILES)
	rm -f $(EXEC_DIR)/$(TARGET) $(EXEC_DIR)/$(TEST_TARGET)
	rm -rf $(OBJ_DIR) $(EXEC_DIR)

docs: Doxyfile
	doxygen Doxyfile
	xdg-open docs/index.html

help:
	@echo "\e[1;4;36mAvailable Commands:\e[0m"
	@echo "    1. \e[1;32mmake\e[0m        - Build the executable (\e[3mprog.exe\e[0m) and store it in \e[3m./bin\e[0m."
	@echo "    2. \e[1;32mmake test\e[0m   - Build the executable (\e[3mtest_taget.exe\e[0m) and run it with the tester (\e[03mtester.exe\e[0m)."
	@echo "                     Tester program runs the lexer against the first two provided test cases and ensure the outputs match."
	@echo "    3. \e[1;32mmake debug\e[0m  - Build the executable (\e[3mprog.exe\e[0m) and run it with the debugger \e[03mgdb\e[0m."
	@echo "    4. \e[1;32mmake docs\e[0m   - Rebuilds the Doxygen documentation and launches the website locally."
	@echo "    5. \e[1;32mmake clean\e[0m  - Remove executables, object files, and their associated directories."
	@echo "    6. \e[1;32mmake help\e[0m   - Display this help message."
