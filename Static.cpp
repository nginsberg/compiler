#include <string>
#include <algorithm>

#include "Expressions.h"
#include "ClassHierarchy.h"
#include "Static.h"

using namespace std;

string type(RExpr *expr, ClassTreeNode *AST, const Scope &scope) {
    string unknown = "$Unknown";

    if(StringLit *strLit = dynamic_cast<StringLit *>(expr)) {
        return "String";
    } else if (IntLit *intLit = dynamic_cast<IntLit *>(expr)) {
        return "Int";
    } else if (ConstructorCall *call = dynamic_cast<ConstructorCall *>(expr)) {
        if (AST->classFromName(call->className)) { return call->className; }
        cerr << "Error: " << call->line << ": Call to coonstructor for class "
            << call->className << " when " << call->className << " was never "
            << "definded." << endl;
        return unknown;
    } else if (LExpr *lexpr = dynamic_cast<LExpr *>(expr)) {
        if(lexpr->expr) {
            // As far as I can tell, the only reason this should happen is for
            // access within a class, as with this.varname. Anything else would
            // be attempting to access a private field, since everything
            // defaults to private.
            if (lexpr->expr->print() != "this") {
                cerr << "Error: " << lexpr->expr->line << ": Attempt to access"
                    << " private member " << lexpr->ident << " of class "
                    << type(lexpr->expr, AST, scope) << endl;
                return unknown;
            }
        }
        if(scope.tokens.find(lexpr->print()) != scope.tokens.end()) {
            return scope.tokens.find(lexpr->print())->second;
        }
        return unknown;
    } else if (FunctionCall *call = dynamic_cast<FunctionCall *>(expr)) {
        // TODO: this doesn't check up the inheritance tree yet.

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
        string ret = unknown;
        for_each(cl->methods.methods.begin(), cl->methods.methods.end(),
            [&] (Method m) {
                if (call->functionName == m.name) {
                    ret = m.retType;
                }
            });
        return ret;
    }
    return unknown;
}
