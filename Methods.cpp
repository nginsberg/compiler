#include "Methods.h"
#include "ClassHierarchy.h"

#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

ostream &operator<<(ostream &os, const Method &mthd) {
    return os << "Name: " << mthd.name << " Args: " << mthd.fArgs
        << " Return Type: " << mthd.retType << " Line: " << mthd.line;
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
