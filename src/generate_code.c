#include <stdio.h>
#include <stdlib.h>
#include "decl-var.h"
#include "generate_code.h"

#define DEFAULT_FILE "_anonymous.asm"

char current_file[] = DEFAULT_FILE;
int current_label = 0;
char label_buf[10] = ".L";

char* label(){
	sprintf(&(label_buf[2]), "%d", current_label);
}

void create_global(){
	int i;
	FILE* in = fopen(DEFAULT_FILE, "w");
	fprintf(in, ".section\n");
	fprintf(in, "table dq");
	for(i = 0; i < nb_global_var() - 1; i++)
		fprintf(in, " 0,");
	fprintf(in, " 0\n");
}

void generate_global(){
}

void generate_code(){
	create_global();
}
