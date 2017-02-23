#include <iostream>
#include <string>
#include <algorithm>

#include "Statements.h"

using namespace std;

string Statements::print(int tabs) {
    string ret = "";
    for_each(ss.begin(), ss.end(), [&] (Statement *s) {
        ret += string(tabs, '\t') + s->print() + "\n";
    });
    return ret;
}

string BareStatement::print() {
    string ret = to_string(line) + ": " + expr.str;
    return ret;
}

string AssignStatement::print() {
    string ret = to_string(line) + ": " + to.str + " <- " + from.str;
    return ret;
}

string ReturnStatement::print() {
    string ret = to_string(line) + ": RET -> " + this->ret.str;
    return ret;
}

string WhileStatement::print() {
    string ret = to_string(line) + ": WHILE (" + ifTrue.str + ") ->\n";
    ret += block.print();
    return ret;
}

string ElseStatement::print() {
    string ret = to_string(line) + ": ELSE ->\n";
    ret += ss.print();
    return ret;
}

string ElifStatement::print() {
    string ret = to_string(line) + ": ELIF (" + ifTrue.str + ") ->\n";
    ret += ss.print();
    return ret;
}

string Elifs::print() {
    string ret;
    for_each(elifs.begin(), elifs.end(), [&] (ElifStatement s) {
        ret += s.print() + "\n";
    });
    return ret;
}

string IfStatement::print() {
    string ret = to_string(line) + ": IF (" + ifTrue.str + ") ->\n";
    ret += stmts.print();
    ret += elifs.print();
    ret += el.print();
    return ret;
}
