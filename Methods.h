#ifndef METHODS_H_
#define METHODS_H_

#include "gc.h"
#include "gc_cpp.h"

#include <string>
#include <iostream>
#include <list>

class FormalArg : public gc {
public:
    FormalArg(std::string v, std::string t): var(v), type(t) {}
    std::string var;
    std::string type;
};

std::ostream &operator<<(std::ostream &os, const FormalArg &formalArg);

class FormalArgs : public gc {
public:
    FormalArgs() {}
    FormalArgs(const FormalArg &fArg) { fArgs.push_back(fArg); }
    std::list<FormalArg> fArgs;

    std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const FormalArgs &formalArgs);

class Method : public gc {
public:
    Method(std::string n, FormalArgs args, std::string r, int l):
        name(n), retType(r), fArgs(args), line(l) {}
    std::string name;
    std::string retType;
    FormalArgs fArgs;
    int line;
};

std::ostream &operator<<(std::ostream &os, const Method &mthd);

class Methods : public gc {
public:
    std::list<Method> methods;
};

std::ostream &operator<<(std::ostream &os, const Methods &mthds);

#endif
