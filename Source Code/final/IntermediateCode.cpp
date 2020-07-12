#include "IntermediateCode.h"
#include "string"
#include <fstream>

InterCode::InterCode() {

}

InterCode::~InterCode() {

}

void InterCode::Add_Code(string str) {
	Codes.push_back(str);
}

void InterCode::Print_Code() {
	ofstream out("InterCode.txt");
	for (string s : Codes) {
		out << s << "\n";
	}
}

string InterCode::VartoCode(string tempname, string op, VarNode node1, VarNode node2) {
	string result = tempname + " = ";
	if (node1.isPointer) {
		result += "*" + node1.name;
	}
	else result += node1.name;
	
	result += " " + op + " ";

	if (node2.isPointer) {
		result += "*" + node2.name;
	}
	else result += node2.name;
	
	return result;
}

string InterCode::AssigntoCode(VarNode node1, VarNode node2) {
	string result;
	if (node1.isPointer) {
		result = "*" + node1.name + " = ";
	}
	else {
		result = node1.name;
		result += " = ";
	}

	if (node2.isPointer) {
		result += "*" + node2.name;
	}
	else result += node2.name;
	return result;
}

string InterCode::ParamtoCode(VarNode node) {
	string result = "PARAM ";
	result += node.name;
	return result;
}

string InterCode::RettoCode(VarNode node) {
	string result = "RETURN ";
	if (node.isPointer) {
		result += "*" + node.name;
	}
	else result += node.name;
	return result;
}

string InterCode::ArgtoCode(VarNode node) {
	string result = "ARG ";
	if (node.isPointer) {
		result += "*" + node.name;
	} 
	else result += node.name;
	return result;
}

string InterCode::getNodeName(VarNode node) {
	if (node.isPointer) {
		return "*" + node.name;
	}
	else return node.name;
}

string InterCode::getAryNodeName(AryNode node) {
	return node.name;
}

string InterCode::getLabelName() {
	return "label" + inttostr(labelNum++);
}