%{
  /* lexer.l */
  /* Compatible avec parser.y */
  #include "parser.tab.h"
  #include "decl-var.h"
  int lineno=1;
  int charno=0;
%}

%option noinput nounput noyywrap
%x SHORT_COMMENT LONG_COMMENT
%%
[ \t\r]+		{charno += yyleng;}
\n			{charno=0; lineno++;}
"/*"			{ charno += yyleng;BEGIN LONG_COMMENT; }
"//"			{ charno += yyleng;BEGIN SHORT_COMMENT; }
&&			{ charno += yyleng;return AND; }
"||"			{ charno += yyleng;return OR; }
"+"|-			{ charno += yyleng; return ADDSUB; }
"*"|"/"|"%"			{ charno += yyleng;return DIVSTAR; }
"<"|"<="|">"|>=		{ charno += yyleng;return ORDER; }
==|!=			{ charno += yyleng;return EQ; }
int			{ strncpy(yylval.ident, yytext, MAXNAME); charno += yyleng;return SIMPLETYPE;}
char		{ strncpy(yylval.ident, yytext, MAXNAME); charno += yyleng;return SIMPLETYPE;}
print			{ charno += yyleng;return PRINT; }
readc			{ charno += yyleng;return READC; }
reade			{ charno += yyleng;return READE; }
void			{ charno += yyleng;return VOID; }
struct		{ charno += yyleng;return STRUCT; }
if			{ charno += yyleng;return IF; }
else			{ charno += yyleng;return ELSE; }
while			{ charno += yyleng;return WHILE; }
return			{ charno += yyleng;return RETURN; }
[a-zA-Z_][a-zA-Z0-9_]*	{ strncpy(yylval.ident, yytext, MAXNAME); charno += yyleng; return IDENT; }
[0-9]+			{ charno += yyleng; return NUM;}
'\\?.'			{ charno += yyleng; return CHARACTER; }
.			{ charno += yyleng; return yytext[0];}
<LONG_COMMENT>"*/"		{ BEGIN INITIAL; charno += yyleng; }
<LONG_COMMENT,SHORT_COMMENT>.		{charno += yyleng;}
<LONG_COMMENT>\n		{lineno++;charno=0;}
<SHORT_COMMENT>\n		{BEGIN INITIAL; lineno++;charno=0;}
%%
