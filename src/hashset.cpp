#include "hashset.h"

#include <algorithm>

using namespace std;

size_t HashSet::hashOf(const string& v) { 
    size_t h = 5381;
    for (unsigned char c : v) h = h * 33 + c;
    return h;
}

HashSet::HashSet() {
    for (int i = 0; i < 8; ++i) buckets.push_back(new SList());
}

HashSet::~HashSet() { for (SList* b : buckets) delete b; }

void HashSet::grow() {  // удвоение числа корзин и перенос элементов
    vector<SList*> bigger;
    for (size_t i = 0; i < buckets.size() * 2; ++i) bigger.push_back(new SList());
    for (SList* b : buckets) {
        for (const string& s : b->items()) bigger[hashOf(s) % bigger.size()]->pushHead(s);
        delete b;
    }
    buckets = bigger;
}

bool HashSet::contains(const string& v) const {
    return buckets[hashOf(v) % buckets.size()]->find(v) >= 0;
}

bool HashSet::add(const string& v) {
    if (contains(v)) return false;
    if (count >= buckets.size()) grow();
    buckets[hashOf(v) % buckets.size()]->pushHead(v);
    ++count;
    return true;
}

bool HashSet::remove(const string& v) {
    bool removed = buckets[hashOf(v) % buckets.size()]->delValue(v);
    if (removed) --count;
    return removed;
}

Strs HashSet::items() const {
    Strs r;
    for (SList* b : buckets) for (const string& s : b->items()) r.push_back(s);
    sort(r.begin(), r.end());
    return r;
}

void HashSet::load(const Strs& t) { for (auto& s : t) add(s); }

string HashSet::show() const {
    Strs v = items();
    Strs q;
    for (auto& s : v) q.push_back(quote(s));
    return "{" + join(q, ", ") + "}";
}
