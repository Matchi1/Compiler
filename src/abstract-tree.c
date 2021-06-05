/* abstract-tree.c */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "abstract-tree.h"
extern int lineno;       /* from lexer */

static const char *StringFromKind[] = {
  "Program",
  "VarDeclList",
  "VarsDecl",
  "TypeDeclList",
  "TypeDecl",
  "FuncDeclList",
  "FuncDecl",
  "InstrList",
  "Instr",
  "ExprList",
  "Expr",
  "Header",
  "IntLiteral",
  "CharLiteral",
  "Identifier",
  "StructType",
  "Type",
  "Body",
  "FuncCall",
  "StructField",
  "Bloc",
  "TypesVars",
  "GlobalDecl",
  "StructDecl"
  
  /* and all other node labels */
  /* The list must coincide with the enum in abstract-tree.h */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
};

static const char *StringFromOp[] = {
	"&&",
	"||",
	"+",
	"-",
	"/",
	"*",
	"%",
	">",
	">=",
	"<",
	"<=",
	"==",
	"!=",
	"!"
};

static const char *StringFromInstrType[] = {
	"Asign",
	"Reade",
	"Readc",
	"Print",
	"If",
	"IfElse",
	"While",
	"",
	"Return",
	"Exp"
};

Node *makeNode(Kind kind) {
  Node *node = (Node*)malloc(sizeof(Node));
  if (!node) {
    printf("Run out of memory\n");
    exit(1);
  }
  node->kind = kind;
  node-> firstChild = node->nextSibling = NULL;
  node->lineno=lineno;
  return node;
}

void print_kind(Node *node){
	printf("%s\n", StringFromKind[node->kind]);
}

void addSibling(Node *node, Node *sibling) {
  Node *curr = node;
  while (curr->nextSibling != NULL) {
    curr = curr->nextSibling;
  }
  curr->nextSibling = sibling;
}

void addChild(Node *parent, Node *child) {
  if (parent->firstChild == NULL) {
    parent->firstChild = child;
  }
  else {
    addSibling(parent->firstChild, child);
  }
}

void deleteTree(Node *node) {
  if (node->firstChild) {
    deleteTree(node->firstChild);
  }
  if (node->nextSibling) {
    deleteTree(node->nextSibling);
  }
  free(node);
}

void printTree(Node *node) {
  static bool rightmost[128]; // current node is rightmost sibling
  static int depth = 0;       // depth of current node
  for (int i = 1; i < depth; i++) { // 2502 = vertical line
    printf(rightmost[i] ? "    " : "\u2502   ");
  }
  if (depth > 0) { // 2514 = up and right; 2500 = horiz; 251c = vertical and right 
    printf(rightmost[depth] ? "\u2514\u2500\u2500 " : "\u251c\u2500\u2500 ");
  }
  printf("%s", StringFromKind[node->kind]);
  switch (node->kind) {
    case IntLiteral: printf(": %d", node->u.integer); break;
    case CharLiteral: printf(": '%c'", node->u.character); break;
    case Identifier: printf(": %s", node->u.identifier); break;
    case StructType: printf(": %s", node->u.identifier); break;
    case Header: printf(": %s", node->u.identifier); break;
    case TypeDecl: printf(": %s", node->u.identifier); break;
    case Type: printf(": %s", node->u.identifier); break;
    case Expr: printf(": %s", StringFromOp[node->u.op]); break;
    case FuncCall: printf(": %s", node->u.identifier); break;
    case Instr: printf(": %s", StringFromInstrType[node->u.instruction]); break;
    default: break;
  }
  printf("\n");
  depth++;
  for (Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    rightmost[depth] = (child->nextSibling) ? false : true;
    printTree(child);
  }
  depth--;
}
