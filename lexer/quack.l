%{
    #include <iostream>
    #include "toknames.h"
    using namespace std;
    #define YY_DECL extern "C" int yylex()
%}
%%
[ \t]          ;
[0-9]+         { return INT_LIT; }
[a-zA-Z0-9]+   { return STRING_LIT; }
[\n]           { ++yylineno; }
%%
