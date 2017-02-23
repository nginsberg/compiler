#ifndef STATEMENTS_H_
#define STATEMENTS_H_

#include <iostream>
#include <string>

#include "gc.h"
#include "gc_cpp.h"
#include "Expressions.h"

class Statement : public gc {
public:
    Statement(int l): line(l) {}
    int line;
    virtual std::string print() { return ""; }
};

class BareStatement : public Statement {
public:
    BareStatement(int l, RExpr e): Statement(l), expr(e) {}
    std::string print() override;
    RExpr expr;
};

#endif
