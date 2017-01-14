%{
    #include <iostream>
    #include "toknames.h"
    using namespace std;
    #define YY_DECL extern "C" int yylex()
%}
%%
class    { return CLASS; }
def      { return DEF; }
extends  { return EXTENDS; }
if       { return IF; }
elif     { return ELIF; }
else     { return ELSE; }
while    { return WHILE; }
return   { return RETURN; }

{        { return '{'; }
}        { return '}'; }
\(       { return '('; }
\)       { return ')'; }
\[       { return '['; }
\]       { return ']'; }

[ \t]    { ; }
<<EOF>>  { return EOF; }

[\n]    { ++yylineno; } // Keep track of line number
%%
