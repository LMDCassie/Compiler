%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "y.tab.h"
#include "ParseTree.h"
using namespace std;

void count(void);
int column = 0;

%}

%option yylineno

D	[0-9]
L	[a-zA-Z_]

%%

"int"		{count(); yylval.gt =Create_Parse_Tree("int",0,yylineno); return INT;}
"void"		{count(); yylval.gt =Create_Parse_Tree("void",0,yylineno); return VOID;}
"if"		{count(); yylval.gt =Create_Parse_Tree("IF",0,yylineno); return IF;}
"else"		{count(); yylval.gt =Create_Parse_Tree("ELSE",0,yylineno); return ELSE;}
"while"		{count(); yylval.gt =Create_Parse_Tree("WHILE",0,yylineno); return WHILE;}
"return"	{count(); yylval.gt =Create_Parse_Tree("RETURN",0,yylineno); return RETURN;}

{L}({L}|{D})*	{count(); yylval.gt =Create_Parse_Tree("ID",0,yylineno); return ID;}
[0]|[1-9]{D}*	{count(); yylval.gt =Create_Parse_Tree("NUM",0,yylineno); return NUM;}

"<="	{count(); yylval.gt =Create_Parse_Tree("LE_OP",0,yylineno); return RELOP;}
">="	{count(); yylval.gt =Create_Parse_Tree("GE_OP",0,yylineno); return RELOP;}
"=="	{count(); yylval.gt =Create_Parse_Tree("EQ_OP",0,yylineno); return RELOP;}
"!="	{count(); yylval.gt =Create_Parse_Tree("NE_OP",0,yylineno); return RELOP;}
";"		{count(); yylval.gt =Create_Parse_Tree(";",0,yylineno); return ';';}
"["		{count(); yylval.gt =Create_Parse_Tree("[",0,yylineno); return '[';}
"]"		{count(); yylval.gt =Create_Parse_Tree("]",0,yylineno); return ']';}
"("		{count(); yylval.gt =Create_Parse_Tree("(",0,yylineno); return '(';}
")"		{count(); yylval.gt =Create_Parse_Tree(")",0,yylineno); return ')';}
","		{count(); yylval.gt =Create_Parse_Tree(",",0,yylineno); return ',';}
"{"		{count(); yylval.gt =Create_Parse_Tree("{",0,yylineno); return '{';}
"}"		{count(); yylval.gt =Create_Parse_Tree("}",0,yylineno); return '}';}
"="		{count(); yylval.gt =Create_Parse_Tree("=",0,yylineno); return '=';}
"<"		{count(); yylval.gt =Create_Parse_Tree("<",0,yylineno); return RELOP;}
">"		{count(); yylval.gt =Create_Parse_Tree(">",0,yylineno); return RELOP;}
"+"		{count(); yylval.gt =Create_Parse_Tree("+",0,yylineno); return ADDOP;}
"-"		{count(); yylval.gt =Create_Parse_Tree("-",0,yylineno); return ADDOP;}
"*"		{count(); yylval.gt =Create_Parse_Tree("*",0,yylineno); return MULOP;}
"/"		{count(); yylval.gt =Create_Parse_Tree("/",0,yylineno); return MULOP;}

[\t\n]	{count();}
" "		{count();}
.		{printf("unmatched characters encountered.\n");}

%%

int yywrap()
{
	return 1;
}

void count(void)
{
	int i;
	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 4 - (column % 4);
		else
			column++;
	ECHO;
}
