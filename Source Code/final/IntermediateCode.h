#ifndef INTERCODE_H
#define INTERCODE_H

#include<iostream>
#include"ParseTree.h"
#include"DataStructure.h"
#include<string>
using namespace std;

extern struct ParseTree *root;
extern string inttostr(int n);
extern int strtoint(string s);

class InterCode
{
public:
	InterCode();
	~InterCode();

	void Add_Code(string);
	void Print_Code();

	string getNodeName(VarNode node);
	string getAryNodeName(AryNode node);
	string getLabelName();

	string VartoCode(string tempname, string op, VarNode node1, VarNode node2);
	string AssigntoCode(VarNode node1, VarNode node2);
	string ParamtoCode(VarNode node);
	string RettoCode(VarNode node);
	string ArgtoCode(VarNode node);

	int labelNum = 0;
	int arrayNum = 0;
	int tempNum = 0;
	int varNum = 0;

private:
	vector<string> Codes;
};

#endif
