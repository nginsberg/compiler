#include <string>
#include <iostream>

#include "Expressions.h"

using namespace std;

ostream &operator<<(ostream &os, const LExpr &expr) { return os << expr.str; }
ostream &operator<<(ostream &os, const ActualArgs &args) { return os << args.str; }
