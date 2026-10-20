#include "dlist.h"

using namespace std;

DNode* DList::findNode(const string& v) const {
    for (DNode* c = head; c; c = c->next) if (c->val == v) return c;
    return nullptr;
}

DNode* DList::mustFind(const string& v) const {
    DNode* t = findNode(v);
    if (!t) fail("value not found: " + v);
    return t;
}

string DList::unlink(DNode* x) { 
    string r = x->val;
    if (x->prev) x->prev->next = x->next; else head = x->next;
    if (x->next) x->next->prev = x->prev; else tail = x->prev;
    delete x;
    --n;
    return r;
}

void DList::insertAfter(DNode* t, const string& v) {
    DNode* x = new DNode{v, t, t->next};
    if (t->next) t->next->prev = x; else tail = x;
    t->next = x;
    ++n;
}

DList::~DList() { while (head) popHead(); }

void DList::pushHead(const string& v) {
    DNode* x = new DNode{v, nullptr, head};
    if (head) head->prev = x; else tail = x;
    head = x;
    ++n;
}

void DList::pushTail(const string& v) {
    DNode* x = new DNode{v, tail, nullptr};
    if (tail) tail->next = x; else head = x;
    tail = x;
    ++n;
}

void DList::pushAfter(const string& target, const string& v) { insertAfter(mustFind(target), v); }

void DList::pushBefore(const string& target, const string& v) {
    DNode* t = mustFind(target);
    if (t == head) pushHead(v); else insertAfter(t->prev, v);
}

string DList::popHead() { if (!head) fail("list is empty"); return unlink(head); }

string DList::popTail() { if (!head) fail("list is empty"); return unlink(tail); }

string DList::delAfter(const string& target) {
    DNode* t = mustFind(target);
    if (!t->next) fail("no element after " + target);
    return unlink(t->next);
}

string DList::delBefore(const string& target) {
    DNode* t = mustFind(target);
    if (!t->prev) fail("no element before " + target);
    return unlink(t->prev);
}

bool DList::delValue(const string& v) {
    DNode* x = findNode(v);
    if (x) unlink(x);
    return x != nullptr;
}

int DList::find(const string& v) const {
    int i = 0;
    for (DNode* c = head; c; c = c->next, ++i) if (c->val == v) return i;
    return -1;
}

const string& DList::at(size_t i) const {
    if (i >= n) fail("index out of range: " + to_string(i));
    DNode* c = head;
    while (i--) c = c->next;
    return c->val;
}

Strs DList::items() const {
    Strs r;
    for (DNode* c = head; c; c = c->next) r.push_back(c->val);
    return r;
}

Strs DList::itemsReversed() const {
    Strs r;
    for (DNode* c = tail; c; c = c->prev) r.push_back(c->val);
    return r;
}

void DList::load(const Strs& t) { for (auto& s : t) pushTail(s); }

string DList::show() const {
    return head ? "NULL <-> " + join(items(), " <-> ") + " <-> NULL" : "NULL";
}

// ---- добавлено в ЛР2 (нужно для LRU-кэша) ----

DNode* DList::pushHeadNode(const string& v) {
    pushHead(v);
    return head;
}

void DList::moveToHead(DNode* x) {
    if (x == head) return;
    x->prev->next = x->next;                    
    if (x->next) x->next->prev = x->prev; else tail = x->prev;
    x->prev = nullptr;                          
    x->next = head;
    head->prev = x;
    head = x;
}
