#include <ostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <utility>
#include <string>

#include "ClassHierarchy.h"

using namespace std;

ostream &operator<<(ostream &os, const ClassSignature &cs) {
    return os << "Class: " << cs.className << " Superclass: " << cs.super;
}

ostream &operator<<(ostream &os, const Class &c) {
    return os << c.cs;
}

ostream &operator<<(ostream &os, const Classes &cls) {
    for_each(begin(cls.classTable), end(cls.classTable),
        [&] (pair<string, string> entry) {
            os << "Class: " << entry.first << " Superclass: " << entry.second
                << endl;
        });
    return os;
}
