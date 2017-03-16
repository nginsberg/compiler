#ifndef GENERATE_H_
#define GENERATE_H_

#include <string>

#include "ClassHierarchy.h"

std::string generateCode(ClassTreeNode *AST);

std::string generateStructsForClass(ClassTreeNode *c, ClassTreeNode *AST);

#endif
