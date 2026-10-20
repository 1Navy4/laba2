// Двусвязный список (L) строк.
#pragma once
#include "common.h"

struct DNode { std::string val; DNode *prev, *next; };

struct DList {
private:
    DNode *head = nullptr, *tail = nullptr;
    size_t n = 0;
    DNode* findNode(const std::string& v) const;
    DNode* mustFind(const std::string& v) const;
    std::string unlink(DNode* x);          
    void insertAfter(DNode* t, const std::string& v);
public:
    DList() {}
    DList(const DList&) = delete;
    DList& operator=(const DList&) = delete;
    ~DList();

    void pushHead(const std::string& v);                          
    void pushTail(const std::string& v);                                 
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
    Strs itemsReversed() const;
    DNode* pushHeadNode(const std::string& v);   // как pushHead, но возвращает созданный узел
    void moveToHead(DNode* x);                   //  перенести узел в голову, O(1)
    DNode* tailNode() const { return tail; }     // последний узел

    Strs dump() const { return items(); }
    void load(const Strs& t);
    std::string show() const;
};
