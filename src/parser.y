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
%type <node> DeclVars Declarateurs
%type <type> Type

%%

Prog:  TypesVars DeclFoncts
    ;
TypesVars:
       TypesVars Type Declarateurs ';'
    |  TypesVars STRUCT IDENT '{' DeclChamps '}' ';'
    |  %empty
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
			strcpy(node->u.identifier, yytext);
			addSibling($$, node);
			if($<type>0 == 0)
				alreadyIn(yylval.ident);
			addVar(yylval.ident, $<type>0);
	   }
    |  IDENT {
	   		$$ = makeNode(Identifier);
			strcpy($$->u.identifier, yytext);
			if($<type>0 == 0)
				alreadyIn(yylval.ident);
			addVar(yylval.ident, $<type>0);
	}
    ;
DeclChamps :
       DeclChamps SIMPLETYPE Declarateurs ';'
    |  SIMPLETYPE Declarateurs ';'
    ;
DeclFoncts:
       DeclFoncts DeclFonct
    |  DeclFonct
    ;
DeclFonct:
       EnTeteFonct Corps
    ;
EnTeteFonct:
       Type IDENT '(' Parametres ')'
    |  VOID IDENT '(' Parametres ')'
    ;
Parametres:
       VOID
    |  ListTypVar
    ;
ListTypVar:
       ListTypVar ',' Type IDENT
    |  Type IDENT
    ;
Corps: '{' DeclVars SuiteInstr '}' { 
		printTree($2);
		deleteTree($2); 
		printTable();
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
