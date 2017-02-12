%{
    #include <cstdio>
    #include <iostream>
    #include <memory>
    #include <string.h>
    #include <memory>

    #include "ClassHierarchy.h"
    #include "gc.h"
    #include "gc_cpp.h"

    using namespace std;

    extern "C" int yylex();
    extern "C" int yyparse();
    extern "C" FILE *yyin;
    extern "C" int yylineno;

    void yyerror(const char *s);
    extern char *yytext;
%}

%union {
    int ival;
    char *sval;
    ClassSignature *cs;
}

%define parse.error verbose
%left EQUALS
%left AND OR NOT
%left ATMOST ATLEAST '<' '>'
%left '+' '-'
%left '/' '*'
%left UNARY
%left '.'

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
%token IDENT
%token INT_LIT
%token STRING_LIT


%type <sval> IDENT
%type <sval> ident
%type <ival> INT_LIT
%type <sval> STRING_LIT
%type <cs>   class_signature

%%
// Top level rule
program:
    classes statements { cout << "Finished parse with no errors" << endl; }
    ;

// Classes
classes:
    %empty
    | classes class
    ;
class:
    class_signature class_body
    ;
class_signature:
    CLASS ident '(' formal_args ')' {
        $$ = new ClassSignature($2, "Obj");
        cout << *$$ << endl;
    }
    | CLASS ident '(' formal_args ')' EXTENDS ident {
        $$ = new ClassSignature($2, $7);
        cout << *$$ << endl;
    }
    ;
class_body:
    '{' statements methods '}'
    ;

// Methods
methods:
    %empty
    | methods method
    ;
method:
    DEF ident '(' formal_args ')' return statement_block
    ;
formal_args:
    formal_args ',' formal_arg
    | formal_arg
    ;
formal_arg:
    %empty
    | ident ':' ident
    ;
return:
    %empty
    | ':' ident

// Statements
statements:
    %empty
    | statements statement
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
    ident
    | r_expr '.' ident
    ;

// Bare Expressions
statement:
    r_expr ';'
    ;

// Expressions
r_expr:
    STRING_LIT
    | INT_LIT
    | l_expr
    | r_expr '+' r_expr
    | r_expr '-' r_expr
    | '-' r_expr %prec UNARY
    | r_expr '/' r_expr
    | r_expr '*' r_expr
    | '(' r_expr ')'
    | r_expr EQUALS r_expr
    | r_expr ATMOST r_expr
    | r_expr '<' r_expr
    | r_expr ATLEAST r_expr
    | r_expr '>' r_expr
    | r_expr AND r_expr
    | r_expr OR r_expr
    | NOT r_expr
    | r_expr '.' ident '(' actual_args ')'
    | ident '(' actual_args ')'
    ;
actual_args:
    %empty
    | actual_args ',' r_expr
    | r_expr
    ;

ident: IDENT { $$ = strdup(yytext); }
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
