#ifndef EXPRESSIONS_H_
#define EXPRESSIONS_H_

#include <string>
#include <iostream>

#include "gc.h"
#include "gc_cpp.h"

class RExpr : public gc {
public:
    RExpr(std::string s): str(s) {}
    std::string str;
};

std::ostream &operator<<(std::ostream &os, const RExpr &expr);

class LExpr : public gc {
public:
    LExpr(std::string s): str(s) {}
    std::string str;
};

std::ostream &operator<<(std::ostream &os, const LExpr &expr);

class ActualArgs : public gc {
public:
    ActualArgs(std::string s): str(s) {}
    std::string str;
};

std::ostream &operator<<(std::ostream &os, const ActualArgs &args);

#endif
