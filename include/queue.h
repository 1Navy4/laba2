#pragma once
#include "slist.h"

struct Queue {
private:
    SList l;
public:
    void push(const std::string& v);  
    std::string pop();               
    Strs items() const;               

    Strs dump() const { return l.items(); }
    void load(const Strs& t);
    std::string show() const;
};
