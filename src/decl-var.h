/* decl-var.h */
/* Analyse descendante récursive des déclarations de variables en C */
#ifndef __STENTRY__
#define __STENTRY__
#define MAXNAME 32
#define MAXSYMBOLS 256

#include "abstract-tree.h"

typedef struct {
    char name[MAXNAME];
    char type[MAXNAME];
} STentry;

typedef struct {
	STentry header;
	int STsize;
	STentry symbolTable[MAXSYMBOLS];
} STfunc;

void printTable();
void create_ST(Node *tree);

#endif 
