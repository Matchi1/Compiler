/* decl-var.c */
/* Traduction descendante récursive des déclarations de variables en C */
/* Compatible avec decl-var.lex */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl-var.h"
#include "verif_type.h"

#define GLOBAL "global"

static int STfuncSize = 0;			/* current size of the symbol table for functions */
static int STstructSize = 0;
static STstructure STstructs[MAXFUNCS];
static STfunc STFunctions[MAXFUNCS];
char current_func[MAXNAME];	/* contains the current function name */
char current_type[MAXNAME];	/* contains the current type */

int var_already_in(STfunc *stfunc, const char *name, int lineno){
	int i;
	for(i = 0; i < stfunc->STsize; i++){
		if(!strcmp(stfunc->STvariables[i].name, name)){
			printf("semantic error, redefinition of variable %s near line %d\n", name, lineno);
			return 1;
		}
	}
	return 0;
}

STfunc* st_global(){
	return &STFunctions[0];
}

int nb_global_var(){
	return STFunctions[0].STsize;
}

char* type(const char* var_name, const char* func_name){
	int i;
	STfunc *st = stfunc(func_name);
	for(i = 0; i < st->STsize; i++){
		if(strcmp(st->STvariables[i].name, var_name) == 0)
			return st->STvariables[i].type;
	}
	// verify in global variables
	st = st_global();
	for(i = 0; i < st->STsize; i++){
		if(strcmp(st->STvariables[i].name, var_name) == 0){
			return st->STvariables[i].type;
		}
	}
	return NULL;
}

/**
 * Translate the type from string to int.
 * @parameter type the type to translate.
 * @return -1 if the type does not exist else 0, 1, 2.
 */
int string_to_type(const char *type){
	if(type == NULL)
		return SEM_ERR;
	if(strcmp(type, "void") == 0)
		return VOID_TYPE;
	if(strcmp(type, "int") == 0)
		return INT_TYPE;
	if(strcmp(type, "char") == 0)
		return CHAR_TYPE;
	return SEM_ERR;
}

/**
 * Translate the type from string to int.
 * @parameter type the type to translate.
 * @return -1 if the type does not exist else 0, 1, 2.
 */
int type_to_string(int type, char *str){
	switch(type){
		case VOID_TYPE:
			strcpy(str, "void"); break;
		case INT_TYPE:
			strcpy(str, "int"); break;
		case CHAR_TYPE:
			strcpy(str, "char"); break;
		default:
			return -1;
	}
	return 1;
}

char* get_func_type(const char* func_name){
	return stfunc(func_name)->header.type;
}

int get_size_argument(const char* func_name){
	return stfunc(func_name)->STargSize;
}

int get_argument(const char* arg, const char* func_name){
	int i;
	STfunc *stf = stfunc(func_name);
	for(i = 0; i < stf->STargSize; i++){
		if(!strcmp(stf->arguments[i].name, arg))
			return i;
	}
	return -1;
}


char* get_arg_type_name(const char* arg, const char* func_name){
	int i;
	STfunc *stf = stfunc(func_name);
	for(i = 0; i < stf->STargSize; i++){
		if(!strcmp(stf->arguments[i].name, arg))
			return stf->arguments[i].type;
	}
	return NULL;
}

char* get_arg_type_index(int index, const char* func_name){
	STfunc *stf = stfunc(func_name);
	return stf->arguments[index].type;
}

void add_field_to_table(int index, const char* type, const char* field){
	int size = STstructs[index].STsize;
	strcpy(STstructs[index].STfields[size].type, type);
	strcpy(STstructs[index].STfields[size].name, field);
	(STstructs[index].STsize)++;
}

int add_struct_to_table(const char* root){
	int i;
	// find the corresponding structure table
	for(i = 0; i < STstructSize; i++){
		if(strcmp(STstructs[i].root, root) == 0)
			return i;
	}
	strcpy(STstructs[STstructSize].root, root);
	STstructs[STstructSize].STsize = 0;
	return STstructSize++;
}

void add_declarateur(Node *decl, const char *root, const char *type){
	Node *browse;
	int index;
	// browse declared fields
	index = add_struct_to_table(root);
	// add all declared fields into the structure table
	for(browse = decl; browse != NULL; browse = browse->nextSibling){
		add_field_to_table(index, type, browse->u.identifier);
	}
}

void add_structure(Node* struct_decl){
	Node* browse;
	char* root = struct_decl->u.identifier;
	for(browse = FIRSTCHILD(struct_decl); browse != NULL; browse = browse->nextSibling){
		if(FIRSTCHILD(browse)->kind == Identifier)
			add_declarateur(FIRSTCHILD(browse), root, SECONDCHILD(browse)->u.identifier);
		else
			add_declarateur(SECONDCHILD(browse), root, FIRSTCHILD(browse)->u.identifier);
	}
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
	var_already_in(stfunc, name, lineno);
    if (++(stfunc->STsize) > MAXSYMBOLS) {
        printf("too many variables near line %d\n", lineno);
		exit(3);
    }
	entry = &(stfunc->STvariables[stfunc->STsize - 1]);
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

void print_arguments(STfunc func){
	int i;
	printf("### FUNCTION ARGUMENTS ###\n");
	if(func.STargSize == 0)
		printf("empty\n");
	for(i = 0; i < func.STargSize; i++){
		printf("%s %s\n", func.arguments[i].type, func.arguments[i].name);
	}
}

void print_structure_table(){
	int i, j;
	STentry entry;
	printf("### STRUCTURE TABLE ###\n");
	for(i = 0; i < STstructSize; i++){
		printf("%s\n", STstructs[i].root);				// Display name of variable
		for(j = 0; j < STstructs[i].STsize; j++){
			entry = STstructs[i].STfields[j]; 			// Display fields name and type
			printf("\t%s %s\n", entry.type, entry.name);
		}
	}
}

void print_function_table(){
	int i, j;
	STentry entry;
	printf("### FUNCTION TABLE ###\n");
	for(i = 0; i < STfuncSize; i++){
		// Display function's type and name
		printf("%s %s()\n", STFunctions[i].header.type, STFunctions[i].header.name);
		print_arguments(STFunctions[i]);
		printf("### VARIABLES ###\n");
		for(j = 0; j < STFunctions[i].STsize; j++){
			entry = STFunctions[i].STvariables[j];
			printf("%s %s\n", entry.type, entry.name);	// Display variable's name and type
		}
	}
}

void printTable(){
	print_structure_table();
	print_function_table();
}

void add_identifier(Node* node){
	Node* browse;
	strcpy(current_type, FIRSTCHILD(node)->u.identifier);
	for(browse = SECONDCHILD(node); browse != NULL; browse = browse->nextSibling){
		if(browse->kind == Identifier)
			addVar(current_func, browse->u.identifier, current_type, browse->lineno);
	}
}

void add_arguments(Node *arguments, const char* func_name){
	STfunc *stf = stfunc(func_name);
	char *t = FIRSTCHILD(arguments)->u.identifier;
	char *name = SECONDCHILD(arguments)->u.identifier;
	strcpy(stf->arguments[stf->STargSize].type, t);
	strcpy(stf->arguments[stf->STargSize].name, name);
	(stf->STargSize)++;
}

void add_arg_list(Node *argList, const char* func_name){
	Node *browse;
	for(browse = FIRSTCHILD(argList); browse != NULL; browse = browse->nextSibling){
		add_arguments(browse, func_name);
	}
}

void add_header(Node* header){
	strcpy(current_func, header->u.identifier);
	if(FIRSTCHILD(header) != NULL){
		switch(FIRSTCHILD(header)->kind){
			case Type:
				addFunc(current_func, FIRSTCHILD(header)->u.identifier, header->lineno); break;
			case TypeDeclList:
				addFunc(current_func, "void", header->lineno); 
				add_arg_list(FIRSTCHILD(header), current_func); break;
			default: break;
		}
	}
	if(SECONDCHILD(header) != NULL)
		add_arg_list(SECONDCHILD(header), current_func);
}

void create_ST_aux(Node *tree){
	switch(tree->kind){
		case Header: 
			add_header(tree); break;
		case VarDeclList: 
			add_identifier(tree); break;
		case TypeDeclList:
			add_identifier(tree); break;
		case GlobalDecl: 
			strcpy(current_func, GLOBAL);
			addFunc(GLOBAL, "void", tree->lineno); break;
		case StructDecl:
			add_structure(tree); break;
		default: break;
	}
}

void create_ST(Node *tree){
	Node *child;	
	create_ST_aux(tree);
	for(child = tree->firstChild; child != NULL; child = child->nextSibling)
		create_ST(child);
}

STfunc* stfunc(const char *func_name){
	int i;
	for(i = 0; i < STfuncSize; i++){
		if(strcmp(STFunctions[i].header.name, func_name) == 0)
			return &(STFunctions[i]);
	}
	return NULL;
}

int type_field(const char *type, const char *field){
	int i, j;
	STstructure structure;	
	for(i = 0; i < STstructSize; i++){
		structure = STstructs[i];
		for(j = 0; j < STstructs[i].STsize; j++){
			if(!strcmp(structure.root, type)
					&& !strcmp(structure.STfields[j].name, field)){
				return string_to_type(structure.STfields[j].type);
			}
		}
	}
	return -1;
}

