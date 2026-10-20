// Стек (S) строк на основе односвязного списка (вершина - голова списка).
#pragma once
#include "slist.h"

struct Stack {
private:
    SList l;
public:
    void push(const std::string& v);   // O(1)
    std::string pop();                 // O(1)
    Strs items() const;                // от вершины к низу
    bool empty() const { return l.size() == 0; }     // добавлено в ЛР2: стек пуст?
    std::string top() const { return l.at(0); }      // добавлено в ЛР2: вершина без удаления, O(1)

    Strs dump() const;        // от низа к вершине
    void load(const Strs& t);
    std::string show() const;
};
