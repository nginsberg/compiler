#include <iostream>
#include <string>
#include <algorithm>

#include "Statements.h"

using namespace std;

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
    for_each(block.ss.begin(), block.ss.end(), [&] (Statement s) {
        ret += "\t" + s.print() + "\n";
    });
    return ret;
}
