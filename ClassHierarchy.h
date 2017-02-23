#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <string>
#include <iostream>
#include <list>
#include <utility>
#include <vector>

#include "gc.h"
#include "Methods.h"

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
    ClassBody(const Methods &ms): mthds(ms) {}
    Methods mthds;
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

class ClassTreeNode {
public:
    // Removes classes from table as it adds them - makes it easy to check
    // if the class hierarchy is valid.
    ClassTreeNode(Classes &cls);
    ClassTreeNode(std::string name, FormalArgs args, int l, Methods m):
        className(name), fArgs(args), line(l), methods(m) {}

    bool makeSureClassExists(const std::string &name);

    std::list<ClassTreeNode *> subclasses;

    std::string className;
    int line;
    Methods methods;
    FormalArgs fArgs;
};

std::ostream &operator<<(std::ostream &os, const ClassTreeNode &ctn);

void makeSureTableIsEmpty(const Classes &cls);

#endif