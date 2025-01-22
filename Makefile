# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -m32 -Wno-int-conversion -Wno-unused-function 

# Directories
SRC_DIR = ./src
INCLUDE_DIR = ./include
TEST_DIR = ./test
BIN_DIR = .

# Default target
all: $(BIN_DIR)/codegen

# Flex and Bison-generated files
$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/lex.l $(SRC_DIR)/y.tab.h
	flex -o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/lex.l

$(SRC_DIR)/y.tab.c $(SRC_DIR)/y.tab.h: $(SRC_DIR)/grammar.y
	byacc -d -v -o $(SRC_DIR)/y.tab.c $(SRC_DIR)/grammar.y

# Compile all source files into a single executable
$(BIN_DIR)/codegen: $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/codegen $(SRC_DIR)/y.tab.c \
	$(SRC_DIR)/string_hash_table.c $(SRC_DIR)/seman.c \
	$(SRC_DIR)/codegen.c $(SRC_DIR)/tree.c $(SRC_DIR)/symbol_table.c -I$(INCLUDE_DIR) -lfl

# Clean up generated files
clean:
	rm -rf $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.c $(SRC_DIR)/y.tab.h $(SRC_DIR)/y.output \
	$(BIN_DIR)/codegen $(BIN_DIR)/*.s $(BIN_DIR)/*.txt $(BIN_DIR)/*.out $(BIN_DIR)/*.c $(BIN_DIR)/*.h $(BIN_DIR)/y.output

# Run tests using the combined script
test: all
	bash ./test.sh