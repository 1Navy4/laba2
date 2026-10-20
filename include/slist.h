#pragma once
#include "common.h"

struct SNode { std::string val; SNode* next; };

struct SList {
private:
    SNode *head = nullptr, *tail = nullptr;
    size_t n = 0;
    SNode* findNode(const std::string& v) const;
    SNode* mustFind(const std::string& v) const;
public:
    SList() {}
    SList(const SList&) = delete;
    SList& operator=(const SList&) = delete;
    ~SList();

    size_t size() const { return n; }
    void pushHead(const std::string& v);                                 
    void pushAfter(const std::string& target, const std::string& v);    
    void pushBefore(const std::string& target, const std::string& v); 
    std::string popHead();  
    std::string popTail();  
    std::string delAfter(const std::string& target);   
    std::string delBefore(const std::string& target);    
    bool delValue(const std::string& v);                  
    int find(const std::string& v) const;     
    const std::string& at(size_t i) const;  
    Strs items() const;

    Strs dump() const { return items(); }
    void load(const Strs& t);
    std::string show() const;
};
