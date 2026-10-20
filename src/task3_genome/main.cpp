#include <iostream>
#include <sstream>

#include "hashset.h"
#include "io.h"

using namespace std;

static string trimmed(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

static bool onlyLetters(const string& s) {
    for (char c : s) if (c < 'A' || c > 'Z') return false;
    return true;
}

int main(int argc, char** argv) {
    istringstream in(readInput(argc, argv));
    string first, second;
    getline(in, first);
    getline(in, second);
    first = trimmed(first);
    second = trimmed(second);
    if (first.empty() || second.empty() || !onlyLetters(first) || !onlyLetters(second)) {
        cerr << "ERROR: нужны две непустые строки из заглавных латинских букв A-Z" << endl;
        return 1;
    }

    HashSet pairs;                                          // пары второго генома
    for (size_t i = 0; i + 1 < second.size(); ++i) pairs.add(second.substr(i, 2));

    long long closeness = 0;
    for (size_t i = 0; i + 1 < first.size(); ++i)           // каждая пара первого генома (с повторами)
        if (pairs.contains(first.substr(i, 2))) ++closeness;
    cout << closeness << endl;
}
