#include <string>
#include <iostream>
#include <algorithm>

#include "Expressions.h"

using namespace std;

string LExpr::print() {
    string ret = "";
    if (expr) {
        ret += expr->print() + ".";
    }
    ret += ident;
    return ret;
}

string ConstructorCall::print() {
    return className + "(" + args.print() + ")";
}

string ActualArgs::print() {
    string ret = "";
    for_each(args.begin(), args.end(), [&] (RExpr *expr) {
        ret += expr->print() + ", ";
    });
    return ret;
}
