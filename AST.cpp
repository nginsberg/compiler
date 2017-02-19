#include "AST.h"
#include "ClassHierarchy.h"

#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

ostream &operator<<(ostream &os, const Method &mthd) {
    return os << mthd.name;
}

ostream &operator<<(ostream &os, const Methods &mthds) {
    for_each(mthds.methods.begin(), mthds.methods.end(), [&] (Method m) {
        os << m << endl;
    });

    return os;
}

