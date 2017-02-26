#ifndef EXPRESSIONS_H_
#define EXPRESSIONS_H_

#include <string>
#include <iostream>
#include <algorithm>
#include <list>

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

class LExpr : public RExpr {
public:
    LExpr(int l, RExpr *e, std::string i): RExpr("", l), expr(e), ident(i) {}
    LExpr(int l, std::string i): RExpr("", l), expr(NULL), ident(i) {}
    std::string print() override;

    RExpr *expr;
    std::string ident;
};

class ActualArgs {
public:
    std::list<RExpr *>args;
    std::string print();
};

class ConstructorCall : public RExpr {
public:
    ConstructorCall(int l, std::string name, ActualArgs a): RExpr("", l), \
        className(name), args(a) {}
    std::string print() override;

    std::string className;
    ActualArgs args;
};

#endif
