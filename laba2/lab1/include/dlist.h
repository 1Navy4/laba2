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
    std::string unlink(DNode* x);               // O(1)
    void insertAfter(DNode* t, const std::string& v);
public:
    DList() {}
    DList(const DList&) = delete;
    DList& operator=(const DList&) = delete;
    ~DList();

    void pushHead(const std::string& v);                                  // O(1)
    void pushTail(const std::string& v);                                  // O(1)
    void pushAfter(const std::string& target, const std::string& v);      // O(n) - поиск
    void pushBefore(const std::string& target, const std::string& v);     // O(n) - поиск
    std::string popHead();                                                // O(1)
    std::string popTail();                                                // O(1)
    std::string delAfter(const std::string& target);
    std::string delBefore(const std::string& target);
    bool delValue(const std::string& v);
    int find(const std::string& v) const;
    const std::string& at(size_t i) const;
    Strs items() const;
    Strs itemsReversed() const;
    DNode* pushHeadNode(const std::string& v);   // добавлено в ЛР2: как pushHead, но возвращает созданный узел
    void moveToHead(DNode* x);                   // добавлено в ЛР2: перенести узел в голову, O(1)
    DNode* tailNode() const { return tail; }     // добавлено в ЛР2: последний узел

    Strs dump() const { return items(); }
    void load(const Strs& t);
    std::string show() const;
};
