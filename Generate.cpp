#include <string>
#include <queue>
#include <vector>
#include <algorithm>

#include "ClassHierarchy.h"
#include "Generate.h"

using namespace std;

string generateCode(ClassTreeNode *AST) {
    string ret = "";
    queue<ClassTreeNode *>toProcess;
    toProcess.push(AST);

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
            ret += generateStructsForClass(current, AST);
            ret += "\n";
        }

        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                toProcess.push(subclass);
        });
    }

    return ret;
}

string generateStructsForClass(ClassTreeNode *c, ClassTreeNode *AST) {
    string ret = "";

    ret += "struct class_" + c->className + "_struct;\n";
    ret += "typedef struct class_" + c->className + "_struct* class_" + c->className + ";\n";
    ret += "\n";
    ret += "typedef struct obj_" + c->className + "_struct {\n";
    ret += generateObjStructContents(c);
    ret += "} * obj_" + c->className + ";\n";
    ret += "\n";
    ret += "struct class_" + c->className + "_struct {\n";
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
        auto v = find(c->scopeOrder.begin(), c->scopeOrder.end(), var->first);
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
