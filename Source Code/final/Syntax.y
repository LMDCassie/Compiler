%{

#include <cstdlib>
#include <cstdio>
#include <string>
#include "ParseTree.h"
#include "Semantic.h"
using namespace std;

ParseTree *root;
extern char *yytext;
extern int column;
extern FILE * yyin;
extern FILE * yyout;
extern int yylineno;

int yylex(void);
void yyerror(const char*); 

%}

%union 
{ 
    struct ParseTree* gt;
};

%token<gt> INT VOID IF ELSE WHILE RETURN ID NUM ';' '[' ']' '(' ')' ',' '{' '}' '=' RELOP ADDOP MULOP

%type<gt> program declaration_list declaration var_declaration type_specifier fun_declaration params param_list param compound_stmt local_declarations statement_list statement expression_stmt selection_stmt iteration_stmt return_stmt expression var simple_expression additive_expression term factor call args arg_list

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
program: 
	declaration_list{
		root =Create_Parse_Tree("program", 1, $1);
	}
	;

declaration_list:
	declaration_list declaration{
		$$ =Create_Parse_Tree("declaration_list", 2, $1, $2);
	}
	|
	declaration{
		$$ =Create_Parse_Tree("declaration_list", 1, $1);
	}
	;

declaration:
	var_declaration{
		$$ =Create_Parse_Tree("declaration", 1, $1);
	}
	|
	fun_declaration{
		$$ =Create_Parse_Tree("declaration", 1, $1);
	}
	;

var_declaration:
	type_specifier ID ';'{
		//add_var($1, $2);
		$$ =Create_Parse_Tree("var_declaration", 3, $1, $2, $3);
	}
	|
	type_specifier ID '[' NUM ']' ';'{
		//add_var($1, $2, $4);
		$$ =Create_Parse_Tree("var_declaration", 6, $1, $2, $3, $4, $5, $6);
	}
	;

type_specifier:
	INT{
		//$$.dtype = $1.dtype;
		$$ =Create_Parse_Tree("type_specifier", 1, $1);
	}
	|
	VOID{
		//$$.dtype = $1.dtype;
		$$ =Create_Parse_Tree("type_specifier", 1, $1);
	}
	;


fun_declaration:
	type_specifier ID /*for_func */'(' params ')' compound_stmt{
		//add_func($1, $2, $5);
		//point = delete_table();
		$$ =Create_Parse_Tree("fun_declaration", 6, $1, $2, $3, $4, $5, $6);
	}
	;

params:
	param_list{
		$$ =Create_Parse_Tree("params", 1, $1);
	}
	|
	VOID{
		$$ =Create_Parse_Tree("params", 1, $1);
	}
	;

param_list:
	param_list ',' param{
		$$ =Create_Parse_Tree("param_list", 3, $1, $2, $3);
	}
	|
	param{
		$$ =Create_Parse_Tree("param_list", 1, $1);
	}
	;

param:
	type_specifier ID{
		$$ =Create_Parse_Tree("param", 2, $1, $2);
	}
	|
	type_specifier ID '[' ']'{
		$$ =Create_Parse_Tree("param", 4, $1, $2, $3, $4);
	}
	;

compound_stmt:
	'{' local_declarations statement_list '}'{
		$$ =Create_Parse_Tree("compound_stmt", 4, $1, $2, $3, $4);
	}
	;

local_declarations:
	local_declarations var_declaration{
		$$ =Create_Parse_Tree("local_declarations", 2, $1, $2);
	}
	|
	{
		$$ =Create_Parse_Tree("local_declarations", 0, 0);
	}
	;

statement_list:
	statement_list statement{
		$$ =Create_Parse_Tree("statement_list", 2, $1, $2);
	}
	|
	{
		$$ =Create_Parse_Tree("statement_list", 0, 0);
	}
	;

statement:
	expression_stmt{
		$$ =Create_Parse_Tree("statement", 1, $1);
	}
	|
	compound_stmt{
		$$ =Create_Parse_Tree("statement", 1, $1);
	}
	|
	selection_stmt{
		$$ =Create_Parse_Tree("statement", 1, $1);
	}
	|
	iteration_stmt{
		$$ =Create_Parse_Tree("statement", 1, $1);
	}
	|
	return_stmt{
		$$ =Create_Parse_Tree("statement", 1, $1);
	}
	;

expression_stmt:
	expression ';'{
		$$ =Create_Parse_Tree("expression_stmt", 2, $1, $2);
	}
	|
	';'{
		$$ =Create_Parse_Tree("expression_stmt", 1, $1);
	}
	;
	
selection_stmt:
	IF '(' expression ')' statement %prec LOWER_THAN_ELSE {
		$$ =Create_Parse_Tree("selection_stmt", 5, $1, $2, $3, $4, $5);
	}
   	| 
	IF '(' expression ')' statement ELSE statement {
		$$ =Create_Parse_Tree("selection_stmt", 7, $1, $2, $3, $4, $5, $6, $7);
	}
    	;

iteration_stmt:
	WHILE '(' expression ')' statement{
		$$ =Create_Parse_Tree("iteration_stmt", 5, $1, $2, $3, $4, $5);
	}
	;

return_stmt:
	RETURN ';'{
		$$ =Create_Parse_Tree("return_stmt", 2, $1, $2);
	}
	|
	RETURN expression ';'{
		$$ =Create_Parse_Tree("return_stmt", 3, $1, $2, $3);
	}
	;

expression:
	var '=' expression{
		//$1.dtype = $3.dtype;
		//$$.dtype = $1.dtype;
		$$ =Create_Parse_Tree("expression", 3, $1, $2, $3);
	}
	|
	simple_expression{
		//$$.dtype = $1.dtype;
		$$ =Create_Parse_Tree("expression", 1, $1);
	}
	;

var:
	ID{
		$$ =Create_Parse_Tree("var", 1, $1);
	}
	|
	ID '[' expression ']'{
		$$ =Create_Parse_Tree("var", 4, $1, $2, $3, $4);
	}
	;

simple_expression:
	additive_expression RELOP additive_expression{
		$$ = Create_Parse_Tree("simple_expression", 3, $1, $2, $3);
	}
	|
	additive_expression{
		//$$.dtype = $1.dtype;
		$$ = Create_Parse_Tree("simple_expression", 1, $1);
	}
	;

additive_expression:
	additive_expression ADDOP term{
		/*if($1.dtype == $3.dtype == "int"){
			$$ = $1.dtype;
		}*/
		$$ = Create_Parse_Tree("additive_expression", 3, $1, $2, $3);
	}
	|
	term{
		//$$.dtype = $1.dtype;
		$$ = Create_Parse_Tree("additive_expression", 1, $1);
	}
	;

term:
	term MULOP factor{
		/*if($1.dtype == $3.dtype == "int"){
			$$ = $1.dtype;
		}*/
		$$ = Create_Parse_Tree("term", 3, $1, $2, $3);
	}
	|
	factor{
		//$$.dtype = $1.dtype;
		$$ = Create_Parse_Tree("term", 1, $1);
	}
	;

factor:
	'(' expression ')'{
		//$$.dtype = $2.dtype;
		$$ = Create_Parse_Tree("factor", 3, $1, $2, $3);
	}
	|
	var{
		//$$.dtype = $1.dtype;
		$$ = Create_Parse_Tree("factor", 1, $1);
	}
	|
	call{
		//$$.dtype = $1.dtype;
		$$ = Create_Parse_Tree("factor", 1, $1);
	}
	|
	NUM{
		//$$.dtype = $1.dtype;
		$$ = Create_Parse_Tree("factor", 1, $1);
	}
	;

call:
	ID '(' args ')'{
		/*if(func_exist($1, $3)){
			$$.dtype = func_type($1);
		}*/
		$$ = Create_Parse_Tree("call", 4, $1, $2, $3, $4);
	}
	;

args:
	arg_list{
		$$ = Create_Parse_Tree("args", 1, $1);
	}
	|
	{
		$$ = Create_Parse_Tree("args", 0, 0);
	}
	;

arg_list:
	arg_list ',' expression{
		$$ = Create_Parse_Tree("arg_list", 3, $1, $2, $3);
	}
	|
	expression{
		$$ = Create_Parse_Tree("arg_list", 1, $1);
	}
	;
%%

void yyerror(char const *s){
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

int main(int argc,char* argv[]) {
	yyin = fopen(argv[1],"r");
	printf("begin compile\n");
	yyparse();
	printf("end compile\n");
	eval(root,0);
	SemanticParser parser(root);
	Free_Parse_Tree(root);
	fclose(yyin);
	return 0;
}
