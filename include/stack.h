#pragma once
#include "slist.h"

struct Stack {
private:
    SList l;
public:
    void push(const std::string& v);  
    std::string pop();                
    Strs items() const;             
    bool empty() const { return l.size() == 0; }     // стек пуст?
    std::string top() const { return l.at(0); }      //  вершина без удаления, O(1)

    Strs dump() const;        // от низа к вершине
    void load(const Strs& t);
    std::string show() const;
};
