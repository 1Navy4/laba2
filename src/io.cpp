#include "io.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;

string readInput(int argc, char** argv) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (string(argv[i]) == "--file") {
            ifstream in(argv[i + 1]);
            if (!in) { cerr << "ERROR: cannot open file: " << argv[i + 1] << endl; exit(1); }
            return string(istreambuf_iterator<char>(in), istreambuf_iterator<char>());
        }
    }
    return string(istreambuf_iterator<char>(cin), istreambuf_iterator<char>());
}
