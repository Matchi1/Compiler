#include "verif_type.h"
#include "decl-var.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char current_func_name[20] = "global";

void verif_argument(Node *ExprList, const char *func_name);

void print_semantic_error(int lineno){
	printf("semantic error: line %d\n", lineno);
	exit(3);
}

int type_diff(int expected_type, int type){
	if(expected_type == type)
		return 0;
	return 1;
}

int verif_expr(Node *node, int expected_type, const char *func_name){
	Node *browse;
	int current_type;
	assert(node != NULL);

	browse = FIRSTCHILD(node);
	while(browse != NULL){
		switch(browse->kind){
			case Expr:
				current_type = verif_expr(browse, expected_type, func_name); break;
			case IntLiteral:
				current_type = INT_TYPE; break;
			case CharLiteral:
				current_type = CHAR_TYPE; break;
			case Identifier:
				current_type = string_to_type(type(browse->u.identifier, func_name)); break;
			case FuncCall:
				verif_argument(FIRSTCHILD(browse), browse->u.identifier);
				current_type = string_to_type(get_func_type(browse->u.identifier)); break;
			default: break;
		}
		if(expected_type == SEM_ERR)
			expected_type = current_type;
		else if(current_type == SEM_ERR || type_diff(expected_type, current_type))
			return SEM_ERR;
		browse = browse->nextSibling;
	}
	return expected_type;
}

/**
 * Verify if the RETURN instruction return a value that has the same
 * type as the return value of the function.
 * We assume that 'node' has the 'Return' InstrType.
 * @parameter node a pointer to a Node type.
 * @parameter expected_type the expected type of the returned variable.
 * @parameter func_name name of the current function.
 */
void verif_return(Node *node, const char *func_name){
	STfunc* stf;
	int expected_type, expr_type;
	assert(node != NULL);
	
	stf = stfunc(func_name);
	if(stf == NULL){
		fprintf(stderr, "fonction %s inconnue dans la table des symboles\n", func_name);
		exit(EXIT_FAILURE);
	}

	expected_type = string_to_type(stf->header.type);
	if((FIRSTCHILD(node) == NULL && expected_type != VOID_TYPE)
		|| (FIRSTCHILD(node) != NULL && expected_type == VOID_TYPE))
		print_semantic_error(node->lineno);

	expr_type = verif_expr(FIRSTCHILD(node), expected_type, func_name);
	if(expr_type == SEM_ERR || type_diff(expected_type, expr_type))
		print_semantic_error(node->lineno);
}

int verif_field(Node *node){
	char *t = type(FIRSTCHILD(node)->u.identifier, current_func_name);
	return type_field(t, SECONDCHILD(node)->u.identifier);
}

int verif_var(Node *node, const char* func_name){
	int t = string_to_type(type(node->u.identifier, func_name));
	return t;
}

/**
 * Verify if the ASIGNMENT instruction asign values that have the same type.
 * We assume that 'node' has the 'Asign' InstrType.
 * @parameter node a pointer to a Node type.
 * @parameter func_name name of the current function.
 * @return 0 if the returned variable has the same type as the expected type.
 */
void verif_asign(Node *node, const char *func_name){
	int expected_type, expr_type;

	switch(FIRSTCHILD(node)->kind){
		case StructField:
			expected_type = verif_field(FIRSTCHILD(node)); break;
		case Identifier:
			expected_type = verif_var(FIRSTCHILD(node), func_name); break;
		default: 
			expected_type = -1;
	}
	expr_type = verif_expr(SECONDCHILD(node), expected_type, func_name);

	if(expr_type == SEM_ERR || type_diff(expected_type, expr_type))
		print_semantic_error(node->lineno);
}

/**
 * Verify if the READE instruction asign values that have the same type.
 * We assume that 'node' has the 'Asign' InstrType.
 * @parameter node a pointer to a Node type.
 * @parameter func_name name of the current function.
 * @return 0 if the returned variable has the same type as the expected type.
 */
void verif_reade(Node *node, const char *func_name){
	int current_type, return_type_diff; 
	assert(node != NULL && FIRSTCHILD(node) != NULL);

	current_type = string_to_type(type(FIRSTCHILD(node)->u.identifier, func_name));
	return_type_diff = type_diff(INT_TYPE, current_type);

	if(return_type_diff)
		print_semantic_error(node->lineno);
}

/**
 * Verify if the ASIGNMENT instruction asign values that have the same type.
 * We assume that 'node' has the 'Asign' InstrType.
 * @parameter node a pointer to a Node type.
 * @parameter func_name name of the current function.
 * @return 0 if the returned variable has the same type as the expected type.
 */
void verif_readc(Node *node, const char *func_name){
	int current_type, return_type_diff;
	assert(node != NULL && FIRSTCHILD(node) != NULL);

	current_type = string_to_type(type(FIRSTCHILD(node)->u.identifier, func_name));
	return_type_diff = type_diff(CHAR_TYPE, current_type);

	if(return_type_diff)
		print_semantic_error(node->lineno);
}

void verif_nb_arguments(Node *Expr, int lineno, int expected_nb_args){
	int nb_args = 0;
	for(; Expr != NULL; Expr = Expr->nextSibling)
		nb_args++;
	if(expected_nb_args != nb_args)
		print_semantic_error(lineno);
}

void verif_argument(Node *ExprList, const char *func_name){
	int expected_nb_args = get_size_argument(func_name);
	int i = expected_nb_args - 1, no_error, expected_type;
	Node *Expr;
	verif_nb_arguments(FIRSTCHILD(ExprList), ExprList->lineno, expected_nb_args);
	// verify if the number of argument are the same
	for(Expr = FIRSTCHILD(ExprList); Expr != NULL; Expr = Expr->nextSibling){
		expected_type = string_to_type(get_arg_type_index(i, func_name));
		no_error = verif_expr(Expr, expected_type, func_name);
		i--;
		if(no_error == -1)
			print_semantic_error(Expr->lineno);
	}
}

/**
 * Verify if an instruction is semantically correct.
 * @parameter node a pointer to a Node type.
 * @parameter func_name name of the current function.
 */
void verif_instr(Node *node, const char *func_name){
	assert(node != NULL);
	switch(node->u.instruction){
		case Asign:
			verif_asign(node, func_name); break;
		case Reade: 
			verif_reade(node, func_name); break;
		case Readc:
			verif_readc(node, func_name); break;
		case Return:
			verif_return(node, func_name); break;
		case Exp:
			verif_expr(node, -1, func_name); break;
		default: break;
	}
}

void verif_instr_list(Node *node, const char *func_name){
	Node *browse;
	assert(node != NULL);

	browse = FIRSTCHILD(node);		
	while(browse != NULL){
		verif_instr(browse, func_name);
		browse = browse->nextSibling;
	}
}

/**
 *	Analyze a tree and verify if it is semantically correct.
 *	@parameter root a pointor to a Node struct.
 */
void check_semantic_error(Node* root){
	Node *browse;
	if(root != NULL){
		switch(root->kind){
			case Expr:
				if(verif_expr(root, -1, current_func_name) == SEM_ERR) 
					print_semantic_error(root->lineno);
				break;
			case InstrList: 
				verif_instr_list(root, current_func_name); break;
			case Header:
				strcpy(current_func_name, root->u.identifier); break;
			default: 
				for(browse = FIRSTCHILD(root); browse != NULL; browse = browse->nextSibling)
					check_semantic_error(browse);
				break;
		}
	}
}
