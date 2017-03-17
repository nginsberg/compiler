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
    ret += forwardDecs + structDefs;
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

string generateTypeList(FormalArgs args) {
    string ret = "";

    for (auto arg = args.fArgs.begin(); arg != args.fArgs.end(); ++arg) {
        ret += "obj_" + arg->type;
        if (arg != --args.fArgs.end()) { ret += ", "; }
    }

    return ret;
}

string generateMethodSignature(Method m) {
    string ret = "";
    ret += "obj_" + m.retType + " (*" + m.name + ") (" + generateTypeList(m.fArgs) + ");";
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
                override.generatedSignature = generateMethodSignature(c->methods.methodForName(name));
                c->methodTable.push_back(override);
            }
        }
    }

    // Now add all the methods that are left
    for (auto name = methodNames.begin(); name != methodNames.end(); ++name) {
        MethodTableEntry newMethod;
        newMethod.methodName = *name;
        newMethod.generatedSignature = generateMethodSignature(c->methods.methodForName(*name));
        c->methodTable.push_back(newMethod);
    }

    // Now we can finally print out our method table
    for (int i = 0; i < c->methodTable.size(); ++i) {
        ret += "\t" + c->methodTable[i].generatedSignature + "\n";
    }

    return ret;
}
