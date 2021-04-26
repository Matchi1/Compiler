#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
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
	int opt = 0, options_index = 0;
	static struct option long_options[] = {
		{"tree", no_argument, 0, 't'},
		{"symtabs", no_argument, 0, 's'},
		{"help", no_argument, 0, 'h'}
	};
	while((opt = getopt_long(argc, argv, "sth", long_options, &options_index)) != -1){
		switch(opt){
			case 't':
				opts->tree = 1; break;
			case 's':
				opts->st = 1; break;
			case 'h':
				opts->help = 1; break;
			default:
				exit(3);
		}
	}
}
