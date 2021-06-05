#ifndef __ABSTRACT_TREE__
#define __ABSTRACT_TREE__
/* abstract-tree.h */

typedef enum {
  Program,
  VarDeclList,
  VarsDecl,
  TypeDeclList,
  TypeDecl,
  FuncDeclList,
  FuncDecl,
  InstrList,
  Instr,
  ExprList,
  Expr,
  Header,
  IntLiteral,
  CharLiteral,
  Identifier,
  StructType,
  Type,
  Body,
  FuncCall,
  StructField,
  Bloc,
  TypesVars,
  GlobalDecl,
  StructDecl

  /* and allother node labels */
  /* The list must coincide with the strings in abstract-tree.c */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
} Kind;

typedef enum {
	And,
	Or,
	Add,
	Sub,
	Div,
	Mul,
	Mod,
	Greater,
	Gequal,
	Lower,
	Lequal,
	Equal,
	Diff,
	Neg
} Op;

typedef enum {
	Asign,
	Reade,
	Readc,
	Print,
	If,
	IfElse,
	While,
	Void,
	Return,
	Exp
} InstrType;

typedef struct Node {
  Kind kind;
  union {
    int integer, type;
	InstrType instruction;
	Op op;
    char character;
    char identifier[64];
  } u;
  struct Node *firstChild, *nextSibling;
  int lineno;
} Node;

Node *makeNode(Kind kind);
void addSibling(Node *node, Node *sibling);
void addChild(Node *parent, Node *child);
void deleteTree(Node*node);
void printTree(Node *node);
void print_kind(Node *node);

#define FIRSTCHILD(node) node->firstChild
#define SECONDCHILD(node) node->firstChild->nextSibling
#define THIRDCHILD(node) node->firstChild->nextSibling->nextSibling

#endif 
