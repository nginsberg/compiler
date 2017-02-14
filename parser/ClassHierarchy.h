#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <string>
#include <iostream>
#include <list>
#include <utility>

#include "gc.h"
#include "gc_cpp.h"

/****************************** CLASS TABLE ******************************/

class ClassSignature : public gc {
public:
    ClassSignature(std::string c, std::string s): className(c), super(s) {}
    std::string className;
    std::string super;
};

std::ostream &operator<<(std::ostream &os, const ClassSignature &cs);

class Class : public gc {
public:
    Class(const ClassSignature &in): cs(in) {}
    ClassSignature cs;
};

std::ostream &operator<<(std::ostream &os, const Class &c);

class Classes : public gc {
public:
    std::list<std::pair<std::string, std::string>> classTable;
};

std::ostream &operator<<(std::ostream &os, const Classes &cls);

/****************************** CLASS TREE ******************************/

class ClassTreeNode : public gc {
public:
    // Removes classes from table as it adds them - makes it easy to check
    // if the class hierarchy is valid.
    ClassTreeNode(Classes &cls);
    ClassTreeNode(std::string name): className(name) {}
    std::list<ClassTreeNode *> subclasses;
    std::string className;
};

std::ostream &operator<<(std::ostream &os, const ClassTreeNode &ctn);

void makeSureTableIsEmpty(const Classes &cls);

#endif
