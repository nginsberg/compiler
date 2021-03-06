#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <string>
#include <iostream>
#include <list>
#include <utility>
#include <vector>

#include "gc.h"
#include "Methods.h"
#include "Statements.h"

/****************************** CLASS TABLE ******************************/

class ClassSignature {
public:
    ClassSignature(std::string c, FormalArgs args, std::string s, int l):
        className(c), fArgs(args), super(s), line(l) {}
    std::string className;
    FormalArgs fArgs;
    std::string super;
    int line;
};

std::ostream &operator<<(std::ostream &os, const ClassSignature &cs);

class ClassBody {
public:
    ClassBody(const Methods &ms, const Statements &ss): mthds(ms), stmts(ss) {}
    Methods mthds;
    Statements stmts;
};

std::ostream &operator<<(std::ostream &os, const ClassBody &classBody);

class Class {
public:
    Class(const ClassSignature &sig, const ClassBody &bod): cs(sig), cb(bod) {}
    ClassSignature cs;
    ClassBody cb;
};

std::ostream &operator<<(std::ostream &os, const Class &c);

class Classes {
public:
    std::list<Class> classes;
};

std::ostream &operator<<(std::ostream &os, const Classes &cls);

/****************************** CLASS TREE ******************************/

struct MethodTableEntry {
    std::string methodName;
    std::string generatedSignature;
    std::string generatedName;
};

class ClassTreeNode {
public:
    // Removes classes from table as it adds them - makes it easy to check
    // if the class hierarchy is valid.
    ClassTreeNode(Classes &cls);
    ClassTreeNode(std::string name, FormalArgs args, int l, Methods m, Statements ss):
        className(name), fArgs(args), line(l), methods(m), stmts(ss),
        superclass(NULL) {}

    ClassTreeNode *classFromName(const std::string &name);
    // A list of all classes this inherits from
    std::list<ClassTreeNode *>superChain();
    // Returns the return type for the corresponding function, or the empty
    // string if none is found.
    std::string returnTypeForFunction(std::string name,
        std::list<std::string> argTypes, ClassTreeNode *AST);

    bool validateConstructorArgs(std::list<std::string> argTypes, ClassTreeNode *AST);

    // Calculate the scope for the class and its methods.
    int populateScopes(ClassTreeNode *AST);

    bool inheritsFrom(std::string possibleSuper);

    std::list<ClassTreeNode *> subclasses;
    ClassTreeNode *superclass;

    std::string className;
    int line;
    Methods methods;
    FormalArgs fArgs;
    Statements stmts;
    Scope scope;

    std::vector<std::string> scopeOrder;
    std::vector<MethodTableEntry> methodTable;
};

std::string type(RExpr *expr, ClassTreeNode *AST, const Scope &scope,
    const Scope &classScope = Scope());

std::string leastCommonAncestor(ClassTreeNode *c1, ClassTreeNode *c2);

int updateScope(const Statements &stmts, ClassTreeNode *AST, Scope &scope,
    Scope &classScope, bool inConstructor);

bool computeAllScopes(ClassTreeNode *AST);

bool checkAllMethods(ClassTreeNode *AST);

bool checkAllReturns(ClassTreeNode *AST);

bool checkClassScopes(ClassTreeNode *AST);

std::ostream &operator<<(std::ostream &os, const ClassTreeNode &ctn);

void makeSureTableIsEmpty(const Classes &cls);

Scope intersectScopes(const Scope &s1, const Scope &s2, ClassTreeNode *AST);

#endif
