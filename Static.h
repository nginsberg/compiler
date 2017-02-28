#ifndef STATIC_H_
#define STATIC_H_

#include <string>
#include <map>

#include "Expressions.h"
#include "ClassHierarchy.h"

class Scope {
public:
    std::map<std::string, std::string> tokens;
};

std::string type(RExpr *expr, ClassTreeNode *AST, const Scope &scope);

#endif
