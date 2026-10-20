#include "lru.h"

using namespace std;

static const char SEP = '\x1F';   // разделяет номер узла и значение в записи индекса

LRUCache::LRUCache(size_t cap) : capacity(cap), index(DOUBLE, 11, true, true) {}

size_t LRUCache::allocId(DNode* node) {
    if (!freeIds.empty()) {
        size_t id = freeIds.back();
        freeIds.pop_back();
        nodes[id] = node;
        return id;
    }
    nodes.push_back(node);
    return nodes.size() - 1;
}

void LRUCache::set(const string& key, const string& value) {
    if (capacity == 0) return;
    string entry;
    if (index.get(key, entry)) {                      // ключ уже есть: перезаписать значение
        size_t id = stoul(entry.substr(0, entry.find(SEP)));
        index.put(key, to_string(id) + SEP + value);
        order.moveToHead(nodes[id]);                  // и сделать самым недавним
        return;
    }
    if (index.size() == capacity) {                   // кэш полон убрать элемент, который дольше всех не запрашивался
        string oldKey = order.tailNode()->val;
        string e;
        index.get(oldKey, e);
        freeIds.push_back(stoul(e.substr(0, e.find(SEP))));
        index.erase(oldKey);
        order.popTail();
    }
    DNode* node = order.pushHeadNode(key);            // новый элемент - самый недавний
    index.put(key, to_string(allocId(node)) + SEP + value);
}

bool LRUCache::get(const string& key, string& value) {
    string entry;
    if (!index.get(key, entry)) return false;
    size_t p = entry.find(SEP);
    order.moveToHead(nodes[stoul(entry.substr(0, p))]);   // обращение делает ключ самым недавним
    value = entry.substr(p + 1);
    return true;
}

string LRUCache::show() {
    Strs parts;
    for (const string& key : order.itemsReversed()) {     // от хвоста (давний) к голове (недавний)
        string e;
        index.get(key, e);
        parts.push_back(key + " -> " + e.substr(e.find(SEP) + 1));
    }
    return parts.empty() ? "(empty)" : join(parts, ", ");
}
