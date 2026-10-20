#include "massiv.h"

using namespace std;

void Array::check(size_t i) const {
    if (i >= len) fail("index out of range: " + to_string(i));
}

void Array::insert(size_t idx, const string& v) {
    if (idx > len) fail("index out of range: " + to_string(idx));
    if (len == cap) {  // удвоение емкости: добавление в конец амортизированно O(1)
        cap = cap * 2;
        string* nd = new string[cap];
        for (size_t i = 0; i < len; ++i) nd[i] = data[i];
        delete[] data;
        data = nd;
    }
    for (size_t i = len; i > idx; --i) data[i] = data[i - 1];
    data[idx] = v;
    ++len;
}

void Array::push(const string& v) { insert(len, v); }

const string& Array::get(size_t i) const { check(i); return data[i]; }

void Array::set(size_t i, const string& v) { check(i); data[i] = v; }

string Array::erase(size_t i) {
    check(i);
    string r = data[i];
    for (size_t j = i; j + 1 < len; ++j) data[j] = data[j + 1];
    --len;
    return r;
}

Strs Array::dump() const { return Strs(data, data + len); }

void Array::load(const Strs& t) { for (auto& s : t) push(s); }

string Array::show() const { return "[" + join(dump(), ", ") + "] (len=" + to_string(len) + ")"; }
