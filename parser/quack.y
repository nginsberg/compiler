%{
    #include <cstdio>
    #include <iostream>
    using namespace std;

    extern "C" int yylex();
    extern "C" int yyparse();
    extern "C" FILE *yyin;

    void yyerror(const char *s);
%}

%union {
    int ival;
    char *sval;
}

%define parse.error verbose

%token CLASS
%token DEF
%token EXTENDS
%token IF
%token ELIF
%token ELSE
%token WHILE
%token RETURN
%token ATLEAST
%token ATMOST
%token EQUALS
%token AND
%token OR
%token NOT

%token <sval> IDENT
%token <ival> INT_LIT
%token <sval> STRING_LIT

%%
program:
    classes { cout << "done with a quack file!" << endl; }
    ; // statements;
classes:
    classes class
    | class
    ;
//statements: statements statement | statement;
class:
    class_signature
    ; // class_body;
class_signature:
    CLASS IDENT '(' formal_args ')'
    | CLASS IDENT '(' formal_args ')' EXTENDS IDENT
    ;
formal_args:
    formal_args ',' formal_arg
    | formal_arg
    ;
formal_arg:
    %empty
    | IDENT ':' IDENT
    ;
%%

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Usage: " << argv[0] << " filename.qk" << endl;
        exit(-1);
    }
    // open a file handle to a particular file:
    FILE *myfile = fopen(argv[1], "r");
    // make sure it's valid:
    if (!myfile) {
        cout << "I can't open " << argv[1] << endl;
        return -1;
    }
    // set lex to read from it instead of defaulting to STDIN:
    yyin = myfile;

    // lex through the input:
    do {
        yyparse();
    } while (!feof(yyin));
}

void yyerror(const char *s) {
        cout << "EEK, parse error! Message: " << s << endl;
        // might as well halt now:
        exit(-1);
}
