#include "queue.h"

using namespace std;

void Queue::push(const string& v) { l.pushTail(v); }

string Queue::pop() {
    if (!l.size()) fail("queue is empty");
    return l.popHead();
}

Strs Queue::items() const { return l.items(); }

void Queue::load(const Strs& t) { for (auto& s : t) push(s); }

string Queue::show() const {
    return l.size() ? "front -> " + join(l.items(), ", ") + " <- back" : "(empty queue)";
}
