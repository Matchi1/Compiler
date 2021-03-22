/* decl-var.c */
/* Traduction descendante récursive des déclarations de variables en C */
/* Compatible avec decl-var.lex */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl-var.h"
extern int lineno;  /* from lexical analyser */

#define MAXSYMBOLS 256
STentry symbolTable[MAXSYMBOLS];
int STmax=MAXSYMBOLS; /* maximum size of symbol table */
int STsize=0;         /* size of symbol table */

void addVar(const char name[], int type){
    int count;
    for (count=0;count<STsize;count++) {
        if (!strcmp(symbolTable[count].name,name)) {
            printf("semantic error, redefinition of variable %s near line %d\n",
            name, lineno);
            return;
        }
    }
    if (++STsize>STmax) {
        printf("too many variables near line %d\n", lineno);
        exit(1);
    }
    strcpy(symbolTable[STsize-1].name, name);
    symbolTable[STsize-1].type=type;
}

void printTable(){
	int i;
	for(i = 0; i < STsize; i++){
		if(symbolTable[i].type == 0)
			printf("simple type %s\n", symbolTable[i].name);
		else
			printf("struct %s\n", symbolTable[i].name);
	}
}

int alreadyIn(char* ident){
	int i;
	for(i = 0; i < STsize; i++){
		if(strcmp(ident, symbolTable[i].name) == 0){
			printf("%s est déjà présent dans la table des symboles\n", ident);
			return 1;
		}
	}
	return 0;
}
