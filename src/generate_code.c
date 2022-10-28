#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "decl-var.h"
#include "generate_code.h"

#define DEFAULT_FILE "_anonymous.asm"
#define O_SIZE 8 // size of each variable (octet)
#define MAIN_FUNCTION "_start"

char main_function[] = "_start";
char current_file[] = DEFAULT_FILE;
char label_buf[10] = ".L";
char global_label[] = "table";
char current_func_name[30];
int current_label = 0;
int r_param_index = 0;
int r_not_volatil_index = 0;
int is_main = 0;
FILE *in;

static const char *r_params[] = {
	"rdi",
	"rsi",
	"rdx",
	"rcx",
	"r8",
	"r9"
};

static const char *r_operand[] = {
	"rax",
	"rbx",
	"r12",
	"r13",
	"r14",
	"r15"
};

/**
 * @brief return and update the index to the next non-volatil register
 */
int index_open_r_not_volatil(){
	return (r_not_volatil_index)++;
}

/**
 * @brief return and update the index to the next parameter register
 */
int index_open_r_params(){
	return (r_param_index)++;
}

/**
 * @brief free the last specified non-volatil register
 * @param nb_index number of index to free
 */
void open_r_not_volatil(int nb_index){
	r_not_volatil_index -= nb_index;
	if(r_not_volatil_index < 0)
		r_not_volatil_index = 0;
}

/**
 * @brief free the last specified parameter register
 * @param nb_index number of index to free
 */
void open_r_param(int nb_index){
	r_param_index -= nb_index;
	if(r_param_index < 0)
		r_param_index = 0;
}

/**
 * UNUSED
 * @brief return the next label
 * @return the next label
 */
char* label(){
	sprintf(&(label_buf[2]), "%d", current_label);
	return label_buf;
}

/**
 * @brief create a file descriptor
 */
void file_init(){
	in = fopen(current_file, "w");
}

/**
 * @brief close the current file descriptor
 */
void file_deinit(){
	fclose(in);
}


/**
 * @brief generate the code for global variables
 */
void create_global(){
	int i;
	fprintf(in, "section .data\n");
	fprintf(in, "table dq");
	for(i = 0; i < nb_global_var() - 1; i++)
		fprintf(in, " 0,");
	fprintf(in, " 0\n");
}

/**
 * @brief generate the code for the operation
 * @param Exp a pointor to a Node structure
 */
void generate_operation(Node *Exp){
	assert(Exp != NULL);
	assert(Exp->kind == Expr);
	int r1, r2;
	r1 = index_open_r_not_volatil();
	r2 = index_open_r_not_volatil();
	fprintf(in, "\tpop %s\n", r_operand[r2]);
	fprintf(in, "\tpop %s\n", r_operand[r1]);
	switch(Exp->u.op){
		case Add: 
			fprintf(in, "\tadd %s, %s\n", r_operand[r1], r_operand[r2]); break;
		case Sub: 
			fprintf(in, "\tsub %s, %s\n", r_operand[r1], r_operand[r2]); break;
		case Div: 
			fprintf(in, "\tidiv %s\n", r_operand[r2]); break;
		case Mul: 
			fprintf(in, "\timul %s, %s\n", r_operand[r1], r_operand[r2]); break;
		case Mod:
			fprintf(in, "\tidiv %s\n", r_operand[r2]);
			fprintf(in, "\tpush rdx\n");
			return;
		// case Or: break;
		// case And: break;
		// case Greater: break;
		// case Gequal: break;
		// case Lower: break;
		// case Lequal: break;
		// case Equal: break;
		// case Diff: break;
		// case Neg: break;
		default: break;
	}
	fprintf(in, "\tpush %s ; push the result in the heap\n", r_operand[r1]);
	open_r_not_volatil(2);
}

/**
 * @brief generate the code for the expression
 * @param Exp a pointor to a Node structure
 */
void generate_expr(Node *Exp){
	assert(Exp != NULL);
	assert(Exp->kind == Expr);
	Node *child;
	for(child = FIRSTCHILD(Exp); child != NULL; child = child->nextSibling){
		switch(child->kind){
			case Expr:
				generate_expr(child); break;
			case IntLiteral:
				fprintf(in, "\tpush %d\n", child->u.integer); break;
			case CharLiteral:
				fprintf(in, "\tpush '%c'\n", child->u.character); break;
			// case Identifier:
			// case FuncCall:
			default: 
				break;
		}
	}
	generate_operation(Exp);
}

/**
 * @brief generate the code for the start of a heap
 * @param size size of the heap
 */
void generate_start_heap(int size){
	fprintf(in, "\tpush rbp ; save the current base\n");
	fprintf(in, "\tmov rbp, rsp ; generate a new heap\n");
	fprintf(in, "\tsub rsp, %d ; size of the new heap\n", size * O_SIZE);
}

/**
 * @brief generate the code for the end of a heap
 * @param size size of the heap
 */
void generate_end_heap(int size){
	fprintf(in, "\tadd rsp, %d ; close the current heap\n", size * O_SIZE);
	fprintf(in, "\tpop rbp ; reset the current base\n");
}

/**
 * @brief generate the code for the asignment instruction
 * @param asign a pointor to a Node structure
 */
void generate_asign(Node *asign){
	assert(asign != NULL);
	assert(asign->u.instruction == Asign);
	int index = index_var(current_func_name, FIRSTCHILD(asign)->u.identifier);
	generate_expr(SECONDCHILD(asign));
	fprintf(in, "\tpop rax\n");
	fprintf(in, "\tmov [rsp + %d], rax\n", index * O_SIZE);
}

/**
 * @brief generate the code for the return instruction
 * @param ret a pointor to a Node structure
 */
void generate_return(Node *ret){
	assert(ret != NULL);
	assert(ret->u.instruction == Return);
	generate_end_heap(stfunc(current_func_name)->STsize);
	if(FIRSTCHILD(ret) != NULL){
		switch(FIRSTCHILD(ret)->kind){
			case IntLiteral:
				fprintf(in, "\tmov rax, %d ; return value\n",
						FIRSTCHILD(ret)->u.integer); break;
			case CharLiteral:
				fprintf(in, "\tmov rax,'%c' ; return value\n",
						FIRSTCHILD(ret)->u.character); break;
			case Expr: 
				generate_expr(FIRSTCHILD(ret));
				fprintf(in, "\tpop rax\n"); break;
			default: break;
		}
	}
	if(is_main){
		fprintf(in, "\tmov rdi, rax\n");
		fprintf(in, "\tmov rax, 60\n");
		fprintf(in, "\tsyscall\n");
	} else
		fprintf(in, "\tret\n");
}

/**
 * UNUSED
 * @brief generate the code for the return instruction
 * @param ret a pointor to a Node structure
 */
void generate_print(Node *print){
	assert(print != NULL);
	assert(print->u.instruction == Print);
	generate_start_heap(0);
	switch(FIRSTCHILD(print)->kind){
		case IntLiteral:
			fprintf(in, "\tpush '%d'\n", FIRSTCHILD(print)->u.integer); break;
		case CharLiteral:
			fprintf(in, "\tpush '%c'\n", FIRSTCHILD(print)->u.character); break;
		default:
			break;
	}
	fprintf(in, "\tcall printf\n");
	fprintf(in, "\tmov rsp, rbp\n");
}

/**
 * @brief generate the code for an instruction
 * @param instr a pointor to a Node structure
 */
void generate_instr(Node *instr){
	assert(instr != NULL);
	assert(instr->kind == Instr);
	switch(instr->u.instruction){
		case Asign:
			generate_asign(instr); break;
		case Exp:
			break; // TODO
		case Return:
			generate_return(instr); break;
		// case Reade: break;
		// case Readc: break;
		// case If: break;
		// case IfElse: break;
		// case While: break;
		// case Void: break;
		/* case Print: 
			generate_print(instr); break;
			*/
		default:
			break; // TODO
	}
}

/**
 * @brief generate the code for an instruction list
 * @param instr a pointor to a Node structure list
 */
void generate_instr_list(Node *instr_list){
	assert(instr_list != NULL);
	assert(instr_list->kind == InstrList);
	Node *instr;
	for(instr = FIRSTCHILD(instr_list); instr != NULL; instr = instr->nextSibling){
		generate_instr(instr);
	}
}

/**
 * @brief generate the header of each function in the nasm format
 * @param header a pointor to the Node structure
 */
void generate_header(Node *header){
	assert(header != NULL);
	assert(header->kind == Header);
	strcpy(current_func_name, header->u.identifier);
	if(!strcmp(header->u.identifier, "main")){
		is_main = 1;
		fprintf(in, "_start:\n");
	} else {
		is_main = 0;
		fprintf(in, "%s:\n", header->u.identifier);
	}
}

/**
 * @brief generate the body of each function in the nasm format
 * @param body a pointor to the Node structure
 */
void generate_body(Node *body){
	assert(body != NULL);
	assert(body->kind == Body);
	generate_start_heap(stfunc(current_func_name)->STsize);
	generate_instr_list(SECONDCHILD(body));
}

/**
 * @brief generate the function declaration of each function in the nasm format
 * @param body a pointor to the Node structure
 */
void generate_func_decl(Node *func_decl){
	assert(func_decl != NULL);
	assert(func_decl->kind == FuncDecl);
	generate_header(FIRSTCHILD(func_decl));
	generate_body(SECONDCHILD(func_decl));
}

/**
 * @brief auxiliary function to translate the code into the nasm format
 * @param node a pointor to the Node structure
 */
void generate_code_aux(Node *node){
	Node *child;
	for(child = FIRSTCHILD(node); child != NULL; child = child->nextSibling){
		switch(child->kind){
			case FuncDecl:
				generate_func_decl(child); break;
			default:
				generate_code_aux(child);
		}
	}
}

/**
 * @brief create the local section
 */
void create_local(){
	fprintf(in, "section .text\n");
	fprintf(in, "global %s\n", MAIN_FUNCTION);
}

/**
 * @brief translate the code into the nasm format
 * @param root a pointor to the Node structure
 */
void generate_code(Node *root){
	file_init();
	create_global();
	if(!root)
		return;
	create_local();
	generate_code_aux(root);
	file_deinit();
}
