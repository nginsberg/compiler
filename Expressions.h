#ifndef EXPRESSIONS_H_
#define EXPRESSIONS_H_

#include <string>
#include <iostream>
#include <algorithm>

#include "gc.h"

class RExpr {
public:
    RExpr(std::string s = "", int l = 0): line(l), str(s) {}
    std::string str;
    int line;
    virtual std::string print() { return ""; }
};

class StringLit : public RExpr {
public:
    StringLit(int l, std::string v): RExpr("", l), val(v) {}
    std::string print() override { return val; }
    std::string val;
};

class IntLit : public RExpr {
public:
    IntLit(int l, int v): RExpr("", l), val(v) {}
    std::string print() override { return std::to_string(val); }
    int val;
};

class LExpr {
public:
    LExpr(std::string s): str(s) {}
    std::string str;
};

std::ostream &operator<<(std::ostream &os, const LExpr &expr);

class ActualArgs {
public:
    ActualArgs(std::string s): str(s) {}
    std::string str;
};

std::ostream &operator<<(std::ostream &os, const ActualArgs &args);

#endif
