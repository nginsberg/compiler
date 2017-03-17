#ifndef GENERATE_H_
#define GENERATE_H_

#include <string>

#include "ClassHierarchy.h"

std::string generateCode(ClassTreeNode *AST);

std::string generateForwardDecsForClass(ClassTreeNode *c);

std::string generateStructsForClass(ClassTreeNode *c);

std::string generateObjStructContents(ClassTreeNode *c);

#endif
