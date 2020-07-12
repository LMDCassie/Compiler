#ifndef PARSETREE_H
#define PARSETREE_H

#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdarg>
#include<iostream>
#include<string>

extern char *yytext;
extern int yylineno;
using namespace std;

string inttostr(int n);
int strtoint(string s);

struct ParseTree {
	string name;
	string content;
	int line;
	struct ParseTree *left;
	struct ParseTree *right;
};

extern struct ParseTree *root;

struct ParseTree* Create_Parse_Tree(string name, int num, ...);
void Free_Parse_Tree(ParseTree *node);
void eval(struct ParseTree *head, int level);

#endif
