/* decl-var.h */
/* Analyse descendante récursive des déclarations de variables en C */
#ifndef __STENTRY__
#define __STENTRY__
#define MAXNAME 32

typedef struct {
    char name[MAXNAME];
    int type;
} STentry;

void check(int token);
void addVar(const char name[], int type);
void syntaxError();
void printTable();
int alreadyIn(char* ident);

#endif 
