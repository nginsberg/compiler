#include <string>
#include <algorithm>
#include <queue>

#include "Expressions.h"
#include "ClassHierarchy.h"

using namespace std;

static string unknown = "$Unknown";
static string returnToken = "$return";

string type(RExpr *expr, ClassTreeNode *AST, const Scope &scope, const Scope &classScope) {

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
            if (scope.tokens.find("this") == scope.tokens.end()) {
                cout << lexpr->print() << endl;
                cerr << "Error: " << lexpr->line << ": Attempt to access "
                    << " member value from global scope." << endl;
                return unknown;
            }

            // Make sure we are attempting to access data from the same class
            string thisType = scope.tokens.find("this")->second;
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
                cout << "Class scope:" << endl;
                classScope.print();
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
        string className = type(call->expr, AST, scope, classScope);
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
                << " is not defined." << endl;
            return unknown;
        }

        // Find out if the class responsds to the method
        string name = call->functionName;
        list<string> argTypes;
        for (auto arg = call->args.args.begin(); arg != call->args.args.end();
            ++arg) {
            argTypes.push_back(type(*arg, AST, scope, classScope));
        }
        string ret = cl->returnTypeForFunction(name, argTypes, AST);
        if (ret == "") {
            cerr << "Error: " << call->line << ": Unable to find function "
                << "matching " << call->print() << endl;
            return unknown;
        }
        return ret;
    }
    return unknown;
}

void updateScope(const Statements &stmts, ClassTreeNode *AST, Scope &scope,
    Scope &classScope, bool inConstructor) {
    for_each(stmts.ss.begin(), stmts.ss.end(), [&] (Statement *stmnt) {
        if (AssignStatement *assignment = dynamic_cast<AssignStatement *>(stmnt)) {
            // First we determine if this is an assignment to the local scope
            // or the class scope
            Scope *usedScope;
            bool canCreateNewVariable = true;
            if (assignment->to->expr) {
                if (assignment->to->expr->print() != "this") {
                    cerr << "Error: " << assignment->line << ": Attempt to "
                        << "assign to a member variable from a different class"
                        << "." << endl;
                    return;
                }
                usedScope = &classScope;
                canCreateNewVariable = inConstructor;
            } else {
                usedScope = &scope;
            }

            // Determine the new type
            string t = type(assignment->from, AST, scope, classScope);
            if (t == unknown) {
                cerr << "Error: " << assignment->line << ": Cannot determine "
                    << "type of rexpr " << assignment->from->print() << endl;
                return;
            }

            // Add it to the proper scope.
            auto var = usedScope->tokens.find(assignment->to->ident);
            if (var == usedScope->tokens.end()) { // New variable
                if (!canCreateNewVariable) {
                    cerr << "Error: " << assignment->line << ": class member "
                        << "variable " << assignment->to->ident << " must be "
                        << "initialized in the constructor." << endl;
                    return;
                }
                usedScope->tokens[assignment->to->ident] = t;
            } else {
                ClassTreeNode *c1 = AST->classFromName(var->second);
                ClassTreeNode *c2 = AST->classFromName(t);
                usedScope->tokens[assignment->to->ident] = leastCommonAncestor(c1, c2);
            }
        } else if (ReturnStatement *retSatement = dynamic_cast<ReturnStatement *>(stmnt)) {
            string t = type(retSatement->ret, AST, scope, classScope);
            if (t == unknown) {
                cerr << "Error: " << retSatement->line << ": Cannot determine "
                    << "type of rexpr " << retSatement->ret->print() << endl;
                return;
            }

            // Figure out if we've encountered a return statement before
            auto var = scope.tokens.find(returnToken);
            if (var == scope.tokens.end()) {
                scope.tokens[returnToken] = t;
            } else {
                ClassTreeNode *c1 = AST->classFromName(var->second);
                ClassTreeNode *c2 = AST->classFromName(t);
                scope.tokens[returnToken] = leastCommonAncestor(c1, c2);
            }
        } else if (BareStatement *bareStatement = dynamic_cast<BareStatement *>(stmnt)) {
            // We just have to check to make sure we can get a type for the rexpr
            string t = type(bareStatement->expr, AST, scope, classScope);
            if (t == unknown) {
                cerr << "Error: " << bareStatement->line << ": Cannot determine "
                    << "type of rexpr " << bareStatement->expr->print() << endl;
                return;
            }
        } else if (WhileStatement *whileStatement = dynamic_cast<WhileStatement *>(stmnt)) {
            // First we check to make sure the conditional is a subclass of Boolean
            string t = type(whileStatement->ifTrue, AST, scope, classScope);
            if (t == unknown) {
                cerr << "Error: " << whileStatement->line << ":  Cannot determine "
                    << "type of rexpr " << whileStatement->ifTrue->print() << endl;
                return;
            }
            ClassTreeNode *cl = AST->classFromName(t);
            if (!cl->inheritsFrom("Boolean")) {
                cerr << "Error: " << whileStatement->line << ": Conditional "
                    << "must have a type that inherits from Boolean, which "
                    << t << " does not" << endl;
                return;
            }

            // Now we make copies of the scope and update the copies until they
            // are stable. Then we throw the copies away, since the while loop
            // may never execute, but we want to make sure there are no errors
            Scope scopeCopy;
            Scope classScopeCopy;
            do {
                scopeCopy = scope;
                classScopeCopy = classScope;

                updateScope(whileStatement->block, AST, scopeCopy, classScopeCopy, inConstructor);
            } while (scope.tokens != scopeCopy.tokens || classScope.tokens != classScopeCopy.tokens);
        }
    });
}

void computeAllScopes(ClassTreeNode *AST) {
    queue<ClassTreeNode *> toProcess;
    toProcess.push(AST);

    while (!toProcess.empty()) {
        ClassTreeNode *current = toProcess.front();
        toProcess.pop();

        cout << "Processing " << current->className << endl;
        // Compute scopes
        current->populateScopes(AST);
        cout << endl;
        // Add subclasses
        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                toProcess.push(subclass);
        });
    }
}

