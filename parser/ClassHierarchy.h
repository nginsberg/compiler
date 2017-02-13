#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <string>
#include <iostream>
#include <vector>
#include <utility>

#include "gc.h"
#include "gc_cpp.h"

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
    std::vector<std::pair<std::string, std::string>> classTable;
};

std::ostream &operator<<(std::ostream &os, const Classes &cls);

#endif
