#include "stack.h"

#include <algorithm>

using namespace std;

void Stack::push(const string& v) { l.pushHead(v); }

string Stack::pop() {
    if (!l.size()) fail("stack is empty");
    return l.popHead();
}

Strs Stack::items() const { return l.items(); }

Strs Stack::dump() const {
    Strs v = l.items();
    reverse(v.begin(), v.end());
    return v;
}

void Stack::load(const Strs& t) { for (auto& s : t) push(s); }

string Stack::show() const {
    Strs v = l.items();
    if (v.empty()) return "(empty stack)";
    string r;
    for (size_t i = 0; i < v.size(); ++i) {
        r = r + "| " + v[i] + " |";
        if (i == 0) r = r + "  <- top";
        r = r + "\n";
    }
    return r + "+---+";
}
