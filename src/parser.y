%{
	/* parser.y */
	/* Syntaxe du TPC pour le projet d'analyse syntaxique de 2020-2021 */
	#include <stdio.h>
	#include <string.h>
	extern int lineno;
	extern int charno;
	extern char ident[25];
	extern char* yytext;
	extern char text_line[100];
	int yylex();
	void yyerror(const char *);
%}

%code requires {
	#include "abstract-tree.h"
	#include "decl-var.h"
	#include "verif_type.h"
	#include "Option.h"
}

%union {
	Node* node;
	Op op;
	char type[10];
	int integer;
	char character;
}

%token CHARACTER
%token NUM
%token IDENT
%token SIMPLETYPE
%token ORDER EQ
%token ADDSUB
%token DIVSTAR
%token OR AND STRUCT IF WHILE RETURN VOID PRINT READC READE
%precedence ')'
%precedence ELSE
%type <node> DeclVars Declarateurs TypesVars DeclChamps DeclChampsAux Prog
%type <node> DeclFoncts DeclFonct Corps Parametres ListTypVar EnTeteFonct
%type <node> Exp TB FB M E T F LValue LValueAux
%type <node> Type ListExp Arguments SuiteInstr Instr SaveIdent CallName SaveOp

%%

Prog:  TypesVars DeclFoncts {
		extern Node* root;
		root = makeNode(Program);
		Node* node = makeNode(FuncDeclList);
		addChild(node, $2);
		addChild(root, $1);
		addChild(root, node);
	}
    ;
TypesVars:
       TypesVars Type Declarateurs ';' {
		$1 = makeNode(VarDeclList);
		addChild($1, $2);
		addChild($1, $3);
		addChild($$, $1);
	   }
    |  TypesVars STRUCT IDENT '{' DeclChamps '}' ';' {
		$1 = makeNode(StructType);
		strcpy($1->u.identifier, yylval.type);
		addChild($1, $5);
		addChild($$, $1);
	}
    |  %empty {
		$$ = makeNode(TypesVars);
	}
    ;
Type:
	  SIMPLETYPE {
		$$ = makeNode(Type);
		$$->lineno = lineno;
		strcpy($$->u.identifier, yylval.type);
	  }
	| STRUCT IDENT {
		$$ = makeNode(Type);
		$$->lineno = lineno;
		strcpy($$->u.identifier, yylval.type);
	  }
    ;
Declarateurs:
       Declarateurs ',' IDENT {
	   		$$ = $1;
			Node* node = makeNode(Identifier);
			node->lineno = lineno;
			strcpy(node->u.identifier, ident);
			addSibling($$, node);
	   }
    |  IDENT {
	   		$$ = makeNode(Identifier);
			$$->lineno = lineno;
			strcpy($$->u.identifier, ident);
	}
    ;
DeclChamps :
       DeclChamps SIMPLETYPE DeclChampsAux Declarateurs ';' {
	   		$$ = makeNode(VarDeclList);
			addChild($$, $4);
			addChild($$, $3);
			addSibling($$, $1);
	   }
    |  SIMPLETYPE DeclChampsAux Declarateurs ';' {
			$$ = makeNode(VarDeclList);
			addChild($$, $2);
			addChild($$, $3);
	}
    ;
DeclChampsAux :
	   %empty {
			$$ = makeNode(Type);
			$$->lineno = lineno;
			strcpy($$->u.identifier, yylval.type);
	   }
DeclFoncts:
       DeclFoncts DeclFonct {
	   		$$ = $1;
			addSibling($$, $2);
	   }
    |  DeclFonct
    ;
DeclFonct:
       EnTeteFonct Corps {
	   		$$ = makeNode(DeclFunc);
			addChild($$, $1);
			addChild($$, $2);
	   }
    ;
EnTeteFonct:
       Type IDENT SaveIdent '(' Parametres ')' {
	   		$$ = $3;
			addChild($$, $1);
			addChild($$, $5);
	   }
    |  VOID IDENT SaveIdent '(' Parametres ')' {
	   		$$ = $3;
			addChild($$, $5);
	 }
    ;
SaveIdent:
	   %empty {
	   		$$ = makeNode(EnTete);
			$$->lineno = lineno;
			strcpy($$->u.identifier, ident);
	   }
Parametres:
       VOID	{
			$$ = NULL;
	   }
    |  ListTypVar {
			$$ = makeNode(TypeDeclList);
			addChild($$, $1);
	}
    ;
ListTypVar:
       ListTypVar ',' Type IDENT {
			$$ = makeNode(TypeDecl);
			addChild($$, $3);
			Node* node = makeNode(Identifier);
			node->lineno = lineno;
			strcpy(node->u.identifier, ident);
			addChild($$, node);
			addSibling($$, $1);
	   }
    |  Type IDENT {
			$$ = makeNode(TypeDecl);
			addChild($$, $1);
			Node* node = makeNode(Identifier);
			node->lineno = lineno;
			strcpy(node->u.identifier, ident);
			addChild($$, node);
	}
    ;
Corps: '{' DeclVars SuiteInstr '}' { 
	 	$$ = makeNode(Corps);
		addChild($$, $2);
		addChild($$, $3);
	}
    ;
DeclVars:
       DeclVars Type Declarateurs ';' {
	   $1 = makeNode(VarDeclList);
	   addChild($$, $1);
	   addChild($1, $2);
	   addChild($1, $3);
	}
    |  %empty	{
		$$ = makeNode(DeclVars);
	}
    ;
SuiteInstr:
       SuiteInstr Instr {
	   	$$ = $1;
		addChild($$, $2);
	}
    |  %empty {
		$$ = makeNode(ListInstr);
	}
    ;
Instr:
       LValue '=' Exp ';' {
		$$ = makeNode(Instr);   	
		$$->lineno = lineno;
		$$->u.instruction = Affect;
		addChild($$, $1);
		addChild($$, $3);
	}
    |  READE '(' LValue ')' ';' {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = Reade;
		addChild($$, $3);
	}
    |  READC '(' LValue ')' ';' {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = Readc;
		addChild($$, $3);
	}
    |  PRINT '(' Exp ')' ';' {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = Print;
		addChild($$, $3);
	}
    |  IF '(' Exp ')' Instr {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = If;
	}
    |  IF '(' Exp ')' Instr ELSE Instr {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = IfElse;
	}
    |  WHILE '(' Exp ')' Instr {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = While;
	}
    |  Exp ';'	{
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = Vide;
		addChild($$, $1);
	}
    |  RETURN Exp ';' {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = Return;
		addChild($$, $2);
	}
    |  RETURN ';' {
		$$ = makeNode(Instr);
		$$->lineno = lineno;
		$$->u.instruction = Return;
	}
    |  '{' SuiteInstr '}' {
		$$ = makeNode(Bloc);
	}
    |  ';' {
		$$ = NULL;
	}
    ;
Exp :  Exp OR SaveOp TB {
	   $$ = $3;
	   addChild($$, $1);
	   addChild($$, $4);
	}
    |  TB
    ;
TB  :  TB AND SaveOp FB {
	   $$ = $3;
	   addChild($$, $1);
	   addChild($$, $4);
	}
    |  FB
    ;
FB  :  FB EQ SaveOp M {
	   $$ = $3;
	   addChild($$, $1);
	   addChild($$, $4);
	}
    |  M
    ;
M   :  M ORDER SaveOp E {
	   $$ = $3;
	   addChild($$, $1);
	   addChild($$, $4);
	}
    |  E
    ;
E   :  E ADDSUB SaveOp T {
	   $$ = $3;
	   addChild($$, $1);
	   addChild($$, $4);
	}
    |  T
    ;
T   :  T DIVSTAR SaveOp F {
	   $$ = $3;
	   addChild($$, $1);
	   addChild($$, $4);
	}
    |  F
    ;
F   :  ADDSUB SaveOp F {
	   $$ = $2;
	   addChild($$, $3);
	}
    |  '!' F {
       $$ = makeNode(Expr);
       addChild($$, $2);
	   $$->u.op = Neg;
	}
    |  '(' Exp ')' {$$ = $2;}
    |  NUM {
       $$ = makeNode(IntLiteral);
       $$ -> u.integer = yylval.integer;
	}
    |  CHARACTER {
       $$ = makeNode(CharLiteral);
       $$ -> u.character = yylval.character;
	}
    |  LValue
    |  IDENT CallName '(' Arguments  ')' {
       $$ = $2;
	   addChild($$, $4);
	}
    ;
SaveOp:
	   %empty {
		$$ = makeNode(Expr);
		$$->lineno = lineno;
		$$->u.op = yylval.op;
	}
	;
CallName:
	   %empty {
	   		$$ = makeNode(FunctionCall);
			$$->lineno = lineno;
			strcpy($$->u.identifier, ident);
	   }
LValue:
       IDENT {
		$$ = makeNode(Identifier);
		$$->lineno = lineno;
		strcpy($$->u.identifier, ident);
	}
    |  IDENT LValueAux '.' IDENT {
       Node* node = makeNode(Identifier);
       strcpy(node->u.identifier, ident);
       $$ = makeNode(StructField); 
		$$->lineno = lineno;
       addChild($$, node);
       addChild($$, $2);
	}
    ;
LValueAux:
	   %empty	{
       $$ = makeNode(Identifier);  
	   $$->lineno = lineno;
       strcpy($$->u.identifier, ident);
    }
Arguments:
       ListExp { $$ = $1; }
    |  %empty  { $$ = NULL; }
    ;
ListExp:
       ListExp ',' Exp {
		$$ = $1;
	   	addChild($$, $3);
	}
    |  Exp	{
		$$ = makeNode(ListExp);
		addChild($$, $1);
	}
    ;
%%

/* 
	Root of the tree that will be created
	during the parsing.	
*/
Node* root;

void display_error(){
	int index;
	for(index = 0; index < charno - 1; index++){
		if(text_line[index] == '\t')
			printf("\t");
		else
			printf(" ");
	}
	printf("^\n");
}

void yyerror(const char *s) {
	printf("Erreur à la ligne %d colonne %d!\n", lineno, charno);
	printf("%s\n", text_line);
	display_error();
}

int main(int argc, char** argv) {
	int ret_value;
	Options opts;

	init_options(&opts);
	check_options(argc, argv, &opts);
	ret_value = yyparse();
	if(root != NULL)
		create_ST(root);

	if(opts.tree)
		printTree(root);
	if(opts.st)
		printTable();

	return ret_value;
}
