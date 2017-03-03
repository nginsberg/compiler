#include <iostream>
#include <string>
#include <algorithm>

#include "Statements.h"

using namespace std;

string Statements::print(int tabs) const {
    string ret = "";
    for_each(ss.begin(), ss.end(), [&] (Statement *s) {
        ret += string(tabs, '\t') + s->print() + "\n";
    });
    return ret;
}

string Statements::node(string lblName) const {
    string ret = lblName + " [shape=record, label=\"{";
    for(auto s = ss.begin(); s != ss.end(); ++s) {
        ret += (*s)->print();
        if (s != --ss.end()) { ret += "|"; }
    }
    ret += "}\"]";
    return ret;
}

string BareStatement::print() {
    string ret = to_string(line) + ": " + expr->print();
    return ret;
}

string AssignStatement::print() {
    string ret = to_string(line) + ": " + to->print() + " GETS " + from->print();
    return ret;
}

string ReturnStatement::print() {
    string ret = to_string(line) + ": RET -> " + this->ret->print();
    return ret;
}

string WhileStatement::print() {
    string ret = to_string(line) + ": WHILE (" + ifTrue->print() + ") ->\n";
    ret += block.print();
    return ret;
}

string ElseStatement::print() {
    string ret = to_string(line) + ": ELSE ->\n";
    ret += ss.print();
    return ret;
}

string ElifStatement::print() {
    string ret = to_string(line) + ": ELIF (" + ifTrue.print() + ") ->\n";
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
    string ret = to_string(line) + ": IF (" + ifTrue.print() + ") ->\n";
    ret += stmts.print();
    ret += elifs.print();
    ret += el.print();
    return ret;
}
