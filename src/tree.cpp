#include "tree.h"

#include <algorithm>

using namespace std;

void CBTree::destroy(TNode* x) {
    if (x) { destroy(x->left); destroy(x->right); delete x; }
}

vector<TNode*> CBTree::nodes() const {  // сам вектор служит очередью
    vector<TNode*> o;
    if (root) o.push_back(root);
    for (size_t i = 0; i < o.size(); ++i) {
        if (o[i]->left) o.push_back(o[i]->left);
        if (o[i]->right) o.push_back(o[i]->right);
    }
    return o;
}

void CBTree::preorder(TNode* x, Strs& o) const {  // прямой: узел, левое, правое
    if (!x) return;
    o.push_back(x->key);
    preorder(x->left, o);
    preorder(x->right, o);
}

void CBTree::inorder(TNode* x, Strs& o) const {  // симметричный: левое, узел, правое
    if (!x) return;
    inorder(x->left, o);
    o.push_back(x->key);
    inorder(x->right, o);
}

void CBTree::postorder(TNode* x, Strs& o) const {  // обратный: левое, правое, узел
    if (!x) return;
    postorder(x->left, o);
    postorder(x->right, o);
    o.push_back(x->key);
}

void CBTree::dumpRec(TNode* x, Strs& o) const {
    o.push_back(x ? x->key : "#");
    if (x) { dumpRec(x->left, o); dumpRec(x->right, o); }
}

TNode* CBTree::build(const Strs& t, size_t& i) {
    if (i >= t.size()) return nullptr;
    string key = t[i];
    ++i;
    if (key == "#") return nullptr;
    TNode* x = new TNode{normInt(key), nullptr, nullptr};  // в дереве только числа
    x->left = build(t, i);
    x->right = build(t, i);
    return x;
}

string CBTree::insert(const string& k) {
    TNode* z = new TNode{normInt(k), nullptr, nullptr};  // не число -> ошибка до изменения дерева
    string result = z->key;
    if (!root) { root = z; return result; }
    for (TNode* x : nodes()) {  
        if (!x->left) { x->left = z; return result; }
        if (!x->right) { x->right = z; return result; }
    }
    return result;
}

bool CBTree::contains(const string& k) const {  // O(n) - дерево не упорядочено, смотрим все узлы
    string key = normInt(k);
    for (TNode* x : nodes()) if (x->key == key) return true;
    return false;
}

// Проверка Complete: обход в ширину вместе с пустыми местами (nullptr).Если после первого пустого места встретился настоящий узел - дерево не complete.
bool CBTree::isComplete() const {
    vector<TNode*> o{root};
    bool gap = false;
    for (size_t i = 0; i < o.size(); ++i) {
        if (!o[i]) { gap = true; continue; }
        if (gap) return false;
        o.push_back(o[i]->left);
        o.push_back(o[i]->right);
    }
    return true;
}

Strs CBTree::traverse(const string& order) const {
    Strs o;
    if (order == "PREORDER") preorder(root, o);
    else if (order == "INORDER") inorder(root, o);
    else if (order == "POSTORDER") postorder(root, o);
    else if (order == "SORTED") {  
        for (TNode* x : nodes()) o.push_back(x->key);
        sort(o.begin(), o.end(), [](const string& a, const string& b) { return stoll(a) < stoll(b); });
    }
    else if (order == "BFS") for (TNode* x : nodes()) o.push_back(x->key);
    else fail("unknown order: " + order + " (inorder, preorder, postorder, bfs, sorted)");
    return o;
}

Strs CBTree::dump() const { Strs o; dumpRec(root, o); return o; }

void CBTree::load(const Strs& t) { destroy(root); root = nullptr; size_t i = 0; root = build(t, i); }

namespace {

struct Block {
    std::vector<std::u32string> rows;
    int width;  
    int root;   
};

std::u32string spaces(int n) { return std::u32string(n < 0 ? 0 : n, U' '); }

std::u32string fit(const std::u32string& s, int w) {  // дополнить пробелами до ширины w
    std::u32string r = s;
    r.resize(w, U' ');
    return r;
}

std::string toUtf8(const std::u32string& s) {  
    std::string r;
    for (char32_t c : s) {
        if (c < 0x80) r += (char)c;
        else if (c < 0x800) { r += (char)(0xC0 | (c >> 6)); r += (char)(0x80 | (c & 0x3F)); }
        else { r += (char)(0xE0 | (c >> 12)); r += (char)(0x80 | ((c >> 6) & 0x3F)); r += (char)(0x80 | (c & 0x3F)); }
    }
    return r;
}

Block buildBlock(TNode* x) {
    std::u32string label(x->key.begin(), x->key.end());  // в дереве только цифры и минус
    int n = (int)label.size();
    if (!x->left && !x->right) return Block{{label}, n, (n - 1) / 2};

    const int gap = 3;  // пустое место между блоками детей
    Block L{{}, 0, 0}, R{{}, 0, 0};
    bool hl = x->left != nullptr, hr = x->right != nullptr;
    if (hl) L = buildBlock(x->left);
    if (hr) R = buildBlock(x->right);

    int rOff = hl ? L.width + gap : 0;  // где начинается блок правого ребенка
    int lr = L.root, rr = rOff + R.root;
    int c;                              // колонка родителя
    if (hl && hr) c = (lr + rr) / 2;
    else if (hl) c = lr + 2;            // один левый ребенок: родитель правее и выше
    else c = rr - 2;                    // один правый ребенок: родитель левее и выше
    int start = c - (n - 1) / 2;        // первая буква подписи родителя
    int shift = start < 0 ? -start : 0; // если вылезли влево - сдвигаем все вправо
    lr += shift; rr += shift; c += shift; start += shift;

    int childW = hl && hr ? L.width + gap + R.width : (hl ? L.width : R.width);
    int width = std::max(shift + childW, start + n);

    Block res{{}, width, c};
    std::u32string top = spaces(width);
    top.replace(start, n, label);
    res.rows.push_back(top);

    std::u32string bar = spaces(width);  // перекладина между родителем и детьми
    int from = hl ? lr : c, to = hr ? rr : c;
    for (int i = from; i <= to; ++i) bar[i] = U'\u2500';  // ─
    if (hl) bar[lr] = U'\u250C';                           // ┌
    if (hr) bar[rr] = U'\u2510';                           // ┐
    bar[c] = hl && hr ? U'\u2534' : (hl ? U'\u2518' : U'\u2514');  // ┴ или ┘ или └
    res.rows.push_back(bar);

    size_t h = std::max(L.rows.size(), R.rows.size());
    for (size_t i = 0; i < h; ++i) {
        std::u32string line = spaces(shift);
        if (hl) line += fit(i < L.rows.size() ? L.rows[i] : U"", L.width);
        if (hl && hr) line += spaces(gap);
        if (hr) line += fit(i < R.rows.size() ? R.rows[i] : U"", R.width);
        res.rows.push_back(fit(line, width));
    }
    return res;
}

} 

string CBTree::show() const {
    if (!root) return "(empty tree)";
    Block b = buildBlock(root);
    string out;
    for (auto& r : b.rows) {
        size_t e = r.find_last_not_of(U' ');
        out = out + toUtf8(r.substr(0, e + 1)) + "\n";
    }
    out.pop_back();
    return out;
}
