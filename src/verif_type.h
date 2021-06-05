#ifndef __VERIF_TYPE__
#define __VERIF_TYPE__

#include "abstract-tree.h"

#define SEM_ERR -1
#define VOID_TYPE 0
#define INT_TYPE 1
#define CHAR_TYPE 2

void check_semantic_error(Node* root);

#endif
