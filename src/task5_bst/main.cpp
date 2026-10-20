#include <iostream>
#include <sstream>

#include "bst.h"
#include "io.h"

using namespace std;

int main(int argc, char** argv) {
    istringstream in(readInput(argc, argv));
    BST tree;
    bool first = true;
    long long v;
    while (in >> v && v != 0) {
        int depth = tree.insert(v);
        if (depth == 0) continue;                 // повтор
        if (!first) cout << ' ';
        cout << depth;
        first = false;
    }
    cout << endl;
}
