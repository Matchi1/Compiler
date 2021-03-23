%{
/* parser.y */
/* Syntaxe du TPC pour le projet d'analyse syntaxique de 2020-2021 */
  #include <stdio.h>
  #include <string.h>
  extern int lineno;
  extern int charno;
  int yylex();
  void yyerror(const char *);
  extern char* yytext;
%}

%code requires {
	#include "abstract-tree.h"
	#include "decl-var.h"
}

%union {
	Node* node;
	int type;
	int op;
	char ident[MAXNAME];
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
%type <node> DeclVars Declarateurs TypesVars DeclChamps Prog
%type <node> DeclFoncts DeclFonct Corps Parametres ListTypVar EnTeteFonct
%type <type> Type

%%

Prog:  TypesVars DeclFoncts {
		$$ = makeNode(Program);
		Node* node = makeNode(FuncDeclList);
		addChild(node, $2);
		addChild($$, $1);
		addChild($$, node);
		printTree($$);
		deleteTree($$);
	}
    ;
TypesVars:
       TypesVars Type Declarateurs ';' {
		$1 = makeNode(VarDeclList);
		addChild($1, $3);
		addChild($$, $1);
	   }
    |  TypesVars STRUCT IDENT '{' DeclChamps '}' ';' {
		$1 = makeNode(StructType);
		strcpy($1->u.identifier, yylval.ident);
		addChild($1, $5);
		addChild($$, $1);
	}
    |  %empty {
		$$ = makeNode(VarDeclList);
	}
    ;
Type:
	  SIMPLETYPE {
		$$ = 0;
	  }
	| STRUCT IDENT {
		$$ = 1;
	  }
    ;
Declarateurs:
       Declarateurs ',' IDENT {
	   		$$ = $1;
			Node* node = makeNode(Identifier);
			strcpy(node->u.identifier, yylval.ident);
			addSibling($$, node);
			if($<type>0 == 0)
				alreadyIn(yylval.ident);
			addVar(yylval.ident, $<type>0);
	   }
    |  IDENT {
	   		$$ = makeNode(Identifier);
			strcpy($$->u.identifier, yylval.ident);
			if($<type>0 == 0)
				alreadyIn(yylval.ident);
			addVar(yylval.ident, $<type>0);
	}
    ;
DeclChamps :
       DeclChamps SIMPLETYPE Declarateurs ';' {
	   		$$ = makeNode(VarDeclList);
			addChild($$, $3);
			addSibling($$, $1);
	   }
    |  SIMPLETYPE Declarateurs ';' {
			$$ = makeNode(VarDeclList);
			addChild($$, $2);
	}
    ;
DeclFoncts:
       DeclFoncts DeclFonct {
	   		$$ = $1;
			addSibling($$, $2);
	   }
    |  DeclFonct {
			$$ = $1;
	}
    ;
DeclFonct:
       EnTeteFonct Corps {
	   		$$ = makeNode(FuncDecl);
			addChild($$, $1);
			addChild($$, $2);
	   }
    ;
EnTeteFonct:
       Type IDENT '(' Parametres ')' {
	   		$$ = makeNode(EnTete);
			strcpy($$->u.identifier, yylval.ident);
			addChild($$, $4);
	   }
    |  VOID IDENT '(' Parametres ')' {
	   		$$ = makeNode(EnTete);
			strcpy($$->u.identifier, yylval.ident);
			addChild($$, $4);
	 }
    ;
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
			strcpy($$->u.identifier, yylval.ident);
			addSibling($$, $1);
	   }
    |  Type IDENT {
			$$ = makeNode(TypeDecl);
			strcpy($$->u.identifier, yylval.ident);
	}
    ;
Corps: '{' DeclVars SuiteInstr '}' { 
	 	$$ = makeNode(Corps);
		addChild($$, $2);
	}
    ;
DeclVars:
       DeclVars Type Declarateurs ';' {
	   $1 = makeNode(VarDeclList);
	   addChild($1, $3);
	   addChild($$, $1);
	}
    |  %empty	{
		$$ = makeNode(VarDeclList);
	}
    ;
SuiteInstr:
       SuiteInstr Instr
    |  %empty
    ;
Instr:
       LValue '=' Exp ';'
    |  READE '(' LValue ')' ';'
    |  READC '(' LValue ')' ';'
    |  PRINT '(' Exp ')' ';'
    |  IF '(' Exp ')' Instr
    |  IF '(' Exp ')' Instr ELSE Instr
    |  WHILE '(' Exp ')' Instr
    |  Exp ';'
    |  RETURN Exp ';'
    |  RETURN ';'
    |  '{' SuiteInstr '}'
    |  ';'
    ;
Exp :  Exp OR TB 
    |  TB
    ;
TB  :  TB AND FB
    |  FB
    ;
FB  :  FB EQ M
    |  M
    ;
M   :  M ORDER E
    |  E
    ;
E   :  E ADDSUB T
    |  T
    ;
T   :  T DIVSTAR F
    |  F
    ;
F   :  ADDSUB F
    |  '!' F
    |  '(' Exp ')'
    |  NUM
    |  CHARACTER
    |  LValue 
    |  IDENT '(' Arguments  ')'
    ;
LValue:
       IDENT
    |  IDENT '.' IDENT
    ;
Arguments:
       ListExp
    |  %empty
    ;
ListExp:
       ListExp ',' Exp
    |  Exp
    ;

%%

int main(int argc, char** argv) {
	return yyparse();
}

void yyerror(const char *s){
	fprintf(stderr, "%s near line %d character %d\n", s, lineno,charno);
}
