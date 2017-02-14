#include <ostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <utility>
#include <string>
#include <queue>

#include "ClassHierarchy.h"

using namespace std;

/****************************** CLASS TABLE ******************************/

ostream &operator<<(ostream &os, const ClassSignature &cs) {
    return os << "Class: " << cs.className << " Superclass: " << cs.super;
}

ostream &operator<<(ostream &os, const Class &c) {
    return os << c.cs;
}

ostream &operator<<(ostream &os, const Classes &cls) {
    for_each(cls.classTable.begin(), cls.classTable.end(),
        [&] (pair<string, string> entry) {
            os << "Class: " << entry.first << " Superclass: " << entry.second
                << endl;
        });
    return os;
}

/****************************** CLASS TREE ******************************/

ClassTreeNode::ClassTreeNode(Classes &cls) {
    this->className = "Obj"; // We are root

    // Add default subclasses
    ClassTreeNode *nothing = new ClassTreeNode("Nothing");
    this->subclasses.push_back(nothing);
    ClassTreeNode *intClass = new ClassTreeNode("Int");
    this->subclasses.push_back(intClass);
    ClassTreeNode *stringClass = new ClassTreeNode("String");
    this->subclasses.push_back(stringClass);

    // First we check to make sure no class is defined multiple times
    vector<string> classes;
    for_each(cls.classTable.begin(), cls.classTable.end(),
        [&] (pair<string, string> entry) {
            if (count(classes.begin(), classes.end(), entry.first) > 0) {
                cerr << "Error: Multiple definitions of class " << entry.first
                    << ". Classes must only be defined once." << endl;
                exit(-1);
            }
            if (entry.first == "Obj") {
                cerr << "Error: Obj is a predefined class and cannot be redefined."
                    << endl;
                exit(-1);
            }
            if (entry.first == "Nothing") {
                cerr << "Error: Nothing is a predefined class and cannot be redefined."
                    << endl;
                exit(-1);
            }
            if (entry.first == "Int") {
                cerr << "Error: Int is a predefined class and cannot be redefined."
                    << endl;
                exit(-1);
            }
            if (entry.first == "String") {
                cerr << "Error: String is a predefined class and cannot be redefined."
                    << endl;
                exit(-1);
            }

            classes.push_back(entry.first);
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
        for (auto iter = cls.classTable.begin(); iter != cls.classTable.end();
            ++iter) {
            // Check if we've found something that inherits from us
            if (current->className == iter->second) {
                // Create a new node
                ClassTreeNode *currentClass = new ClassTreeNode(iter->first);
                // Add it as a subclass
                current->subclasses.push_back(currentClass);
                nodesToSearch.push(currentClass); // Add it to search
                iter = cls.classTable.erase(iter); // Remove it from the table
                --iter; // Since we are about to increment
            }
        }
    }
}

// BFS of tree, printing it out in a way that GraphViz will understand:
ostream &operator<<(ostream &os, const ClassTreeNode &ctn) {
    os << "digraph {" << endl;

    queue<const ClassTreeNode *>toPrint;
    toPrint.push(&ctn);
    while(!toPrint.empty()) {
        const ClassTreeNode *current = toPrint.front();
        toPrint.pop();
        for_each(current->subclasses.begin(), current->subclasses.end(),
            [&] (ClassTreeNode *subclass) {
                os << "\t" << current->className << " -> "
                    << subclass->className << endl;
                toPrint.push(subclass);
            });
    }

    os << "}" << endl;
    return os;
}

void makeSureTableIsEmpty(const Classes &cls) {
    if (!cls.classTable.empty()) {
        for_each(cls.classTable.begin(), cls.classTable.end(),
            [] (pair<string, string> cl) {
                cerr << "Error: Unable to attatch " << cl.first << " to the "
                    << "class hierarchy. Did you forget to define its "
                    << "superclass, " << cl.second << "?" << endl;
            });
        exit(-1);
    }
}











