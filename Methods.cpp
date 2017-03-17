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

Method Methods::methodForName(std::string name) {
    auto fallback = methods.begin();
    for (auto m = methods.begin(); m != methods.end(); ++m) {
        if (m->name == name) { return *m; }
    }
    return *fallback;
}

ostream &operator<<(ostream &os, const Methods &mthds) {
    for_each(mthds.methods.begin(), mthds.methods.end(), [&] (Method m) {
        os << m << endl;
    });

    return os;
}

bool Method::determineIfOverrideOk(ClassTreeNode *owner, ClassTreeNode *AST) {
    list<ClassTreeNode *> toSearch = owner->superChain();
    for (auto c = toSearch.begin(); c != toSearch.end(); ++c) {
        for (auto m = (*c)->methods.methods.begin(); m != (*c)->methods.methods.end(); ++m) {
            if (m->name == name) { // We found the method we are overriding
                // Check return type is a subclass
                ClassTreeNode *retTypeClass = AST->classFromName(retType);
                if (!retTypeClass) {
                    cerr << "Error: " << line << ": return type " << retType
                        << " is not a defined class." << endl;
                    return false;
                }
                if (!retTypeClass->inheritsFrom(m->retType)) {
                    cerr << "Error: " << line << ": return type " << retType
                        << "is not a subclass of return type of overridden "
                        << "method." << endl;
                    return false;
                }

                // Check the argument types match
                if (fArgs.fArgs.size() != m->fArgs.fArgs.size()) {
                    cerr << "Error: " << line << ": different number of "
                        << "arguments from overridden method" << endl;
                    return false;
                }
                auto arg = fArgs.fArgs.begin();
                auto sArg = m->fArgs.fArgs.begin();
                while (arg != fArgs.fArgs.end()) {
                    ClassTreeNode *sArgClass = AST->classFromName(sArg->type);
                    if (!sArgClass->inheritsFrom(arg->type)) {
                        cerr << "Error: " << line << ": " << arg->var
                            << " has a type that is not a superclass of its "
                            << "type in the overridden method." << endl;
                        return false;
                    }

                    ++arg; ++sArg;
                }
            }
        }
    }

    return true;
}

bool Method::checkRetType(ClassTreeNode *AST) {
    string actual = stmts.scope.tokens.find("$return")->second;
    ClassTreeNode *actualClass = AST->classFromName(actual);
    return actualClass->inheritsFrom(retType);
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

bool FormalArgs::typesExist(ClassTreeNode *AST) const {
    for (auto arg = fArgs.begin(); arg != fArgs.end(); ++arg) {
        if (!AST->classFromName(arg->type)) { return false; }
    }
    return true;
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
