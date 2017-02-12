#include <ostream>

#include "ClassHierarchy.h"

using namespace std;

ostream &operator<<(ostream &os, const ClassSignature &cs) {
    return os << "Class: " << cs.className << " Superclass: " << cs.super;
}
