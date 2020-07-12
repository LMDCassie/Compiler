#ifndef SEMANTIC_H
#define SEMANTIC_H

#include<map>
#include<fstream>
#include<queue>
#include<vector>
#include"DataStructure.h"
#include"ParseTree.h"
#include"IntermediateCode.h"
using namespace std;

extern string inttostr(int n);
extern int strtoint(string s);

class SemanticParser
{
public:
	SemanticParser(ParseTree *node);
	~SemanticParser();

private:
	map<string, FunNode> funcPool;
	vector<CodeBlock> CodeBlockStack;
	InterCode intercode;

	struct ParseTree *root;

	void ParserInit();
	void ParseTree_Parse(struct ParseTree *node);

	struct ParseTree* declare_parse(struct ParseTree *node);
	void var_declare_parse(struct ParseTree *node);
	void fun_declare_parse(struct ParseTree *node);
	void parameters_parse(struct ParseTree *node, string funcName, bool definite);
	void parameter_list_parse(struct ParseTree* node, string funcName, bool definite);
	void parameter_parse(struct ParseTree* node, string funcName, bool definite);
	void compound_statement_parse(struct ParseTree *node);
	void local_declare_parse(struct ParseTree *node);
	void statement_list_parse(struct ParseTree *node);
	void statement_parse(struct ParseTree *node);
	void expr_statement_parser(struct ParseTree *node);
	void select_statement_parse(struct ParseTree *node);
	void loop_statement_parse(struct ParseTree *node);
	void return_statement_parse(struct ParseTree *node);
//-------------------------------------------------------------------------
	VarNode var_parse(struct ParseTree *node);
	VarNode expr_parse(struct ParseTree *node);
	VarNode simple_expr_parse(struct ParseTree *node);
	VarNode additive_expr_parse(struct ParseTree *node);
	string relop_parse(struct ParseTree *node);
	string addop_parse(struct ParseTree *node);
	string mulop_parse(struct ParseTree *node);
	VarNode term_parse(struct ParseTree *node);
	VarNode factor_parse(struct ParseTree *node);
	VarNode call_parse(struct ParseTree *node);

	void arg_list_parse(struct ParseTree *node, string funcName);
	void args_parse(struct ParseTree *node, string funcName);

	struct VarNode VarNode_search(string name);
	struct AryNode getAryNode(string name);
	string getFunRetType();
	bool var_search(string name);

	void error(int line, string error);
	void print_code();
	void genDot(ParseTree *node);

	struct VarNode createTempVar(string name, string type);
};

#endif

