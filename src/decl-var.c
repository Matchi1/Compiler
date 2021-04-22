/* decl-var.c */
/* Traduction descendante récursive des déclarations de variables en C */
/* Compatible avec decl-var.lex */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl-var.h"

#define MAXFUNCS 100
#define GLOBAL "global"

STentry symbolTable[MAXSYMBOLS];
STfunc STFunctions[MAXFUNCS];
int STfuncSize = 0;			/* current size of the symbol table for functions */
int STmax = MAXSYMBOLS; 	/* maximum size of symbol table */
char current_func[MAXNAME];	/* contains the current function name */
char current_type[MAXNAME];	/* contains the current type */

int var_already_in(STfunc *stfunc, const char *name, int lineno){
	int i, j;
	STfunc tmp;
	for(i = 0; i < 2; i++){
		switch(i){
			case 0:
				tmp = STFunctions[0]; break;
			default:
				tmp = *stfunc; break;
		}
		for(j = 0; j < tmp.STsize; j++){
			if(!strcmp(tmp.symbolTable[j].name, name)){
				printf("semantic error, redefinition of variable %s near line %d\n", name, lineno);
				return 1;
			}
		}
	}
	return 0;
}

int func_already_in(const char *name, int lineno){
	int i;
	for(i = 0; i < STfuncSize; i++){
		if(!strcmp(STFunctions[i].header.name, name)){
			printf("semantic error, redefinition of function %s near line %d\n", name, lineno);
			return 1;
		}
	}
	return 0;
}

void addVarAux(STfunc* stfunc, const char *name, const char *type, int lineno){
	STentry *entry;
	if(var_already_in(stfunc, name, lineno))
		return;
    if (++(stfunc->STsize) > STmax) {
        printf("too many variables near line %d\n", lineno);
        exit(EXIT_FAILURE);
    }
	entry = &(stfunc->symbolTable[stfunc->STsize - 1]);
    strcpy(entry->name, name);
    strcpy(entry->type, type);
}

void addVar(const char *funcName, const char *name, const char *type, int lineno){
	int count;
    for (count = 0; count < STfuncSize; count++) {
        if (!strcmp(STFunctions[count].header.name, funcName)) {
			addVarAux(&(STFunctions[count]), name, type, lineno);
			return;
        }
    }
}

void addFunc(const char *name, const char *type, int lineno){
	if(func_already_in(name, lineno))
		return;
    if (++STfuncSize > MAXFUNCS) {
        printf("too many variables near line %d\n", lineno);
        exit(EXIT_FAILURE);
    }
    strcpy(STFunctions[STfuncSize - 1].header.name, name);
    strcpy(STFunctions[STfuncSize - 1].header.type, type);
	STFunctions[STfuncSize - 1].STsize = 0;
}

void printTable(){
	int i, j;
	STentry entry;
	for(i = 0; i < STfuncSize; i++){
		printf("%s %s()\n", STFunctions[i].header.type, STFunctions[i].header.name);
		for(j = 0; j < STFunctions[i].STsize; j++){
			entry = STFunctions[i].symbolTable[j];
			printf("%s %s\n", entry.type, entry.name);
		}
	}
}

void add_identifier(Node* node){
	Node* browse;
	strcpy(current_type, FIRSTCHILD(node)->u.identifier);
	for(browse = SECONDCHILD(node); browse != NULL; browse = browse->nextSibling){
		if(browse->kind == Identifier)
			addVar(current_func, browse->u.identifier, current_type, browse->lineno);
	}
}

void add_parameter(Node *param){
	Node *browse;
	for(browse = param->firstChild; browse != NULL; browse = browse->nextSibling)
		add_identifier(browse);
}

void add_header(Node* func){
	strcpy(current_func, func->u.identifier);
	if(FIRSTCHILD(func) != NULL){
		switch(FIRSTCHILD(func)->kind){
			case Type:
				addFunc(current_func, FIRSTCHILD(func)->u.identifier, func->lineno); break;
			case TypeDeclList:
				addFunc(current_func, "void", func->lineno); 
				add_parameter(FIRSTCHILD(func)); break;
			default: break;
		}
	}
	if(SECONDCHILD(func) != NULL)
		add_parameter(SECONDCHILD(func));
}

void create_ST_aux(Node *tree){
	switch(tree->kind){
		case EnTete: 
			add_header(tree); break;
		case VarDeclList: 
			add_identifier(tree); break;
		case TypeDeclList:
			add_identifier(tree); break;
		case TypesVars: 
			strcpy(current_func, GLOBAL);
			addFunc(GLOBAL, "void", tree->lineno); break;
		default: break;
	}
}

void create_ST(Node *tree){
	Node *child;	
	create_ST_aux(tree);
	for(child = tree->firstChild; child != NULL; child = child->nextSibling)
		create_ST(child);
}
