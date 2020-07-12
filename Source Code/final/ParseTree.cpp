#include"ParseTree.h"

string inttostr(int n) {
	char buf[10];
	sprintf(buf, "%d", n);
	string b = buf;
	return b;
}

int strtoint(string s) {
	int in;
	in = atoi(s.c_str());
	return in;
}


ParseTree* Create_Parse_Tree(string name, int num, ...) {
	va_list valist;
	ParseTree *head = new ParseTree();
	if (!head) {
		printf("Out of Memory\n");
		exit(0);
	}
	head->left = NULL;
	head->right = NULL;
	head->name = name;
	head->content = "";

	ParseTree *temp = NULL;
	va_start(valist, num);
	if (num > 0) {
		temp = va_arg(valist, ParseTree*);
		head->left = temp;
		head->line = temp->line;
		if (num == 1) {
			if (temp->content.size() > 0) {
				head->content = temp->content;
			}
			else head->content = "";
		}
		else {
			for (int i = 1; i < num; i++) {
				temp->right = va_arg(valist, ParseTree*);
				temp = temp->right;
			}
		}
	}
	else {
		int line = va_arg(valist, int);
		head->line = line;
		if (head->name == "NUM") {
			int value = atoi(yytext);
			head->content = inttostr(value);
		}
		else head->content = yytext;
	}
	return head;
}

void eval(ParseTree *head, int level) {
	if (head != NULL) {
		string Name = head->name;
		if (head->line != -1) {
			for (int i = 0; i < level; i++) {
				cout << " ";
			}
			cout << head->name;
			
			if (head->name == "ID") {
				cout << ":" << head->content;
			}
			else if (head->name == "NUM") {
				cout << ":" << head->content << " ";
			}
			else {
				cout << " [" << head->line << "]";
			}
			cout << endl;
		}
		eval(head->left, level + 1);
		eval(head->right, level);
	}
}

void Free_Parse_Tree(ParseTree *node) {
	if (node == NULL) return;
	Free_Parse_Tree(node->left);
	delete node;
	Free_Parse_Tree(node->right);
}
