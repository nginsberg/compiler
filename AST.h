#ifndef AST_H_
#define AST_H_

#include "gc.h"
#include "gc_cpp.h"

#include <string>
#include <iostream>
#include <list>

class Method : public gc {
public:
    Method(std::string n): name(n) {}
    std::string name;
};

std::ostream &operator<<(std::ostream &os, const Method &mthd);

class Methods : public gc {
public:
    std::list<Method> methods;
};

std::ostream &operator<<(std::ostream &os, const Methods &mthds);


#endif
