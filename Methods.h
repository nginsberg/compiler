#ifndef METHODS_H_
#define METHODS_H_

#include "gc.h"
#include "Statements.h"

#include <string>
#include <iostream>
#include <list>
#include <map>

class FormalArg {
public:
    FormalArg(std::string v, std::string t): var(v), type(t) {}
    std::string var;
    std::string type;
};

std::ostream &operator<<(std::ostream &os, const FormalArg &formalArg);

class FormalArgs {
public:
    FormalArgs() {}
    FormalArgs(const FormalArg &fArg) { fArgs.push_back(fArg); }
    std::list<FormalArg> fArgs;

    std::string toString() const;
};

std::ostream &operator<<(std::ostream &os, const FormalArgs &formalArgs);

class Method {
public:
    Method(std::string n, FormalArgs args, std::string r, Statements ss, int l):
        name(n), retType(r), fArgs(args), stmts(ss), line(l) {}
    std::string name;
    std::string retType;
    FormalArgs fArgs;
    Statements stmts;
    int line;
};

std::ostream &operator<<(std::ostream &os, const Method &mthd);

class Methods {
public:
    std::list<Method> methods;
    bool determineIfUnique();
};

std::ostream &operator<<(std::ostream &os, const Methods &mthds);

#endif
