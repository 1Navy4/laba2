#include "slist.h"

using namespace std;

SNode* SList::findNode(const string& v) const {
    for (SNode* c = head; c; c = c->next) if (c->val == v) return c;
    return nullptr;
}

SNode* SList::mustFind(const string& v) const {
    SNode* t = findNode(v);
    if (!t) fail("value not found: " + v);
    return t;
}

SList::~SList() { while (head) popHead(); }

void SList::pushHead(const string& v) {
    head = new SNode{v, head};
    if (!tail) tail = head;
    ++n;
}

void SList::pushTail(const string& v) {
    SNode* x = new SNode{v, nullptr};
    if (tail) tail->next = x; else head = x;
    tail = x;
    ++n;
}

void SList::pushAfter(const string& target, const string& v) {
    SNode* t = mustFind(target);
    t->next = new SNode{v, t->next};
    if (t == tail) tail = t->next;
    ++n;
}

void SList::pushBefore(const string& target, const string& v) {
    mustFind(target);
    if (head->val == target) return pushHead(v);
    SNode* p = head;
    while (p->next->val != target) p = p->next;  // p - узел перед целевым
    p->next = new SNode{v, p->next};
    ++n;
}

string SList::popHead() {
    if (!head) fail("list is empty");
    SNode* t = head;
    string r = t->val;
    head = t->next;
    if (!head) tail = nullptr;
    delete t;
    --n;
    return r;
}

string SList::popTail() {  // нужно найти предпоследний узел
    if (!head) fail("list is empty");
    if (head == tail) return popHead();
    SNode* p = head;
    while (p->next != tail) p = p->next;
    string r = tail->val;
    delete tail;
    tail = p;
    p->next = nullptr;
    --n;
    return r;
}

string SList::delAfter(const string& target) {
    SNode* t = mustFind(target);
    if (!t->next) fail("no element after " + target);
    SNode* x = t->next;
    string r = x->val;
    t->next = x->next;
    if (x == tail) tail = t;
    delete x;
    --n;
    return r;
}

string SList::delBefore(const string& target) {
    SNode* t = mustFind(target);
    if (t == head) fail("no element before " + target);
    if (head->next == t) return popHead();
    SNode* p = head;
    while (p->next->next != t) p = p->next;  // p - узел за два до целевого
    SNode* x = p->next;
    string r = x->val;
    p->next = t;
    delete x;
    --n;
    return r;
}

bool SList::delValue(const string& v) {
    if (!findNode(v)) return false;
    if (head->val == v) { popHead(); return true; }
    SNode* p = head;
    while (p->next->val != v) p = p->next;
    SNode* x = p->next;
    p->next = x->next;
    if (x == tail) tail = p;
    delete x;
    --n;
    return true;
}

int SList::find(const string& v) const {
    int i = 0;
    for (SNode* c = head; c; c = c->next, ++i) if (c->val == v) return i;
    return -1;
}

const string& SList::at(size_t i) const {
    if (i >= n) fail("index out of range: " + to_string(i));
    SNode* c = head;
    while (i--) c = c->next;
    return c->val;
}

Strs SList::items() const {
    Strs r;
    for (SNode* c = head; c; c = c->next) r.push_back(c->val);
    return r;
}

void SList::load(const Strs& t) { for (auto& s : t) pushTail(s); }

string SList::show() const { return head ? join(items(), " -> ") + " -> NULL" : "NULL"; }
