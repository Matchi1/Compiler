#ifndef __VERIF_TYPE__
#define __VERIF_TYPE__

#include "abstract-tree.h"

/*
 * Return : - -1 semantic error
 * 			- 0 int 
 * 			- 1 char
 * */
int verif_expr(Node *node);

#endif
