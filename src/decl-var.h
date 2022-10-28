/* decl-var.h */
/* Analyse descendante récursive des déclarations de variables en C */
#ifndef __STENTRY__
#define __STENTRY__
#define MAXNAME 32
#define MAXSYMBOLS 256
#define MAXFUNCS 100

#include "abstract-tree.h"

typedef struct {
    char name[MAXNAME];
    char type[MAXNAME];
} STentry;

typedef struct {
	char root[MAXNAME];
	int STsize;
	STentry STfields[MAXSYMBOLS];
} *STstruct, STstructure;

typedef struct {
	STentry header;
	int STsize, STargSize;
	STentry STvariables[MAXSYMBOLS];
	STentry arguments[MAXSYMBOLS];
} STfunc;

void printTable();
void create_ST(Node *tree);
STfunc* stfunc(const char *func_name);
int string_to_type(const char *type);
int type_field(const char *root, const char *field);
char* type(const char* var_name, const char* func_name);
char* get_arg_type_name(const char* arg, const char* func_name);
char* get_arg_type_index(int index, const char* func_name);
char* get_func_type(const char *func_name);
int get_size_argument(const char* func_name);
int nb_global_var();
int is_global_var(const char* name);
int index_var(const char* func_name, const char* var);

#endif 
