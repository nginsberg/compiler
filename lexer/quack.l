%{
    #include <iostream>
    #include <string>
    #include "toknames.h"
    using namespace std;
    #define YY_DECL extern "C" int yylex()
    string str;
    void printError(string err);
%}
%option yylineno
%x COMMENT
%x STRING
%%
class                   { return CLASS; }
def                     { return DEF; }
extends                 { return EXTENDS; }
if                      { return IF; }
elif                    { return ELIF; }
else                    { return ELSE; }
while                   { return WHILE; }
return                  { return RETURN; }
[{]                     { return '{'; }
[}]                     { return '}'; }
[(]                     { return '('; }
[)]                     { return ')'; }
[[]                     { return '['; }
[]]                     { return ']'; }
[:]                     { return ':'; }
[,]                     { return ','; }
[=]                     { return '='; }
[;]                     { return ';'; }
[.]                     { return '.'; }
[+]                     { return '+'; }
[-]                     { return '-'; }
[*]                     { return '*'; }
[/]                     { return '/'; }
[<]                     { return '<'; }
[>]                     { return '>'; }
[=][=]                  { return EQUALS; }
[<][=]                  { return ATMOST; }
[>][=]                  { return ATLEAST; }
and                     { return AND; }
or                      { return OR; }
not                     { return NOT; }
[0-9]*                  { return INT_LIT; }
[a-zA-Z_][a-zA-Z0-9_]*  { return IDENT; }
[/][/].*                { ; }
[ \t]                   { ; }
<<EOF>>                 { return EOF; }

[/][*]                  { BEGIN(COMMENT); }
<COMMENT>[*][/]         { BEGIN(INITIAL); }
<COMMENT>.              { ; }

["]["]["]               {
    str = "";
    BEGIN(STRING);
}
<STRING>.|\n               { str += yytext; }
<STRING>["]["]["]       {
    yytext = (char *)str.c_str();
    BEGIN(INITIAL);
    return STRING_LIT;
}

<COMMENT,INITIAL>\n {}
["](([\\][0btnrf"\\])|[^"\\\n])*["] { return STRING_LIT; }
["](([\\].)|[^"\\\n])*\n  {
    printError(":  Unclosed string?  Encountered newline in quoted string.");
    return STRING_LIT;
}
["](([\\].)|[^"\\\n])*["] {
    printError("Illegal escape code; only \\\\, \\0, \\t, \\n, \\r are permitted");
    return STRING_LIT;
}
%%
void printError(string err) {
    cerr << yylineno << ": " << err << " ( at '" << yytext << "')" << endl;
}
