#ifndef CLASS_HIERARCHY_H_
#define CLASS_HIERARCHY_H_

#include <string>
#include <iostream>

#include "gc.h"
#include "gc_cpp.h"

class ClassSignature : public gc {
public:
    ClassSignature(std::string c, std::string s): className(c), super(s) {}
    std::string className;
    std::string super;
};

std::ostream &operator<<(std::ostream &os, const ClassSignature &cs);

#endif
