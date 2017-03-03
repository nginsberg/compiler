#include <ostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <utility>
#include <string>
#include <queue>

#include "ClassHierarchy.h"
#include "Methods.h"

using namespace std;

/****************************** CLASS TABLE ******************************/

ostream &operator<<(ostream &os, const ClassSignature &cs) {
    return os << "Class: " << cs.className << " Superclass: " << cs.super;
}

ostream &operator<<(ostream &os, const Class &c) {
    os << "Class Signature:" << endl;
    os << c.cs << endl;
    os << "Class Body:" << endl;
    return os << c.cb << endl;
}

ostream &operator<<(ostream &os, const Classes &cls) {
    for_each(cls.classes.begin(), cls.classes.end(),
        [&] (Class entry) { os << entry << endl; });
    return os;
}

ostream &operator<<(ostream &os, const ClassBody &classBody) {
    os << "Methods:" << endl;
    os << classBody.mthds << endl;
    os << "Statements:" << endl;
    os << classBody.stmts.print(0) << endl;
    return os;
}

/****************************** CLASS TREE ******************************/

ClassTreeNode::ClassTreeNode(Classes &cls) {
    this->className = "Obj"; // We are root
    this->line = 0;

    // Add default subclasses
    ClassTreeNode *nothing = new ClassTreeNode("Nothing", FormalArgs(), 0, Methods(), Statements());
    nothing->superclass = this;
    this->subclasses.push_back(nothing);
    ClassTreeNode *intClass = new ClassTreeNode("Int", FormalArgs(), 0, Methods(), Statements());
    intClass->superclass = this;
    this->subclasses.push_back(intClass);
    ClassTreeNode *stringClass = new ClassTreeNode("String", FormalArgs(), 0, Methods(), Statements());
    stringClass->superclass = this;
    this->subclasses.push_back(stringClass);
    ClassTreeNode *boolClass = new ClassTreeNode("Boolean", FormalArgs(), 0, Methods(), Statements());
    boolClass->superclass = this;
    this->subclasses.push_back(boolClass);

    // First we check to make sure no class is defined multiple times
    vector<string> classes;
    for_each(cls.classes.begin(), cls.classes.end(),
        [&] (Class entry) {
            if (count(classes.begin(), classes.end(), entry.cs.className) > 0) {
                cerr << "Error: " << entry.cs.line << ": Multiple definitions "
                    << "of class " << entry.cs.className << ". Classes must "
                    << "only be defined once." << endl;
                exit(-1);
            }
            if (entry.cs.className == "Obj") {
                cerr << "Error: " << entry.cs.line << ": Obj is a predefined "
                    << "class and cannot be redefined." << endl;
                exit(-1);
            }
            if (entry.cs.className == "Nothing") {
                cerr << "Error: " << entry.cs.line << ": Nothing is a predefined "
                    << "class and cannot be redefined." << endl;
                exit(-1);
            }
            if (entry.cs.className == "Int") {
                cerr << "Error: " << entry.cs.line << ": Int is a predefined "
                    << "class and cannot be redefined." << endl;
                exit(-1);
            }
            if (entry.cs.className == "String") {
                cerr << "Error: " << entry.cs.line << ": String is a predefined "
                    << "class and cannot be redefined." << endl;
                exit(-1);
            }
            if (entry.cs.className == "Boolean") {
                cerr << "Error: " << entry.cs.line << ": Boolean is a predefined "
                    << "class and cannot be redefined." << endl;
                exit(-1);
            }

            classes.push_back(entry.cs.className);
        });

    // Now we actually build the tree. The plan is to start at the root, and
    // do a breadth first search. At each level, we will add elements of the
    // tree from the table.

    // Init queue with Obj, Nothing, String, and Int
    queue<ClassTreeNode *> nodesToSearch;
    nodesToSearch.push(this);
    for_each(this->subclasses.begin(), this->subclasses.end(),
        [&] (ClassTreeNode *ctn) { nodesToSearch.push(ctn); });

    // The actual search. Ran as long as there is something left to expand
    while (!nodesToSearch.empty()) {
        ClassTreeNode *current = nodesToSearch.front(); // Where we currently are
        nodesToSearch.pop(); // Remove it from the queue
        string name = current->className; // Our name
        // Go through the table.
        for (auto iter = cls.classes.begin(); iter != cls.classes.end();
            ++iter) {
            // Check if we've found something that inherits from us
            if (current->className == iter->cs.super) {
                // Create a new node
                ClassTreeNode *currentClass =
                    new ClassTreeNode(iter->cs.className, iter->cs.fArgs,
                        iter->cs.line, iter->cb.mthds, iter->cb.stmts);
                currentClass->superclass = current;

                // Add it as a subclass
                current->subclasses.push_back(currentClass);
                nodesToSearch.push(currentClass); // Add it to search
                iter = cls.classes.erase(iter); // Remove it from the table
                --iter; // Since we are about to increment
            }
        }
    }
}

// BFS of tree, printing it out in a way that GraphViz will understand:
ostream &operator<<(ostream &os, const ClassTreeNode &ctn) {
    os << "digraph {" << endl;

    int lbl = 0;
    string nodes = "";
    string edges = "";
    queue<const ClassTreeNode *>toPrint;
    toPrint.push(&ctn);
    while(!toPrint.empty()) {
        const ClassTreeNode *current = toPrint.front();
        toPrint.pop();

        // Add class node
        nodes += "\t\t" + current->className + " [label=\"" + \
            current->className + "(" + current->fArgs.toString() + "): " + \
                to_string(current->line) + "\"]\n";

        // Add method nodes, an edge from class to methods, method statements
        // and an edge from each method to its statements
        for_each(current->methods.methods.begin(), current->methods.methods.end(),
            [&] (Method m) {
            nodes += "\t\t" + current->className + "_" + m.name + \
                "[shape=box label=\"" + current->className + "." + m.name + \
                "(" + m.fArgs.toString() + "-> " + m.retType + "): " + \
                to_string(m.line) + "\"]\n";
            edges += "\t" + current->className + " -> " \
                + current->className + "_" + m.name + "\n";

            string lblName = "__STATEMENTS__" + to_string(lbl++);
            nodes += "\t\t" + m.stmts.node(lblName) + "\n";
            edges += "\t" +current->className + "_" + m.name + " -> " + \
                lblName + "\n";
        });

        // Add statements and edge to statements
        if(current->stmts.ss.size() > 0) {
            string lblName = "__STATEMENTS__" + to_string(lbl++);
            nodes += "\t\t" + current->stmts.node(lblName) + "\n";
            edges += "\t" + current->className + " -> " + lblName + "\n";
        }

        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                // Add edge from class to subclass
                edges += "\t" + current->className + " -> " \
                    + subclass->className + "\n";
                toPrint.push(subclass);
            });
    }

    os << "\t{" << endl;
    os << nodes << endl;
    os << "\t}" << endl;
    os << edges << endl;

    os << "}" << endl;
    return os;
}

// BFS until we find the right class or search the whole tree
ClassTreeNode *ClassTreeNode::classFromName(const string &name) {
    queue<ClassTreeNode *> toSearch;
    toSearch.push(this);

    while(!toSearch.empty()) {
        ClassTreeNode *current = toSearch.front();
        toSearch.pop();
        if (current->className == name) { return current; }
        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode * ctn) {
                toSearch.push(ctn);
            });
    }
    return NULL;
}

list<ClassTreeNode *>ClassTreeNode::superChain() {
    ClassTreeNode *current = this;
    list<ClassTreeNode *> ret;
    while (current->superclass) {
        ret.push_back(current->superclass);
        current = current->superclass;
    }

    return ret;
}

string ClassTreeNode::returnTypeForFunction(string name, list<string> argTypes) {
    list<ClassTreeNode *>toSearch = superChain();
    toSearch.push_front(this);

    for(auto cl = toSearch.begin(); cl != toSearch.end(); ++cl) {
        Methods ms = (*cl)->methods;
        for (auto method = ms.methods.begin(); method != ms.methods.end(); ++method) {
            // Check name is the same
            if (method->name != name) { continue; }
            // Check there are the right number of args
            if (method->fArgs.fArgs.size() != argTypes.size()) { continue; }
            // Make sure all types match
            auto req = method->fArgs.fArgs.begin(); // Required formal arg
            auto sup = argTypes.begin(); // Supplied type
            bool matchedUp = true;
            while (sup != argTypes.end()) {
                if (req->type != *sup) {
                    matchedUp = false;
                    break;
                }
                ++req;
                ++sup;
            }
            if (!matchedUp) { continue; }
            return method->retType;
        }
    }

    return "";
}

void ClassTreeNode::populateScopes(ClassTreeNode *AST) {
    Scope currentScope;
    Scope currentClassScope;
    constructorScope.addFormalArgs(fArgs);
    constructorScope.tokens["this"] = className;
    do {
        do {
            currentClassScope = scope;
            currentScope = constructorScope;

            // First we process the constructor
            updateScope(stmts, AST, constructorScope, scope, true);
        } while(currentScope.tokens != constructorScope.tokens || currentClassScope.tokens != scope.tokens);

        // Now we process each method
        for (auto m = methods.methods.begin(); m != methods.methods.end(); ++m) {
            Scope methodScope;
            Scope classScopeCopy;
            m->scope.addFormalArgs(m->fArgs);
            m->scope.tokens["this"] = className;
            do {
                methodScope = m->scope;
                classScopeCopy = scope;
                updateScope(m->stmts, AST, m->scope, scope, false);
            } while(methodScope.tokens != m->scope.tokens || classScopeCopy.tokens != scope.tokens);
        }
    } while(currentScope.tokens != constructorScope.tokens || currentClassScope.tokens != scope.tokens);
}

string leastCommonAncestor(ClassTreeNode *c1, ClassTreeNode *c2) {
    // First we get the superchain of each class
    list<ClassTreeNode *>s1 = c1->superChain();
    s1.push_front(c1);
    list<ClassTreeNode *>s2 = c2->superChain();
    s2.push_front(c2);

    for (auto super1 = s1.begin(); super1 != s1.end(); ++super1) {
        for (auto super2 = s2.begin(); super2 != s2.end(); ++super2) {
            if ((*super1)->className == (*super2)->className) {
                // We will always return here since we will eventually compare
                // and return Obj and Obj, since all superChains end at Obj.
                // G++ doesn't know this, though, so we need another return
                return (*super1)->className;
            }
        }
    }

    return "Obj";
}

void makeSureTableIsEmpty(const Classes &cls) {
    if (!cls.classes.empty()) {
        for_each(cls.classes.begin(), cls.classes.end(),
            [] (Class cl) {
                cerr << "Error: " << cl.cs.line << ": Unable to attatch "
                    << cl.cs.className << " to the class hierarchy. Did you "
                    << "forget to define its superclass, " << cl.cs.super
                    << "?" << endl;
            });
        exit(-1);
    }
}
