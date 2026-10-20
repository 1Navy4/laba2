// Односвязный список (F) строк.
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
    void pushHead(const std::string& v);                                  // O(1)
    void pushTail(const std::string& v);                                  // O(1)
    void pushAfter(const std::string& target, const std::string& v);      // O(n)
    void pushBefore(const std::string& target, const std::string& v);     // O(n)
    std::string popHead();                                                // O(1)
    std::string popTail();                                                // O(n)
    std::string delAfter(const std::string& target);                      // O(n)
    std::string delBefore(const std::string& target);                     // O(n)
    bool delValue(const std::string& v);                                  // O(n)
    int find(const std::string& v) const;                                 // O(n)
    const std::string& at(size_t i) const;                                // O(n)
    Strs items() const;

    Strs dump() const { return items(); }
    void load(const Strs& t);
    std::string show() const;
};
