// Очередь (Q) строк на основе односвязного списка (начало - голова, конец - хвост).
#pragma once
#include "slist.h"

struct Queue {
private:
    SList l;
public:
    void push(const std::string& v);   // O(1)
    std::string pop();                 // O(1)
    Strs items() const;                // от начала к концу

    Strs dump() const { return l.items(); }
    void load(const Strs& t);
    std::string show() const;
};
