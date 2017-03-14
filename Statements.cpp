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

void Conditional::add(const Conditional &other) {
    auto cond = other.conditionals.begin();
    auto block = other.blocks.begin();

    while (cond != other.conditionals.end()) {
        conditionals.push_back(*cond);
        blocks.push_back(*block);
        ++cond;
        ++block;
    }
}

void Scope::addReturn() {
    if (tokens.find("$return") == tokens.end()) {
        tokens["$return"] = "Nothing";
    }
}
