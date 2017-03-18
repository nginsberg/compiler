#ifndef GENERATE_H_
#define GENERATE_H_

#include <string>

#include "ClassHierarchy.h"
#include "Methods.h"
#include "Statements.h"

std::string generateCode(ClassTreeNode *AST);

std::string generateForwardDecsForClass(ClassTreeNode *c);

std::string generateStructsForClass(ClassTreeNode *c);

std::string generateObjStructContents(ClassTreeNode *c);

std::string generateMethods(ClassTreeNode *c);

std::string generateMethodSignature(Method m, std::string thisType);

std::string generateTypeList(FormalArgs args);

std::string generateArgList(FormalArgs args);

std::string generateClassStructContents(ClassTreeNode *c);

std::string generateMethod(Method m, std::string thisType);

std::string generateVarDecs(Scope s, FormalArgs passedIn);

std::string generateVarDecs(Scope declared, Scope newScope, std::string buffer);

std::string generateStatements(Statements stmts, Scope s, std::string buffer = "");

std::string generateStatement(Statement *stmt, std::string buffer, Scope s);

std::string generateArgList(ActualArgs args);

std::string generateExpression(RExpr *expr);

#endif
