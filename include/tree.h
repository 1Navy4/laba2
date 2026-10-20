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

    std::string insert(const std::string& k);       
    bool contains(const std::string& k) const;   
    bool isComplete() const;                    
    Strs traverse(const std::string& order) const; 

    Strs dump() const;
    void load(const Strs& t);
    std::string show() const;
};
