#include <iostream>
#include <string>

#include "Statements.h"

using namespace std;

string BareStatement::print() {
    return expr.str;
}
