# Makefile TP Flex

# $@ : the current target
# $^ : the current prerequisites
# $< : the first current prerequisite

CC=gcc
CFLAGS=-Wall
LDFLAGS=-lfl
LEXER=lexer
PARSER=parser
SRC_PATH = src/
OBJ_PATH = obj/
BIN_PATH = bin/
OBJ_FILES = $(PARSER).tab.o lex.yy.o abstract-tree.o decl-var.o
EXEC = tpcc
CURRENT_DIR=$(notdir $(shell pwd))
ZIP_FILE = Compiler_L3_CHAN_ARAVINDAN.zip

all: $(EXEC) clean

$(EXEC): $(OBJ_FILES)
	$(eval OBJ_FILES_PATH := $(addprefix $(OBJ_PATH), $^))
	@echo Compile the execution file
	$(CC) -o $(BIN_PATH)$@ $(OBJ_FILES_PATH) $(LDFLAGS)

$(PARSER).tab.c: $(SRC_PATH)$(PARSER).y
	@echo Compile the Bison file
	bison -d $< -o $(SRC_PATH)$@

lex.yy.c: $(SRC_PATH)$(LEXER).lex
	@echo Compile the Flex file
	flex -o $(SRC_PATH)$@ $<

abstract-tree.o: $(SRC_PATH)abstract-tree.c
	$(CC) -o $(OBJ_PATH)$@ -c $< $(CFLAGS)

decl-var.o: $(SRC_PATH)decl-var.c
	$(CC) -o $(OBJ_PATH)$@ -c $< $(CFLAGS)

%.o: %.c
	@echo Compile all the C files necessary for the execution file\n
	$(CC) -o $(OBJ_PATH)$@ -c $(SRC_PATH)$< $(CFLAGS)

clean:
	rm -f $(SRC_PATH)lex.yy.*
	rm -f $(SRC_PATH)$(PARSER).output
	rm -f $(SRC_PATH)$(PARSER).tab.*
	rm -f $(OBJ_PATH)*

mrproper: clean
	rm -f $(BIN_PATH)*
	rm -f $(ZIP_FILE)

zip:
	cd ..; zip -r $(CURRENT_DIR)/$(ZIP_FILE) $(CURRENT_DIR) -x '*.git*' -x '.gitignore'
