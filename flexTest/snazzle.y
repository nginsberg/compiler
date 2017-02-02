%{
#include <cstdio>
#include <iostream>
using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;

void yyerror(const char *s);
%}

%union {
    int ival;
    float fval;
    char *sval;
}

%token SNAZZLE TYPE
%token END


%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%%
snazzle:
    header template body_section footer { cout << "done with a snazzle file!" << endl; }
    ;
header:
    SNAZZLE FLOAT { cout << "reading a snazzle file version " << $2 << endl; }
    ;
template:
    typelines
    ;
typelines:
    typelines typeline
    | typeline
    ;
typeline:
    TYPE STRING { cout << "new defined snazzle type: " << $2 << endl; }
    ;
body_section:
    body_lines
    ;
body_lines:
    body_lines body_line
    | body_line
    ;
body_line:
    INT INT INT INT STRING { cout << "new snazzle: " << $1 << $2 << $3 << $4 << $5 << endl; }
    ;
footer:
    END
    ;
%%

int main(int, char**) {
    // open a file handle to a particular file:
    FILE *myfile = fopen("in.snazzle", "r");
    // make sure it's valid:
    if (!myfile) {
        cout << "I can't open a.snazzle.file!" << endl;
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
