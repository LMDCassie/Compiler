#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include"ParseTree.h"
using namespace std;

struct VarNode
{
	string name;
	string type;
	int num = -1;
	bool isPointer = false;
	string CondString;
};

struct FunNode
{
	string name;
	string rtype;
	bool isdefine = false;
	bool no_param = false;
	vector<VarNode> paramlist;
};

struct AryNode
{
	string name;
	string type;
	int num = -1;
};

class CodeBlock
{
public:
	FunNode func;
	bool isfunc = false;
	string breaklabel;
	bool isbreak = false;

	map<string, struct VarNode> VarMap;
	map<string, struct AryNode> AryMap;
};

#endif
