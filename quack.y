%{
    #include <cstdio>
    #include <iostream>
    #include <memory>
    #include <string.h>
    #include <memory>
    #include <vector>
    #include <utility>
    #include <algorithm>

    #include "ClassHierarchy.h"
    #include "AST.h"
    #include "gc.h"
    #include "gc_cpp.h"

    using namespace std;

    extern "C" int yylex();
    extern "C" int yyparse();
    extern "C" FILE *yyin;
    extern "C" int yylineno;

    void yyerror(const char *s);
    extern char *yytext;

    Classes *cls;

    // Clearly this is not how this will be done in the future - this will
    // need to be a tree representing the rest of the grammar, very similar
    // to, if not including, the current class hierarchy tree. I'm going to
    // build that tree once I have a better idea of what it's going to be used
    // for, and for now I'm just going to pull out all the constructor calls
    // and check them against my class hierarchy tree.
    list<pair<string, int>> constructorCalls;
%}

%union {
    int ival;
    char *sval;

    ClassSignature *cs;
    Class *cl;
    Classes *cls;
    ClassBody *cb;

    Method *mthd;
    Methods *mthds;
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


%type <sval>  ident
%type <ival>  INT_LIT
%type <sval>  STRING_LIT

%type <cs>    class_signature
%type <cl>    class
%type <cls>   classes
%type <cb>    class_body

%type <mthd>  method
%type <mthds> methods

%%
// Top level rule
program:
    classes statements
    ;

// Classes
classes:
    %empty {
        $$ = new Classes();
    }
    | classes class {
        Classes *cls = $1;
        cls->classes.push_back(*$2);
        $$ = cls;
        ::cls = cls;
    }
    ;
class:
    class_signature class_body {
        $$ = new Class(*$1, *$2);
    }
    ;
class_signature:
    CLASS ident '(' formal_args ')' {
        $$ = new ClassSignature($2, "Obj", yylineno);
    }
    | CLASS ident '(' formal_args ')' EXTENDS ident {
        $$ = new ClassSignature($2, $7, yylineno);
    }
    ;
class_body:
    '{' statements methods '}' {
        $$ = new ClassBody(*$3);
    }
    ;

// Methods
methods:
    %empty {
        $$ = new Methods();
    }
    | methods method {
        Methods *mthds = $1;
        mthds->methods.push_back(*$2);
        $$ = mthds;
    }
    ;
method:
    DEF ident '(' formal_args ')' return statement_block {
        $$ = new Method($2, yylineno);
    }
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
    | ident '(' actual_args ')' {
        constructorCalls.push_back(pair<string,int>($1, yylineno));
    }
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

    // Create class hierarchy, check for well-formedness
    ClassTreeNode classHierarchy(*cls);
    cout << classHierarchy << endl;
    makeSureTableIsEmpty(*cls); // Everything should be in classHierarchy

    // Check constructor calls
    bool hadBadConstructor = false;
    for_each(constructorCalls.begin(), constructorCalls.end(),
        [&] (pair<string,int> call) {
            bool isValid = classHierarchy.makeSureClassExists(call.first);
            if (!isValid) {
                cerr << "Error: " << call.second << ": Call to constructor for"
                << " class " << call.first << " when " << call.first
                << " was never defined." << endl;
                hadBadConstructor = true;
            }
        });
    if (hadBadConstructor) { exit(-1); }

    cout << "Finished 'compile' with no errors! (Class hierarchy + constructor"
        << " calls)" << endl;
    return 0;
}

void yyerror(const char *s) {
        cout << yylineno << ": Error: " << s << endl;
        // might as well halt now:
        exit(-1);
}
