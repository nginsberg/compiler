%{
    #include <cstdio>
    #include <iostream>
    using namespace std;

    extern "C" int yylex();
    extern "C" int yyparse();
    extern "C" FILE *yyin;
    extern "C" int yylineno;

    void yyerror(const char *s);
%}

%union {
    int ival;
    char *sval;
}

%define parse.error verbose
%left '+' '-'
%left '*' '/'

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
// Top level rule
program:
    classes statements { cout << "Finished parse with no errors" << endl; }
    ;

// Classes
classes:
    %empty
    | classes class
    | class
    ;
class:
    class_signature class_body
    ;
class_signature:
    CLASS IDENT '(' formal_args ')'
    | CLASS IDENT '(' formal_args ')' EXTENDS IDENT
    ;
class_body:
    '{' statements methods '}'
    ;

// Methods
methods:
    %empty
    | methods method
    | method
    ;
method:
    DEF IDENT '(' formal_args ')' return statement_block
    ;
formal_args:
    formal_args ',' formal_arg
    | formal_arg
    ;
formal_arg:
    %empty
    | IDENT ':' IDENT
    ;
return:
    %empty
    | ':' IDENT

// Statements
statements:
    statements statement
    | statement
    ;
statement_block:
    '{' statements '}'
    ;

// Control structures
statement:
    IF r_expr statement_block
    elifs
    else
elifs:
    %empty
    | elifs elif
    | elif
    ;
elif:
    ELIF r_expr statement_block
    ;
else:
    %empty
    | ELSE statement_block
    ;

statement:
    WHILE r_expr statement_block;

// Return
statement:
    RETURN optional_r_expr ';'
    ;
optional_r_expr:
    %empty
    | r_expr
    ;

// Assignment
statement:
    l_expr return '=' r_expr ';'
    ;
l_expr:
    IDENT
    | r_expr '.' IDENT
    ;

// Bare Expressions
statement:
    r_expr ';'
    ;

// Empty
statement:
    %empty
    ;

// Expressions
r_expr:
    STRING_LIT
    | INT_LIT
    | l_expr
    | r_expr '+' r_expr
    | r_expr '-' r_expr
    | r_expr '*' r_expr
    | r_expr '/' r_expr
    | '-' r_expr
    | '(' r_expr ')'
    | r_expr EQUALS r_expr
    | r_expr ATMOST r_expr
    | r_expr '<' r_expr
    | r_expr ATLEAST r_expr
    | r_expr '>' r_expr
    | r_expr AND r_expr
    | r_expr OR r_expr
    | NOT r_expr
    | r_expr '.' IDENT '(' actual_args ')'
    | IDENT '(' actual_args ')'
    ;
actual_args:
    %empty
    | actual_args ',' r_expr
    | r_expr
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
        cout << yylineno << ": EEK, parse error! Message: " << s << endl;
        // might as well halt now:
        exit(-1);
}
