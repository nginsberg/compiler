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
    } else if (BoolLit *boolLit = dynamic_cast<BoolLit *>(expr)) {
        return "Boolean";
    } else if (ConstructorCall *call = dynamic_cast<ConstructorCall *>(expr)) {
        ClassTreeNode *constructed = AST->classFromName(call->className);
        if (!constructed) {
            cerr << "Error: " << call->line << ": Call to constructor for class "
                << call->className << " when " << call->className << " was never "
                << "defined." << endl;
            return unknown;
        }

        list<string> argTypes;
        for (auto arg = call->args.args.begin(); arg != call->args.args.end();
            ++arg) {
            argTypes.push_back(type(*arg, AST, scope, classScope));
        }

        if (!constructed->validateConstructorArgs(argTypes, AST)) {
            cerr << "Error: " << call->line << ": Invalid arguments to "
                << "constructor for class " << call->className << endl;
            return unknown;
        }
        return call->className;
    } else if (LExpr *lexpr = dynamic_cast<LExpr *>(expr)) {
        if (lexpr->expr) {
            // The type of expr needs to be the same as the type of this, or
            // else we shouldn't be accessing the protected value. So we check
            // that, and then look up the ident in the class scope.

            // Make sure we are in a class (not global scope)
            if (scope.tokens.find("this") == scope.tokens.end()) {
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
                if (!c2) {
                    cerr << "Error: " << assignment->line << ": unrecognized "
                        << "type " << t << endl;
                    return;
                }
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
            // are stable.
            whileStatement->block.scope = scope;
            Scope whileClassScope = classScope;

            Scope scopeCopy;
            Scope classScopeCopy;
            do {
                scopeCopy = whileStatement->block.scope;
                classScopeCopy = whileClassScope;

                updateScope(whileStatement->block, AST, whileStatement->block.scope, whileClassScope, inConstructor);
            } while (whileStatement->block.scope.tokens != scopeCopy.tokens || whileClassScope.tokens != classScopeCopy.tokens);
        } else if (Conditional *conditional = dynamic_cast<Conditional *>(stmnt)) {
            // This is very similar to a while loop. First we go through and
            // add scopes to all the blocks in the conditional. Then we set
            // our scope to the intersection of those scopes; basically, if
            // a variable appears in all the scopes of the conditionals,
            // its least common ancestor is added to the main scope.

            for (auto stmnts = conditional->blocks.begin(); stmnts != conditional->blocks.end(); ++stmnts) {
                    stmnts->scope = scope;
                    Scope stmntsClassScope = classScope;

                    Scope scopeCopy;
                    Scope classScopeCopy;
                    do {
                        scopeCopy = stmnts->scope;
                        classScopeCopy = stmntsClassScope;

                        updateScope(*stmnts, AST, stmnts->scope, stmntsClassScope, inConstructor);
                    } while (stmnts->scope.tokens != scopeCopy.tokens || stmntsClassScope.tokens != classScopeCopy.tokens);
            };

            Scope newScope = conditional->blocks.begin()->scope;
            for (auto b = conditional->blocks.begin(); b != conditional->blocks.end(); ++b) {
                newScope = intersectScopes(newScope, b->scope, AST);
            }

            scope = newScope;
        }
    });
}

void computeAllScopes(ClassTreeNode *AST) {
    queue<ClassTreeNode *> toProcess;
    toProcess.push(AST);

    while (!toProcess.empty()) {
        ClassTreeNode *current = toProcess.front();
        toProcess.pop();

        // Compute scopes
        current->populateScopes(AST);
        // Add subclasses
        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                toProcess.push(subclass);
        });
    }
}

Scope intersectScopes(const Scope &s1, const Scope &s2, ClassTreeNode *AST) {
    Scope ret;
    for (auto t1 = s1.tokens.begin(); t1 != s1.tokens.end(); ++t1) {
        auto t2 = s2.tokens.find(t1->first);
        if (t2 != s2.tokens.end()) {
            ClassTreeNode *c1 = AST->classFromName(t1->second);
            ClassTreeNode *c2 = AST->classFromName(t2->second);
            ret.tokens[t1->first] = leastCommonAncestor(c1, c2);
        }
    }
    return ret;
}

bool checkAllMethods(ClassTreeNode *AST) {
    queue<ClassTreeNode *>toProcess;
    toProcess.push(AST);

    while(!toProcess.empty()) {
        ClassTreeNode *current = toProcess.front();
        toProcess.pop();

        if (!current->fArgs.typesExist(AST)) {
            cerr << "Error: " << current->line << ": Constructor defined with "
                << "arguements of unknown type." << endl;
            return false;
        }

        for (auto m = current->methods.methods.begin(); m != current->methods.methods.end(); ++m) {
            if (!m->fArgs.typesExist(AST)) {
                cerr << "Error: " << m->line << ": Method defined with "
                    << "arguements of unknown type." << endl;
                return false;
            }
            if (!AST->classFromName(m->retType)) {
                cerr << "Error: " << m->line << ": Unknown declared return "
                    << "type " << m->retType << "." << endl;
                return false;
            }
        }

        if (!current->methods.determineIfUnique()) { return false; }

        for (auto m = current->methods.methods.begin(); m != current->methods.methods.end(); ++m) {
            if (!m->determineIfOverrideOk(current, AST)) { return false; }
        }

        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                toProcess.push(subclass);
        });
    }

    return true;
}

bool checkAllReturns(ClassTreeNode *AST) {
    queue<ClassTreeNode *>toProcess;
    toProcess.push(AST);

    while(!toProcess.empty()) {
        ClassTreeNode *current = toProcess.front();
        toProcess.pop();

        // We don't want to check return types for our function stubs in the
        // default classes since they clearly are wrong.
        bool skip = false;
        if (current->className == "Obj")     { skip = true; }
        if (current->className == "Int")     { skip = true; }
        if (current->className == "String")  { skip = true; }
        if (current->className == "Nothing") { skip = true; }
        if (current->className == "Boolean") { skip = true; }

        if (!skip) {
            for (auto m = current->methods.methods.begin(); m != current->methods.methods.end(); ++m) {
                if (!m->checkRetType()) {
                    cerr << "Error: " << m->line << ": Method returned "
                        << m->stmts.scope.tokens.find("$return")->second
                        << " but was expected to return " << m->retType << endl;
                    return false;
                }
            }
        }

        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                toProcess.push(subclass);
        });
    }

    return true;
}

bool checkClassScopes(ClassTreeNode *AST) {
    queue<ClassTreeNode *>toProcess;
    toProcess.push(AST);

    int numErrors = 0;
    while(!toProcess.empty()) {
        ClassTreeNode *current = toProcess.front();
        toProcess.pop();

        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                for (auto v = current->scope.tokens.begin(); v != current->scope.tokens.end(); ++v) {
                    string type = v->second;
                    auto subV = subclass->scope.tokens.find(v->first);
                    if (subV == subclass->scope.tokens.end()) {
                        cerr << "Error: " << current->className << " contains "
                            << "a member " << v->first << " which is never "
                            << "defined in its subclass " << subclass->className
                            << "." << endl;
                        ++numErrors;
                        return;
                    }

                    ClassTreeNode *subVClass = AST->classFromName(subV->second);
                    if (!subVClass) {
                        cerr << "Error: Class " << subV->second << " does not "
                            << "exist." << endl;
                        ++numErrors;
                        return;
                    }
                    if (!subVClass->inheritsFrom(type)) {
                        cerr << "Error: " << current->className << " contains "
                            << "a member " << v->first << " which is redifined"
                            << " to a type that is not a subclass of "
                            << v->second << "." << endl;
                        ++numErrors;
                        return;
                    }
                }
            });

        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                toProcess.push(subclass);
        });
    }

    return numErrors == 0;
}






















