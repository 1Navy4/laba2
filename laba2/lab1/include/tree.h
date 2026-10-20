// Complete Binary Tree (T). Хранит только целые числа (в том числе отрицательные).
//
// Все уровни заполнены полностью, кроме, возможно, последнего; последний заполняется
// слева направо без пропусков. Вставка кладет узел на первое свободное место при обходе
// в ширину, поэтому дерево из одних вставок всегда complete. В файле дерево хранится как
// прямой обход с '#' на месте пустых мест, так что там можно записать и НЕполное дерево.
#pragma once
#include <vector>

#include "common.h"

struct TNode { std::string key; TNode *left, *right; };

struct CBTree {
private:
    TNode* root = nullptr;
    static void destroy(TNode* x);
    std::vector<TNode*> nodes() const;  // все узлы в порядке обхода в ширину
    void preorder(TNode* x, Strs& o) const;
    void inorder(TNode* x, Strs& o) const;
    void postorder(TNode* x, Strs& o) const;
    void dumpRec(TNode* x, Strs& o) const;
    TNode* build(const Strs& t, size_t& i);
public:
    CBTree() {}
    CBTree(const CBTree&) = delete;
    CBTree& operator=(const CBTree&) = delete;
    ~CBTree() { destroy(root); }

    std::string insert(const std::string& k);        // O(n); возвращает сохраненное число
    bool contains(const std::string& k) const;       // O(n) - дерево не упорядочено
    bool isComplete() const;                         // O(n)
    Strs traverse(const std::string& order) const;   // PREORDER, INORDER, POSTORDER, BFS

    Strs dump() const;
    void load(const Strs& t);
    std::string show() const;
};
