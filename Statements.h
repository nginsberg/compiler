#ifndef STATEMENTS_H_
#define STATEMENTS_H_

#include <iostream>
#include <string>
#include <list>
#include <map>

#include "gc.h"
#include "Expressions.h"

class FormalArgs;

class Scope {
public:
    std::map<std::string, std::string> tokens;
    void addFormalArgs(const FormalArgs &fArgs);
    void addReturn();
    void print() const;
};

class Statement {
public:
    Statement(int l): line(l) {}
    int line;
    virtual std::string print() { return ""; }
};

class Statements {
public:
    std::list<Statement *> ss;
    std::string print(int tabs = 1) const;
    std::string node(std::string lblName) const;

    Scope scope;
};

class BareStatement : public Statement {
public:
    BareStatement(int l, RExpr *e): Statement(l), expr(e) {}
    std::string print() override;
    RExpr *expr;
};

class AssignStatement : public Statement {
public:
    AssignStatement(int l, LExpr *t, RExpr *f): Statement(l), to(t), from(f) {}
    std::string print() override;
    LExpr *to;
    RExpr *from;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(int l, RExpr *r): Statement(l), ret(r) {}
    std::string print() override;
    RExpr *ret;
};

class WhileStatement : public Statement {
public:
    WhileStatement(int l, RExpr *it, Statements bl): Statement(l), ifTrue(it),
        block(bl) {}
    std::string print() override;
    RExpr *ifTrue;
    Statements block;
};

class ElseStatement : public Statement {
public:
    ElseStatement(int l, Statements s): Statement(l), ss(s) {}
    std::string print() override;
    Statements ss;
};

class Conditional : public Statement {
public:
    Conditional(int l): Statement(l) {}
    void add(const Conditional &other);
    std::list<RExpr *> conditionals;
    std::list<Statements> blocks;
};

#endif










