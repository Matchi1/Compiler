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
OBJ_FILES = $(PARSER).tab.o lex.yy.o abstract-tree.o decl-var.o Option.o verif_type.o generate_code.o
EXEC = tpcc
CURRENT_DIR=$(notdir $(shell pwd))
PREVIEW = ProjetCompilationL3_CHAN_ARAVINDAN
ZIP_FILE = $(PREVIEW).zip
TAR_FILE = $(PREVIEW).tar.gz

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

Option.o: $(SRC_PATH)Option.c
	$(CC) -o $(OBJ_PATH)$@ -c $< $(CFLAGS)

verif_type.o: $(SRC_PATH)verif_type.c
	$(CC) -o $(OBJ_PATH)$@ -c $< $(CFLAGS)

generate_code.o: $(SRC_PATH)generate_code.c
	$(CC) -o $(OBJ_PATH)$@ -c $< $(CFLAGS)

%.o: %.c
	@echo Compile all the C files necessary for the execution file
	$(CC) -o $(OBJ_PATH)$@ -c $(SRC_PATH)$< $(CFLAGS)

clean:
	rm -f $(SRC_PATH)lex.yy.*
	rm -f $(SRC_PATH)$(PARSER).output
	rm -f $(SRC_PATH)$(PARSER).tab.*
	rm -f $(OBJ_PATH)*

mrproper: clean
	rm -f $(BIN_PATH)*
	rm -rf $(TAR_FILE) $(ZIP_FILE) *.log $(PREVIEW) 
	rm -rf $(ZIP_FILE) $(TAR_FILE)

# Make a clean copy of the project
copy: mrproper
	mkdir $(PREVIEW)
	rsync -av . $(PREVIEW) --exclude .git --exclude *.odt --exclude .gitignore --exclude $(PREVIEW)

# create an zip format archive
zip: copy
	zip -r $(PREVIEW).zip $(PREVIEW)

# create an tar format archive
tar: copy
	tar -cvf $(PREVIEW).tar.gz $(PREVIEW)
