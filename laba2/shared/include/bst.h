// Бинарное дерево поиска для целых чисел. Узел - TNode из дерева ЛР1 (tree.h).
// Слева от узла значения меньше, справа - больше. Повторы не добавляются. Без балансировки.
#pragma once
#include "tree.h"

struct BST {
private:
    TNode* root = nullptr;
public:
    BST() {}
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;
    ~BST();

    // Добавляет число. Возвращает его глубину (корень - 1) или 0, если число уже есть в дереве.
    // O(h), где h - высота дерева: в среднем O(log n), в худшем O(n).
    int insert(long long v);
};
