#include"Semantic.h"

SemanticParser::SemanticParser(ParseTree *root)
{
	this->root = root;
	ParserInit();
}

SemanticParser::~SemanticParser() {
	print_code();
}

void SemanticParser::ParserInit()
{
	CodeBlock oneblock;
	CodeBlockStack.push_back(oneblock);

	FunNode writeNode;
	writeNode.rtype = "void";
	writeNode.name = "print";

	VarNode pnode;
	pnode.type = "int";
	writeNode.paramlist.push_back(pnode);
	funcPool.insert({ "print",writeNode });

	FunNode readNode;
	readNode.rtype = "int";
	readNode.name = "read";
	
	funcPool.insert({ "read",readNode });

	ParseTree_Parse(root);
	genDot(this->root);
}

void SemanticParser::ParseTree_Parse(struct ParseTree *node)
{
	if (node == NULL || node->line == -1)
		return;
	
	if (node->name == "declaration") {
		node = declare_parse(node);
	}

	if (node != NULL) {
		ParseTree_Parse(node->left);
		ParseTree_Parse(node->right);
	}
}

struct ParseTree* SemanticParser::declare_parse(struct ParseTree *node)
{
	if (node->left->name == "var_declaration")
		var_declare_parse(node->left);
	else
		fun_declare_parse(node->left);
	return 
		node->right;
}

void SemanticParser::var_declare_parse(struct ParseTree *node)
{
	ParseTree *type_specifier = node->left;
	ParseTree *ID = node->left->right;
	string varType = type_specifier->left->content;
	
	if (varType == "void")
		error(type_specifier->left->line, "void type is assigned to a variable.");
	
	if (ID->right->name == ";") {
		string varName = ID->content;
		if (!var_search(varName)) {
			VarNode newVar;
			newVar.name = varName;
			newVar.type = varType;
			newVar.num = intercode.varNum++;
			CodeBlockStack.back().VarMap.insert({ varName,newVar });
		}
		else {
			error(ID->line, "variable declaration repeated.");
		}
	}
	else {
		string arrayName = ID->content;
		string arrayType = varType;

		string content = ID->right->right->content;
		string tempName1 = "temp" + inttostr(intercode.tempNum);
		intercode.tempNum++;
		
		VarNode rNode = createTempVar(tempName1, "int");
		CodeBlockStack.back().VarMap.insert({ tempName1, rNode });
		intercode.Add_Code(tempName1 + " := #" + content);

		if (rNode.type != "int")
			error(ID->right->line, "array size is not a intger.");

		VarNode tNode;

		string tempName2 = "temp" + inttostr(intercode.tempNum);
		intercode.tempNum++;
		tNode = createTempVar(tempName2, "int");
		CodeBlockStack.back().VarMap.insert({ tempName2,tNode });

		VarNode tempVar3;
		string tempName3 = "temp" + inttostr(intercode.tempNum);
		intercode.tempNum++;
		tempVar3.name = tempName3;
		tempVar3.type = "int";
		CodeBlockStack.back().VarMap.insert({ tempName3,tempVar3 });
		intercode.Add_Code(tempName3 + " := #4");
		intercode.Add_Code(tNode.name + " := " + tempName3 + " * " + rNode.name);

		AryNode aNode;
		aNode.name = arrayName;
		aNode.type = arrayType;
		aNode.num = intercode.arrayNum++;
		intercode.Add_Code("DEC " + intercode.getAryNodeName(aNode) + " " + tNode.name);

		CodeBlockStack.back().AryMap.insert({ arrayName,aNode });
	}
}

void SemanticParser::fun_declare_parse(struct ParseTree *node)
{
	ParseTree *type_specifier = node->left;
	ParseTree *ID = node->left->right;
	ParseTree *compound_statement = ID->right->right->right->right;

	string funcType = type_specifier->left->content;
	string funcName = ID->content;

	bool isdeclared = false;
	FunNode declarFunc;
	if (funcPool.find(funcName) != funcPool.end()) {
		if (funcPool[funcName].isdefine) {
			error(ID->line, "Function " + funcName + " definition duplicated.");
		}
		else {
			isdeclared = true;
			declarFunc = funcPool[funcName];
			funcPool.erase(funcPool.find(funcName));
		}
	}
	CodeBlock funCodeBlock;
	funCodeBlock.isfunc = true;
	funCodeBlock.func.name = funcName;
	funCodeBlock.func.rtype = funcType;
	funCodeBlock.func.isdefine = true;
	CodeBlockStack.push_back(funCodeBlock);
	funcPool.insert({ funcName,funCodeBlock.func });
	intercode.Add_Code("FUNCTION " + funcName + " :");

	parameters_parse(ID->right->right, funcName, true);

	FunNode func = funcPool[funcName];
	if (isdeclared) {
		if (func.rtype != declarFunc.rtype)
			error(type_specifier->left->line, "function return type not equal to the function declared.");
		cout << funCodeBlock.func.paramlist.size() << endl;
		if (func.paramlist.size() != declarFunc.paramlist.size())
			error(ID->right->right->left->line, "function parameter numbers not equal to the function declared.");
		for (int i = 0; i < funCodeBlock.func.paramlist.size(); i++)
			if (func.paramlist[i].type != declarFunc.paramlist[i].type)
				error(ID->right->right->left->line, "parameter " + funCodeBlock.func.paramlist[i].name + "'s type not equal to the function declared.");
	}
	funCodeBlock.func = func;
	compound_statement_parse(compound_statement);
	CodeBlockStack.pop_back();
}

void SemanticParser::parameters_parse(struct ParseTree *node, string funcName, bool definite)
{
	if (node->left->name == "param_list")
		parameter_list_parse(node->left, funcName, definite);
	else
		funcPool[funcName].no_param = true;
}

void SemanticParser::parameter_list_parse(struct ParseTree* node, string funcName, bool definite)
{
	if (node->left->name == "param_list") {
		parameter_list_parse(node->left, funcName, definite);
	}
	else {
		parameter_parse(node->left, funcName, definite);
	}
	if (node->right != NULL)
		if (node->right->name == ",")
			parameter_parse(node->right->right, funcName, definite);
}

void SemanticParser::parameter_parse(struct ParseTree* node, string funcName, bool definite)
{
	ParseTree *type_specifier = node->left;
	ParseTree *ID = node->left->right;
	string typeName = type_specifier->left->content;
	if (typeName == "void")
		error(type_specifier->line, "void can't decide a parameter.");

	string varName = ID->content;
	VarNode newNode;
	newNode.name = varName;
	newNode.type = typeName;
	
	if (definite) {
		newNode.num = intercode.varNum++;
		CodeBlockStack.back().func.paramlist.push_back(newNode);
	}

	funcPool[funcName].paramlist.push_back(newNode);

	CodeBlockStack.back().VarMap.insert({ varName,newNode });
	
	if (definite)
		intercode.Add_Code(intercode.ParamtoCode(newNode));
}

void SemanticParser::compound_statement_parse(struct ParseTree *node)
{
	local_declare_parse(node->left->right);
	statement_list_parse(node->left->right->right);
}

void SemanticParser::local_declare_parse(struct ParseTree *node)
{
	if (node->left != NULL)
		if (node->left->name == "local_declarations") {
			local_declare_parse(node->left);
			var_declare_parse(node->left->right);
		}
}

void SemanticParser::statement_list_parse(struct ParseTree *node)
{	
	if (node->left != NULL)
		if (node->left->name == "statement_list")
		{
			statement_list_parse(node->left);
			statement_parse(node->left->right);
		}
}

void SemanticParser::statement_parse(struct ParseTree* node)
{
	if (node->left->name == "compound_stmt") {
		compound_statement_parse(node->left);
	}
	if (node->left->name == "expression_stmt") {
		expr_statement_parser(node->left);
	}
	if (node->left->name == "selection_stmt") {
		select_statement_parse(node->left);
	}
	if (node->left->name == "iteration_stmt") {
		loop_statement_parse(node->left);
	}
	if (node->left->name == "return_stmt") {
		return_statement_parse(node->left);
	}
}

void SemanticParser::expr_statement_parser(struct ParseTree *node)
{
	if (node->left->name == "expression") {
		expr_parse(node->left);//
	}
}

void SemanticParser::select_statement_parse(struct ParseTree *node)
{
	if (node->left->right->right->right->right->right == NULL) {
		CodeBlock newCodeBlock;
		CodeBlockStack.push_back(newCodeBlock);

		VarNode exp_rnode = expr_parse(node->left->right->right);
		string label1 = intercode.getLabelName();
		string label2 = intercode.getLabelName();
		if (exp_rnode.type == "bool") {
			intercode.Add_Code("IF " + exp_rnode.CondString + " GOTO " + label1);
		}
		else {
			string tempZeroName = "temp" + inttostr(intercode.tempNum);
			intercode.tempNum++;
			VarNode newZeroNode = createTempVar(tempZeroName, "int");
			intercode.Add_Code(tempZeroName + " := #0");
			intercode.Add_Code("IF " + intercode.getNodeName(exp_rnode) + " != " + tempZeroName + " GOTO " + label1);
		}
		intercode.Add_Code("GOTO " + label2);
		intercode.Add_Code("LABEL " + label1 + " :");
		statement_parse(node->left->right->right->right->right);
		intercode.Add_Code("LABEL " + label2 + " :");

		CodeBlockStack.pop_back();
	}
	else {
		CodeBlock newCodeBlock1;
		CodeBlockStack.push_back(newCodeBlock1);

		VarNode exp_rnode = expr_parse(node->left->right->right);
		string label1 = intercode.getLabelName();
		string label2 = intercode.getLabelName();
		string label3 = intercode.getLabelName();
		if (exp_rnode.type == "bool") {
			intercode.Add_Code("IF " + exp_rnode.CondString + " GOTO " + label1);
		}
		else {
			string tempZeroName = "temp" + inttostr(intercode.tempNum);
			intercode.tempNum++;
			VarNode newZeroNode = createTempVar(tempZeroName, "int");
			intercode.Add_Code(tempZeroName + " := #0");
			intercode.Add_Code("IF " + intercode.getNodeName(exp_rnode) + " != " + tempZeroName + " GOTO " + label1);
		}
		intercode.Add_Code("GOTO " + label2);
		intercode.Add_Code("LABEL " + label1 + " :");
		statement_parse(node->left->right->right->right->right);
		intercode.Add_Code("GOTO " + label3);

		CodeBlockStack.pop_back();
		intercode.Add_Code("LABEL " + label2 + " :");

		CodeBlock newCodeBlock2;
		CodeBlockStack.push_back(newCodeBlock2);
		statement_parse(node->left->right->right->right->right->right->right);
		intercode.Add_Code("LABEL " + label3 + " :");

		CodeBlockStack.pop_back();
	}
}

void SemanticParser::loop_statement_parse(struct ParseTree *node)
{
	if (node->left->name == "WHILE") {
		CodeBlock newCodeBlock;
		newCodeBlock.isbreak = true;
		CodeBlockStack.push_back(newCodeBlock);
		string label1 = intercode.getLabelName();
		string label2 = intercode.getLabelName();
		string label3 = intercode.getLabelName();
		CodeBlockStack.back().breaklabel = label3;
		intercode.Add_Code("LABEL " + label1 + " :");
		VarNode exp_node = expr_parse(node->left->right->right);
		if (exp_node.type == "bool") {
			intercode.Add_Code("IF " + exp_node.CondString + " GOTO " + label2);
		}
		else {
			string tempZeroName = "temp" + inttostr(intercode.tempNum);
			intercode.tempNum++;
			VarNode newZeroNode = createTempVar(tempZeroName, "int");
			intercode.Add_Code(tempZeroName + " := #0");
			intercode.Add_Code("IF " + intercode.getNodeName(exp_node) + " != " + tempZeroName + " GOTO " + label2);
		}
		intercode.Add_Code("GOTO " + label3);
		intercode.Add_Code("LABEL " + label2 + " :");
		statement_parse(node->left->right->right->right->right);
		intercode.Add_Code("GOTO " + label1);
		intercode.Add_Code("LABEL " + label3 + " :");
		CodeBlockStack.pop_back();
	}
}

void SemanticParser::return_statement_parse(struct ParseTree *node)
{
	string funType = getFunRetType();
	if (node->left->right->name == "expression") {
		VarNode rnode = expr_parse(node->left->right);
		intercode.Add_Code(intercode.RettoCode(rnode));
		if (rnode.type != funType)
			error(node->left->right->line, "return type not equal to function return type.");
	}
	else {
		intercode.Add_Code("RETURN");
		if (funType != "void")
			error(node->left->right->line, "return should be " + CodeBlockStack.back().func.rtype);
	}
}

VarNode SemanticParser::var_parse(struct ParseTree* node)
{
	if (node->left->name == "ID") {
		if (node->left->right == NULL) {
			string Content = node->left->content;
			VarNode rNode = VarNode_search(Content);
			if (rNode.num < 0) {
				error(node->left->line, "Undefined variable " + Content);
			}
			return rNode;
		}
		else {
			string array_Name = node->left->content;
			ParseTree* exp = node->left->right->right;
			VarNode eNode = expr_parse(exp);
			AryNode aNode = getAryNode(array_Name);
			if (aNode.num < 0) {
				error(node->left->right->line, "Undefined array " + array_Name);
			}

			VarNode tmpVar;
			string tmpName = "temp" + inttostr(intercode.tempNum);

			++intercode.tempNum;
			tmpVar.name = tmpName;
			tmpVar.type = aNode.type;
			tmpVar.isPointer = true;
			CodeBlockStack.back().VarMap.insert({ tmpName,tmpVar });

			VarNode tmpVar2;
			string tmpName2 = "temp" + inttostr(intercode.tempNum);
			++intercode.tempNum;
			tmpVar2.name = tmpName2;
			tmpVar2.type = "int";
			CodeBlockStack.back().VarMap.insert({ tmpName2,tmpVar2 });

			VarNode tmpVar3;
			string tmpName3 = "temp" + inttostr(intercode.tempNum);
			++intercode.tempNum;
			tmpVar3.name = tmpName3;
			tmpVar3.type = "int";
			CodeBlockStack.back().VarMap.insert({ tmpName3,tmpVar3 });

			intercode.Add_Code(tmpName3 + " := #4");
			intercode.Add_Code(tmpName2 + " := " + intercode.getNodeName(eNode) + " * " + tmpName3);
			intercode.Add_Code(tmpName + " := &" + intercode.getAryNodeName(aNode) + " + " + intercode.getNodeName(tmpVar2));

			return tmpVar;
		}
	}
}

VarNode SemanticParser::expr_parse(struct ParseTree *node)
{
	if (node->left->name == "var") {
		struct ParseTree *variable = node->left;
		struct ParseTree *express = node->left->right->right;
		VarNode Node1 = var_parse(variable);
		VarNode Node2 = expr_parse(express);
		
		if (Node1.type != Node2.type) {
			error(node->left->right->line, "assignment error");
		}			

		intercode.Add_Code(intercode.AssigntoCode(Node1, Node2));
		return Node1;
	}
	else if (node->left->name == "simple_expression") {
		return simple_expr_parse(node->left);
	}
}

VarNode SemanticParser::simple_expr_parse(struct ParseTree *node)
{
	if (node->left->right == NULL) {
		return additive_expr_parse(node->left);
	}
	else {
		string operation = relop_parse(node->left->right);

		VarNode Node1 = additive_expr_parse(node->left);
		VarNode Node2 = additive_expr_parse(node->left->right->right);
		if (Node1.type != Node2.type)
			error(node->left->right->line, "different type for two variables.");
			
		string tmpName = "temp" + inttostr(intercode.tempNum);
		++intercode.tempNum;

		VarNode newNode = createTempVar(tmpName, "bool");
		CodeBlockStack.back().VarMap.insert({ tmpName,newNode });
		intercode.Add_Code(intercode.VartoCode(tmpName, operation, Node1, Node2));

		newNode.CondString = intercode.getNodeName(Node1) + " " + operation + " " + intercode.getNodeName(Node2);
		return newNode;
	}
}

VarNode SemanticParser::additive_expr_parse(struct ParseTree* node)
{
	if (node->left->name == "additive_expression") {
		VarNode Node1 = additive_expr_parse(node->left);
		string operation = addop_parse(node->left->right);
		VarNode Node2 = term_parse(node->left->right->right);
		if (Node1.type != Node2.type) {
			error(node->left->right->left->line, "different type for two variables.");
		}

		string tmpName = "temp" + inttostr(intercode.tempNum);
		++intercode.tempNum;
		VarNode newNode = createTempVar(tmpName, Node1.type);
		CodeBlockStack.back().VarMap.insert({ tmpName,newNode });

		intercode.Add_Code(intercode.VartoCode(tmpName, operation, Node1, Node2));
		return newNode;
	}
	else {
		return term_parse(node->left);
	}
}

string SemanticParser::relop_parse(struct ParseTree *node)
{
	return node->content;
}

string SemanticParser::addop_parse(struct ParseTree *node)
{
	return node->content;
}

string SemanticParser::mulop_parse(struct ParseTree* node)
{
	return node->content;
}

VarNode SemanticParser::term_parse(struct ParseTree *node)
{
	if (node->left->name == "term") {
		VarNode Node1 = term_parse(node->left);
		string operation = mulop_parse(node->left->right);
		VarNode Node2 = factor_parse(node->left->right->right);

		if (Node1.type != Node2.type) {
			error(node->left->right->left->line, "different type for two variables.");
		}	

		string tmpName = "temp" + inttostr(intercode.tempNum);
		++intercode.tempNum;
		VarNode newNode = createTempVar(tmpName, Node1.type);
		CodeBlockStack.back().VarMap.insert({ tmpName,newNode });

		intercode.Add_Code(intercode.VartoCode(tmpName, operation, Node1, Node2));
		return newNode;
	}
	else {
		return factor_parse(node->left);
	}
}

VarNode SemanticParser::factor_parse(struct ParseTree *node)
{
	if (node->left->name == "var") {
	return var_parse(node->left);
	}
	else if (node->left->name == "(") {
		return expr_parse(node->left->right);
	}
	else if (node->left->name == "call") {
		return call_parse(node->left);
	}
	else {
		string tmpName = "temp" + inttostr(intercode.tempNum);
		VarNode newNode = createTempVar(tmpName, "int");
		string content = node->left->content;

		++intercode.tempNum;
				
		CodeBlockStack.back().VarMap.insert({ tmpName,newNode });
		intercode.Add_Code(tmpName + " := #" + content);
		
		return newNode;
	}
}

VarNode SemanticParser::call_parse(struct ParseTree *node)
{
	VarNode newNode;
	string functionName = node->left->content;
	
	if (funcPool.find(functionName) == funcPool.end()) {
		error(node->left->line, "undefined function " + functionName);
	}

	args_parse(node->left->right->right, functionName);

	FunNode function = funcPool[functionName];
	if (function.rtype == "void") {
		intercode.Add_Code("CALL " + functionName);
		newNode.type = "void";
	}
	else {
		string tmpName = "temp" + inttostr(intercode.tempNum);
		++intercode.tempNum;
		newNode = createTempVar(tmpName, funcPool[functionName].rtype);
		intercode.Add_Code(tmpName + " := CALL " + functionName);
	}
	return newNode;
}

void SemanticParser::arg_list_parse(struct ParseTree* node, string funcName)
{
	ParseTree* tmp = node->left;
	FunNode function = funcPool[funcName];
	int i = 0;

	while (tmp->name == "arg_list")
	{
		VarNode rNode = expr_parse(tmp->right->right);
		intercode.Add_Code(intercode.ArgtoCode(rNode));
		tmp = tmp->left;
		i++;
		if (function.paramlist[function.paramlist.size() - i].type != rNode.type)
			error(tmp->line, "wrong type arguments in function " + funcName);	
	}
	VarNode rNode = expr_parse(tmp);

	intercode.Add_Code(intercode.ArgtoCode(rNode));
	if (function.paramlist.size() == 0 && rNode.name != "void") {
		error(tmp->line, "wrong type arguments in function " + funcName);
	}
	++i;

	if (function.paramlist[function.paramlist.size() - i].type != rNode.type) {
		error(tmp->line, "wrong type arguments in function " + funcName);
	}
	if (i != function.paramlist.size()) {
		error(tmp->line, "arguments numbers not equal to the function parameters number.");
	}
}

void SemanticParser::args_parse(struct ParseTree *node, string funcName)
{
	if (node->left != NULL) {
		arg_list_parse(node->left, funcName);
	}
	else if (funcPool[funcName].no_param != true) {
		error(node->line, "wrong type arguments in function " + funcName);
	}
}

struct VarNode SemanticParser::VarNode_search(string name)
{
	int N = CodeBlockStack.size();
	for (int i = N - 1; i >= 0; --i)
	{
		if (CodeBlockStack[i].VarMap.find(name) != CodeBlockStack[i].VarMap.end()) 
			return CodeBlockStack[i].VarMap[name];
	}
	VarNode tmp;
	tmp.num = -1;
	return tmp;
}

struct AryNode SemanticParser::getAryNode(string name)
{
	int N = CodeBlockStack.size();
	for (int i = N - 1; i >= 0; --i)
	{
		if (CodeBlockStack[i].AryMap.find(name) != CodeBlockStack[i].AryMap.end())
			return CodeBlockStack[i].AryMap[name];
	}
	AryNode tmp;
	tmp.num = -1;
	return tmp;
}

string SemanticParser::getFunRetType()
{
	int N = CodeBlockStack.size();
	for (int i = N - 1; i >= 0; --i)
	{
		if (CodeBlockStack[i].isfunc)
			return CodeBlockStack[i].func.rtype;
	}
	return "";
}

bool SemanticParser::var_search(string name)
{
	return CodeBlockStack.back().VarMap.find(name) != CodeBlockStack.back().VarMap.end();
}

void SemanticParser::error(int line, string error)
{
	print_code();

	cout << "error encountered in line " << line << ": ";
	cout << error << endl;

	exit(1);
}

void SemanticParser::print_code() {
	intercode.Print_Code();
}

void SemanticParser::genDot(ParseTree *node)
{
	ofstream out("SyntaxTree.dot");
	queue<ParseTree *> que;
	ParseTree *n, *p;

	if (node == NULL)
		return;

	que.push(node);

	out << "digraph G {" << endl;
	out << "node[shape=rect]" << endl;

	out << "_" << &(node->name) << "[label=" << node->name << "]" << endl;

	while (!que.empty())
	{
		n = que.front();
		if (n->left == NULL)
		{
			que.pop();
			continue;
		}
		out << "_" << &(n->name) << " -> _" << &(n->left->name) << endl;

		que.push(n->left);
		out << "_" << &(n->left->name) << "[label=\"" << n->left->name << "\"]" << endl;
	
		p = n->left->right;
		while (p != NULL)
		{			
			out << "_" << &(n->name) << " -> _" << &(p->name) << endl;
			que.push(p);			
			out << "_" << &(p->name) << "[label=\"" << p->name << "\"]" << endl;
			p = p->right;
		}
		que.pop();
	}

	out << "}";
	out.close();
}

struct VarNode SemanticParser::createTempVar(string name, string type)
{
	VarNode variable;
	variable.num = -1;
	variable.name = name;
	variable.type = type;
	return variable;
}
