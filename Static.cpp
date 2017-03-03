#include <string>
#include <algorithm>

#include "Expressions.h"
#include "ClassHierarchy.h"

using namespace std;

string type(RExpr *expr, ClassTreeNode *AST, const Scope &scope,
        const Scope &classScope) {
    string unknown = "$Unknown";

    if(StringLit *strLit = dynamic_cast<StringLit *>(expr)) {
        return "String";
    } else if (IntLit *intLit = dynamic_cast<IntLit *>(expr)) {
        return "Int";
    } else if (ConstructorCall *call = dynamic_cast<ConstructorCall *>(expr)) {
        if (AST->classFromName(call->className)) { return call->className; }
        cerr << "Error: " << call->line << ": Call to constructor for class "
            << call->className << " when " << call->className << " was never "
            << "definded." << endl;
        return unknown;
    } else if (LExpr *lexpr = dynamic_cast<LExpr *>(expr)) {
        if (lexpr->expr) {
            // The type of expr needs to be the same as the type of this, or
            // else we shouldn't be accessing the protected value. So we check
            // that, and then look up the ident in the class scope.

            // Make sure we are in a class (not global scope)
            if (classScope.tokens.find("this") == classScope.tokens.end()) {
                cerr << "Error: " << lexpr->line << ": Attempt to access "
                    << " member value from global scope." << endl;
                return unknown;
            }

            // Make sure we are attempting to access data from the same class
            string thisType = classScope.tokens.find("this")->second;
            string otherType = type(lexpr->expr, AST, scope, classScope);
            if (thisType != otherType) {
                cerr << "Error: " << lexpr->line << ": Attempt to access "
                    << " member value from variable of type " << otherType
                    << " in class " << thisType << endl;
                return unknown;
            }

            // Make sure our class has a member of the provided name
            if (classScope.tokens.find(lexpr->ident) == classScope.tokens.end()) {
                cerr << "Error: " << lexpr->line << ": Class " << thisType
                    << " does not have a member value named " << lexpr->ident
                    << endl;
                return unknown;
            }

            // Return the type of the ident from our class scope.
            return classScope.tokens.find(lexpr->ident)->second;
        } else {
            // Make sure the local scope has a variable of the provided name
            if (scope.tokens.find(lexpr->ident) == scope.tokens.end()) {
                cerr << "Error: " << lexpr->line << ": Attempt to access "
                    << " uninitialized variable " << lexpr->ident << endl;
                return unknown;
            }

            return scope.tokens.find(lexpr->ident)->second;
        }
    } else if (FunctionCall *call = dynamic_cast<FunctionCall *>(expr)) {
        // Get the name of the class
        string className = type(call->expr, AST, scope);
        if (className == unknown) {
            cerr << "Error: " << call->line << ": Attempt to call function "
                << call->functionName << " on " << call->expr->print()
                << " which has an unknown type." << endl;
            return unknown;
        }

        // Find the class in the AST
        ClassTreeNode *cl = AST->classFromName(className);
        if (!cl) {
            cerr << "Error: " << call->line << ": Class " << className
                << " is not definded." << endl;
            return unknown;
        }

        // Find out if the class responsds to the method
        string name = call->functionName;
        list<string> argTypes;
        for (auto arg = call->args.args.begin(); arg != call->args.args.end();
            ++arg) {
            argTypes.push_back(type(*arg, AST, scope, classScope));
        }
        string ret = cl->returnTypeForFunction(name, argTypes);
        if (ret == "") {
            cerr << "Error: " << call->line << ": Unable to find function "
                << "matching " << call->print() << endl;
            return unknown;
        }
        return ret;
    }
    return unknown;
}
