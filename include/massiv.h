#pragma once
#include "common.h"

struct Array {
private:
    std::string* data = new std::string[4];
    size_t len = 0, cap = 4;
    void check(size_t i) const;
public:
    Array() {}
    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;
    ~Array() { delete[] data; }

    size_t size() const { return len; }
    void insert(size_t idx, const std::string& v);   
    void push(const std::string& v);                
    const std::string& get(size_t i) const;       
    void set(size_t i, const std::string& v);      
    std::string erase(size_t i);                  

    Strs dump() const;
    void load(const Strs& t);
    std::string show() const;
};
