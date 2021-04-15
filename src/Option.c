#include <stdlib.h>
#include <unistd.h>
#include "Option.h"

void init_options(Options* opts){
	opts->tree = 0;
	opts->st = 0;
}

/*
	t : draw tree
	s : draw symbol table
*/
void check_options(int argc, char* argv[], Options* opts){
	int opt;
	while((opt = getopt(argc, argv, "st")) != -1){
		switch(opt){
			case 't':
				opts->tree = 1; break;
			case 's':
				opts->st = 1; break;
			default:
				exit(EXIT_FAILURE);
		}
	}
}
