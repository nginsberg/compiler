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
    #include "Methods.h"
    #include "Expressions.h"
    #include "Statements.h"
    #include "gc.h"

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
    const char *sval;

    ClassSignature *cs;
    Class *cl;
    Classes *cls;
    ClassBody *cb;

    Method *mthd;
    Methods *mthds;
    FormalArg *fArg;
    FormalArgs *fArgs;

    RExpr *rexpr;
    LExpr *lexpr;
    ActualArgs *aArgs;

    Statement *stmt;
    Statements *stmts;
}

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
%type <sval>  string_lit
%type <ival>  int_lit
%type <sval>  return

%type <cs>    class_signature
%type <cl>    class
%type <cls>   classes
%type <cb>    class_body

%type <mthd>  method
%type <mthds> methods
%type <fArg>  formal_arg
%type <fArgs> formal_args

%type <rexpr> r_expr
%type <lexpr> l_expr
%type <aArgs> actual_args

%type <stmt>  statement
%type <rexpr> optional_r_expr
%type <stmts> statements
%type <stmts> statement_block
%type <stmt>  else
%type <stmt>  elif
%type <stmt>  elifs

%%
// Top level rule
program:
    classes statements
    ;

// Classes
classes:
    /* empty */ {
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
        $$ = new ClassSignature($2, *$4, "Obj", yylineno);
    }
    | CLASS ident '(' formal_args ')' EXTENDS ident {
        $$ = new ClassSignature($2, *$4, $7, yylineno);
    }
    ;
class_body:
    '{' statements methods '}' {
        $$ = new ClassBody(*$3, *$2);
    }
    ;

// Methods
methods:
    /* empty */ {
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
        $$ = new Method($2, *$4, $6, *$7, yylineno);
    }
    ;
formal_args:
    /* empty */ {
        $$ = new FormalArgs();
    }
    | formal_arg {
        $$ = new FormalArgs(*$1);
    }
    | formal_args ',' formal_arg {
        FormalArgs *args = $1;
        args->fArgs.push_back(*$3);
        $$ = $1;
    }
    ;
formal_arg:
    ident ':' ident {
        $$ = new FormalArg($1, $3);
    }
    ;
return:
    /* empty */ {
        $$ = "Nothing";
    }
    | ':' ident {
        $$ = $2;
    }

// Statements
statements:
    /* empty */ {
        $$ = new Statements();
    }
    | statements statement {
        Statements *stmts = $1;
        stmts->ss.push_back($2);
        $$ = stmts;
    }
    ;
statement_block:
    '{' statements '}' {
        $$ = $2;
    }
    ;

// Control structures
statement:
    IF r_expr statement_block
    elifs
    else {
        RExpr *ifTrue = dynamic_cast<RExpr *>$2;
        Statements *stmts = dynamic_cast<Statements *>$3;
        Elifs *elifs = dynamic_cast<Elifs *>$4;
        ElseStatement *el = dynamic_cast<ElseStatement *>$5;
        $$ = new IfStatement(yylineno, *ifTrue, *stmts, *elifs, *el);
    }
elifs:
    /* empty */ {
        $$ = new Elifs();
    }
    | elifs elif {
        Elifs *es = dynamic_cast<Elifs *>$1;
        ElifStatement *e = dynamic_cast<ElifStatement *>$2;
        es->elifs.push_back(*e);
        $$ = es;
    }
    ;
elif:
    ELIF r_expr statement_block {
        $$ = new ElifStatement(yylineno, *$2, *$3);
    }
    ;
else:
    /* empty */ {
        $$ = new ElseStatement(0, Statements());
    }
    | ELSE statement_block {
        $$ = new ElseStatement(yylineno, *$2);
    }
    ;

statement:
    WHILE r_expr statement_block {
        $$ = new WhileStatement(yylineno, *$2, *$3);
    }

// Return
statement:
    RETURN optional_r_expr ';' {
        $$ = new ReturnStatement(yylineno, *$2);
    }
    ;
optional_r_expr:
    /* empty */ {
        $$ = new RExpr("Nothing()");
    }
    | r_expr {
        $$ = $1;
    }
    ;

// Assignment
statement:
    l_expr '=' r_expr ';' {
        $$ = new AssignStatement(yylineno, *$1, *$3);
    }
    ;

// Bare Expressions
statement:
    r_expr ';' {
        $$ = new BareStatement(yylineno, *$1);
    }
    ;

// Expressions
l_expr:
    ident {
        $$ = new LExpr($1);
    }
    | r_expr '.' ident {
        $$ = new LExpr($1->str + "." + $3);
    }
    ;

r_expr:
    string_lit {
        $$ = new RExpr($1);
    }
    | int_lit {
        $$ = new RExpr(to_string($1));
    }
    | l_expr {
        $$ = new RExpr($1->str);
    }
    | r_expr '+' r_expr {
        $$ = new RExpr($1->str + " + " + $3->str);
    }
    | r_expr '-' r_expr {
        $$ = new RExpr($1->str + " - " + $3->str);
    }
    | '-' r_expr %prec UNARY {
        $$ = new RExpr("- " + $2->str);
    }
    | r_expr '/' r_expr {
        $$ = new RExpr($1->str + " / " + $3->str);
    }
    | r_expr '*' r_expr {
        $$ = new RExpr($1->str + " * " + $3->str);
    }
    | '(' r_expr ')' {
        $$ = new RExpr("(" + $2->str + ")");
    }
    | r_expr EQUALS r_expr {
        $$ = new RExpr($1->str + " == " + $3->str);
    }
    | r_expr ATMOST r_expr {
        $$ = new RExpr($1->str + " <= " + $3->str);
    }
    | r_expr '<' r_expr {
        $$ = new RExpr($1->str + " < " + $3->str);
    }
    | r_expr ATLEAST r_expr {
        $$ = new RExpr($1->str + " >= " + $3->str);
    }
    | r_expr '>' r_expr {
        $$ = new RExpr($1->str + " > " + $3->str);
    }
    | r_expr AND r_expr {
        $$ = new RExpr($1->str + " AND " + $3->str);
    }
    | r_expr OR r_expr {
        $$ = new RExpr($1->str + " OR " + $3->str);
    }
    | NOT r_expr {
        $$ = new RExpr("NOT " + $2->str);
    }
    | r_expr '.' ident '(' actual_args ')' {
        $$ = new RExpr($1->str + "." + $3 + "(" + $5->str + ")");
    }
    | ident '(' actual_args ')' {
        constructorCalls.push_back(pair<string,int>($1, yylineno));

        string s = $1;
        $$ = new RExpr(s + "(" + $3->str + ")");
    }
    ;
actual_args:
    /* empty */ {
        $$ = new ActualArgs("");
    }
    | actual_args ',' r_expr {
        $$ = new ActualArgs($1->str + ", " + $3->str);
    }
    | r_expr {
        $$ = new ActualArgs($1->str);
    }
    ;

ident: IDENT { $$ = strdup(yytext); }
int_lit: INT_LIT { $$ = atoi(yytext); }
string_lit: STRING_LIT { $$ = strdup(yytext); }
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
