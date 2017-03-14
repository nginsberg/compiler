#include "Methods.h"
#include "ClassHierarchy.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <list>

using namespace std;

ostream &operator<<(ostream &os, const Method &mthd) {
    os << "Name: " << mthd.name << " Args: " << mthd.fArgs << " Return Type: "
        << mthd.retType << " Line: " << mthd.line << endl;
    return os << mthd.stmts.print(0);
}

bool Methods::determineIfUnique() {
    list<string> seen;
    for (auto m = methods.begin(); m != methods.end(); ++m) {
        if (find(seen.begin(), seen.end(), m->name) != seen.end()) {
            cerr << "Error: " << m->line << ": Redefinition of method "
                << m->name << endl;
            return false;
        }
        seen.push_back(m->name);
    }
    return true;
}

ostream &operator<<(ostream &os, const Methods &mthds) {
    for_each(mthds.methods.begin(), mthds.methods.end(), [&] (Method m) {
        os << m << endl;
    });

    return os;
}

ostream &operator<<(ostream &os, const FormalArg &formalArg) {
    return os << formalArg.var << ": " << formalArg.type;
}

string FormalArgs::toString() const {
    string ret = "";
    for_each(fArgs.begin(), fArgs.end(), [&] (FormalArg arg) {
        ret += arg.var + ": " + arg.type + " ";
    });
    return ret;
}

ostream &operator<<(ostream &os, const FormalArgs &formalArgs) {
    for_each(formalArgs.fArgs.begin(), formalArgs.fArgs.end(), [&] (FormalArg arg) {
        os << arg << " ";
    });

    return os;
}

void Scope::addFormalArgs(const FormalArgs &fArgs) {
    for_each(fArgs.fArgs.begin(), fArgs.fArgs.end(), [&] (FormalArg arg) {
        tokens[arg.var] = arg.type;
    });
}

void Scope::print() const {
    for_each(tokens.begin(), tokens.end(), [] (pair<string, string> entry) {
        cout << entry.first << ": " << entry.second << endl;
    });
}
