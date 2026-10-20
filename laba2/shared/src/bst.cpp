#include "bst.h"

#include <vector>

using namespace std;

BST::~BST() {  // обход без рекурсии: дерево может выродиться в цепочку из тысяч узлов
    vector<TNode*> todo;
    if (root) todo.push_back(root);
    while (!todo.empty()) {
        TNode* x = todo.back();
        todo.pop_back();
        if (x->left) todo.push_back(x->left);
        if (x->right) todo.push_back(x->right);
        delete x;
    }
}

// Сравнение целых чисел, записанных строками в обычном виде (без ведущих нулей):
// -1 если a < b, 0 если равны, 1 если a > b. Работает за O(длина) без преобразования в число.
static int cmpInt(const string& a, const string& b) {
    bool na = a[0] == '-', nb = b[0] == '-';
    if (na != nb) return na ? -1 : 1;                    // отрицательное меньше неотрицательного
    int r;
    if (a.size() != b.size()) r = a.size() < b.size() ? -1 : 1;   // длиннее - больше по модулю
    else r = a == b ? 0 : (a < b ? -1 : 1);
    return na ? -r : r;                                  // у отрицательных порядок обратный
}

int BST::insert(long long v) {
    string key = to_string(v);
    if (!root) {
        root = new TNode{key, nullptr, nullptr};
        return 1;
    }
    int depth = 1;
    TNode* x = root;
    for (;;) {
        int c = cmpInt(key, x->key);
        if (c == 0) return 0;                            // уже есть - ничего не добавляем
        TNode*& next = c < 0 ? x->left : x->right;       // меньше - влево, больше - вправо
        ++depth;                                         // спустились на уровень ниже
        if (!next) {
            next = new TNode{key, nullptr, nullptr};
            return depth;
        }
        x = next;
    }
}
