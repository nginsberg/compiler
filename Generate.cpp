#include <string>
#include <queue>
#include <vector>
#include <algorithm>

#include "Methods.h"
#include "ClassHierarchy.h"
#include "Generate.h"

using namespace std;

string generateCode(ClassTreeNode *AST) {
    queue<ClassTreeNode *>toProcess;
    toProcess.push(AST);

    string forwardDecs = "";
    string structDefs = "";
    string methodDefs = "";

    while(!toProcess.empty()) {
        ClassTreeNode *current = toProcess.front();
        toProcess.pop();

        // We don't want to generate any code for the builtin classes
        bool skip = false;
        if (current->className == "Obj")     { skip = true; }
        if (current->className == "Int")     { skip = true; }
        if (current->className == "String")  { skip = true; }
        if (current->className == "Nothing") { skip = true; }
        if (current->className == "Boolean") { skip = true; }

        if (!skip) {
            forwardDecs += generateForwardDecsForClass(current);
            forwardDecs += "\n";
            structDefs += generateStructsForClass(current);
            structDefs += "\n";
            methodDefs += generateMethods(current);
            methodDefs += "\n";
        }

        if (skip) {
            generateClassStructContents(current); // So we have method tables
        }

        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                toProcess.push(subclass);
        });
    }

    string ret = "#include \"Builtins.h\"\n\n";
    ret += forwardDecs + structDefs + methodDefs;
    ret += "int main() { return 0; }\n";
    return ret;
}

string generateForwardDecsForClass(ClassTreeNode *c) {
    string ret = "";

    ret += "struct class_" + c->className + "_struct;\n";
    ret += "typedef struct class_" + c->className + "_struct* class_" + c->className + ";\n";
    ret += "extern class_" + c->className + " the_class_" + c->className + ";\n";
    ret += "\n";
    ret += "struct obj_" + c->className + "_struct;\n";
    ret += "typedef struct obj_" + c->className + "_struct* obj_" + c->className + ";\n";
    ret += "\n";

    return ret;
}

string generateStructsForClass(ClassTreeNode *c) {
    string ret = "";

    ret += "struct obj_" + c->className + "_struct {\n";
    ret += generateObjStructContents(c);
    ret += "};\n";
    ret += "\n";
    ret += "struct class_" + c->className + "_struct {\n";
    ret += generateClassStructContents(c);
    ret += "};\n";

    return ret;
}

string generateObjStructContents(ClassTreeNode *c) {
    string ret = "";

    // First the class pointer
    ret += "\tclass_" + c->className + " clazz;\n";

    for (int i = 0; i < c->superclass->scopeOrder.size(); ++i) {
        c->scopeOrder.push_back(c->superclass->scopeOrder[i]);
    }

    for (auto var = c->scope.tokens.begin(); var != c->scope.tokens.end(); ++var) {
        if (find(c->scopeOrder.begin(), c->scopeOrder.end(), var->first) == c->scopeOrder.end()) {
            c->scopeOrder.push_back(var->first);
        }
    }

    // Now the class scope, in the right order.
    for (int i = 0; i < c->scopeOrder.size(); ++i) {
        auto var = c->scope.tokens.find(c->scopeOrder[i]);
        ret += "\tobj_" + var->second + " " + var->first + ";\n";
    }

    return ret;
}

string generateMethods(ClassTreeNode *c) {
    string ret = "";

    for (auto m = c->methods.methods.begin(); m!= c->methods.methods.end(); ++m) {
        ret += generateMethod(*m, c->className);
        ret += "\n";
    }

    return ret;
}

string generateTypeList(FormalArgs args) {
    string ret = "";

    for (auto arg = args.fArgs.begin(); arg != args.fArgs.end(); ++arg) {
        ret += "obj_" + arg->type;
        if (arg != --args.fArgs.end()) { ret += ", "; }
    }

    return ret;
}

string generateArgList(FormalArgs args) {
    string ret = "";

    for (auto arg = args.fArgs.begin(); arg != args.fArgs.end(); ++arg) {
        ret += "obj_" + arg->type  + " " + arg->var;
        if (arg != --args.fArgs.end()) { ret += ", "; }
    }

    return ret;
}

string generateMethodSignature(Method m, string thisType) {
    string ret = "";
    string spacer = m.fArgs.fArgs.size() ? ", " : "";
    ret += "obj_" + m.retType + " (*" + m.name + ") (obj_" + thisType + spacer + generateTypeList(m.fArgs) + ");";
    return ret;
}

string generateClassStructContents(ClassTreeNode *c) {
    string ret = "";

    // First the constructor
    ret += "\tobj_" + c->className + " (*constructor) (" + generateTypeList(c->fArgs) + ");\n";

    // Here's the plan: Make a list of our methods. Then go through our
    // superclass' method table, either propogating the generated method
    // signature down, or replacing it if we override it. Then add any extra
    // methods that are new. Then we just print our method table.

    // Make the list of methods
    list<string> methodNames;
    for (auto m = c->methods.methods.begin(); m != c->methods.methods.end(); ++m) {
        methodNames.push_back(m->name);
    }

    // Go through the super's method table and add it to ours
    if (c->superclass) {
        for (int i = 0; i < c->superclass->methodTable.size(); ++i) {
            string name = c->superclass->methodTable[i].methodName;
            if (find(methodNames.begin(), methodNames.end(), name) == methodNames.end()) {
                // We didn't override this method, so just copy it over.
                c->methodTable.push_back(c->superclass->methodTable[i]);
            } else {
                methodNames.erase(find(methodNames.begin(), methodNames.end(), name));
                MethodTableEntry override;
                override.methodName = name;
                override.generatedSignature = generateMethodSignature(c->methods.methodForName(name), c->className);
                c->methodTable.push_back(override);
            }
        }
    }

    // Now add all the methods that are left
    for (auto name = methodNames.begin(); name != methodNames.end(); ++name) {
        MethodTableEntry newMethod;
        newMethod.methodName = *name;
        newMethod.generatedSignature = generateMethodSignature(c->methods.methodForName(*name), c->className);
        c->methodTable.push_back(newMethod);
    }

    // Now we can finally print out our method table
    for (int i = 0; i < c->methodTable.size(); ++i) {
        ret += "\t" + c->methodTable[i].generatedSignature + "\n";
    }

    return ret;
}

string generateMethod(Method m, std::string thisType) {
    string ret = "";
    string spacer = m.fArgs.fArgs.size() ? ", " : "";

    // Signature
    ret += "obj_" + m.retType + " " + thisType + "_method_" + m.name + "(obj_" + thisType + " this" + spacer + generateArgList(m.fArgs) + ") {\n";
    ret += generateVarDecs(m.stmts.scope, m.fArgs);
    ret += "\n";
    ret += generateStatements(m.stmts, m.stmts.scope);
    ret += "}\n";

    return ret;
}

string generateVarDecs(Scope s, FormalArgs passedIn) {
    string ret = "";

    for (auto tok = s.tokens.begin(); tok != s.tokens.end(); ++tok) {
        // Skip it if it got passed in or if it's a return type
        bool found = false;
        for (auto arg = passedIn.fArgs.begin(); arg != passedIn.fArgs.end(); ++arg) {
            if (arg->var == tok->first) {
                found = true;
                break;
            }
        }
        if (tok->first == "this" || tok->first == "$return") {
            found = true;
        }

        if (found) { continue; }

        ret += "\tobj_" + tok->second + " " + tok->first + ";\n";
    }

    return ret;
}

string generateVarDecs(Scope declared, Scope newScope, string buffer) {
    string ret = "";

    for (auto tok = newScope.tokens.begin(); tok != newScope.tokens.end(); ++tok) {
        if (declared.tokens.find(tok->first) != declared.tokens.end()) { continue; }

        ret += buffer + "\tobj_" + tok->second + " " + tok->first + ";\n";
    }

    return ret;
}

string generateStatements(Statements stmts, Scope sc, string buffer) {
    string ret = "";

    for (auto s = stmts.ss.begin(); s != stmts.ss.end(); ++s) {
        ret += generateStatement(*s, buffer, sc);
    }

    return ret;
}

string generateStatement(Statement *stmt, string buffer, Scope s) {
    string ret = "";

    if (BareStatement *bare = dynamic_cast<BareStatement *>(stmt)) {
        ret += buffer + "\t" + generateExpression(bare->expr) + ";\n";
    } else if (AssignStatement *assign = dynamic_cast<AssignStatement *>(stmt)) {
        ret += buffer + "\t" + generateExpression(assign->to) + " = " + generateExpression(assign->from) + ";\n";
    } else if (ReturnStatement *retStat = dynamic_cast<ReturnStatement *>(stmt)) {
        ret += buffer + "\treturn " + generateExpression(retStat->ret) + ";\n";
    } else if (WhileStatement *whileStat = dynamic_cast<WhileStatement *>(stmt)) {
        ret += generateVarDecs(s, whileStat->block.scope, buffer + "\t");
        ret += buffer + "\twhile (" + generateExpression(whileStat->ifTrue) + " == lit_true) {\n";
        ret += generateStatements(whileStat->block, s, buffer + "\t");
        ret += buffer + "\t}\n";
    } else if (Conditional *cond = dynamic_cast<Conditional *>(stmt)) {
        auto ifTrue = cond->conditionals.begin();
        auto block = cond->blocks.begin();

        ret += buffer + "\tif (" + generateExpression(*ifTrue) + " == lit_true) {\n";
        ret += generateVarDecs(s, block->scope, buffer + "\t");
        ret += generateStatements(*block, s, buffer + "\t");
        ++ifTrue; ++block;

        while (ifTrue != cond->conditionals.end()) {
            ret += buffer + "\t} else if (" + generateExpression(*ifTrue) + " == lit_true) {\n";
            ret += generateVarDecs(s, block->scope, buffer + "\t");
            ret += generateStatements(*block, s, buffer + "\t");
            ++ifTrue; ++block;
        }

        ret += buffer + "\t}\n";
    }

    return ret;
}

string generateArgList(ActualArgs args) {
    string ret = "";

    for (auto arg = args.args.begin(); arg != args.args.end(); ++arg) {
        ret += generateExpression(*arg);
        if (arg != --args.args.end()) { ret += ", "; }
    }

    return ret;
}

string generateExpression(RExpr *expr) {
    if (StringLit *strLit = dynamic_cast<StringLit *>(expr)) {
        return "str_literal(" + strLit->val + ")";
    } else if (IntLit *intLit = dynamic_cast<IntLit *>(expr)) {
        return "int_literal(" + to_string(intLit->val) + ")";
    } else if (BoolLit *boolLit = dynamic_cast<BoolLit *>(expr)) {
        if (boolLit->val) { return "lit_true"; }
        return "lit_false";
    } else if (LExpr *lexpr = dynamic_cast<LExpr *>(expr)) {
        string ret = lexpr->expr ? generateExpression(lexpr->expr) + "->" : "";
        return ret + lexpr->ident;
    } else if (ConstructorCall *constructor = dynamic_cast<ConstructorCall *>(expr)) {
        return "the_class_" + constructor->className + "->constructor(" + generateArgList(constructor->args) + ")";
    } else if (FunctionCall *func = dynamic_cast<FunctionCall *>(expr)) {
        string calledOn = generateExpression(func->expr);
        string argList = generateArgList(func->args);
        if (argList != "") { argList = ", " + argList; }
        return calledOn + "->clazz->" + func->functionName + "(" + calledOn + argList + ")";
    }
    return expr->print();
}
































