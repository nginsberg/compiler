#ifndef GENERATE_H_
#define GENERATE_H_

#include <string>

#include "ClassHierarchy.h"
#include "Methods.h"

std::string generateCode(ClassTreeNode *AST);

std::string generateForwardDecsForClass(ClassTreeNode *c);

std::string generateStructsForClass(ClassTreeNode *c);

std::string generateObjStructContents(ClassTreeNode *c);

std::string generateMethodSignature(Method m);

std::string generateTypeList(FormalArgs args);

std::string generateClassStructContents(ClassTreeNode *c);

#endif
