#include <string>
#include <queue>
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
    ret += "} * obj_" + c->className + ";\n";

    return ret;
}
