#ifndef __OPTION__
#define __OPTION__

typedef struct {
	int tree, st;
}Options;

void init_options(Options* opts);

void check_options(int argc, char* argv[], Options* opts);

#endif
