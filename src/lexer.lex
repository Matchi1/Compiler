%{
	/* lexer.l */
	/* Compatible avec parser.y */
	#include "parser.tab.h"
	#include "decl-var.h"
	#include "abstract-tree.h"
	char text_line[100];
	int index_text = 0;
	int lineno=1;
	int charno=0;
	char ident[25];
	#undef YY_INPUT
	#define YY_INPUT(buf, result, max_size){ \
		char c = fgetc(yyin); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
		/* Vérifie si la ligne a déjà été enregistré */ \
		if(index_text == 0){ \
			while(c != EOF && c != '\n'){ \
				text_line[index_text] = c; \
				index_text++; \
				c = fgetc(yyin); \
			} \
			text_line[index_text] = '\0'; \
			fseek(yyin, -index_text, SEEK_CUR); \
		} \
	}
%}

%option noinput nounput noyywrap
%x SHORT_COMMENT LONG_COMMENT
%%
[ \t\r]+			{ charno += yyleng;}
\n					{ charno = 0; lineno++;}
"/*"				{ charno += yyleng;BEGIN LONG_COMMENT; }
"//"				{ charno += yyleng;BEGIN SHORT_COMMENT; }
&&					{ yylval.op = And; charno += yyleng;return AND; }
"||"				{ yylval.op = Or; charno += yyleng;return OR; }
"+"					{ yylval.op = Add; charno += yyleng; return ADDSUB; }
-					{ yylval.op = Sub; charno += yyleng; return ADDSUB; }
"%"					{ yylval.op = Mod; charno += yyleng;return DIVSTAR; }
"/"					{ yylval.op = Div; charno += yyleng;return DIVSTAR; }
"*"					{ yylval.op = Mul; charno += yyleng;return DIVSTAR; }
>=					{ yylval.op = Gequal; charno += yyleng;return ORDER; }
">"					{ yylval.op = Greater; charno += yyleng;return ORDER; }
"<="				{ yylval.op = Lequal; charno += yyleng;return ORDER; }
"<"					{ yylval.op = Lower; charno += yyleng;return ORDER; }
==					{ yylval.op = Equal; charno += yyleng;return EQ; }
!=					{ yylval.op = Diff; charno += yyleng;return EQ; }
int					{ strncpy(yylval.type, yytext, MAXNAME); charno += yyleng;return SIMPLETYPE;}
char				{ strncpy(yylval.type, yytext, MAXNAME); charno += yyleng;return SIMPLETYPE;}
print				{ charno += yyleng;return PRINT; }
readc				{ charno += yyleng;return READC; }
reade				{ charno += yyleng;return READE; }
void				{ charno += yyleng;return VOID; }
struct				{ charno += yyleng;return STRUCT; }
if					{ charno += yyleng;return IF; }
else				{ charno += yyleng;return ELSE; }
while				{ charno += yyleng;return WHILE; }
return				{ charno += yyleng;return RETURN; }
[a-zA-Z_][a-zA-Z0-9_]*	{ strncpy(ident, yytext, MAXNAME); charno += yyleng; return IDENT; }
[0-9]+				{ yylval.integer = atoi(yytext); charno += yyleng; return NUM;}
'\\?.'				{ yylval.character = yytext[1]; charno += yyleng; return CHARACTER; }
.					{ charno += yyleng; return yytext[0];}
<LONG_COMMENT>"*/"					{ BEGIN INITIAL; charno += yyleng; }
<LONG_COMMENT,SHORT_COMMENT>.		{ charno += yyleng;}
<LONG_COMMENT>\n					{ lineno++;charno=0;}
<SHORT_COMMENT>\n					{ BEGIN INITIAL; lineno++;charno=0;}
%%
