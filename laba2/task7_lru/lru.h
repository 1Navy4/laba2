// LRU-кэш (Least Recently Used). Строится на структурах из предыдущих работ:
//   DList (ЛР1)     - порядок использования ключей: голова - использован последним, хвост - давнее всех;
//   HashTable (з.6) - быстрый поиск по ключу: ключ -> "номер узла списка" и значение.
// Благодаря этому SET и GET выполняются за O(1) (в среднем).
#pragma once
#include <string>
#include <vector>

#include "dlist.h"
#include "hashtable.h"

struct LRUCache {
private:
    size_t capacity;
    DList order;
    HashTable index;
    std::vector<DNode*> nodes;     // номер узла -> узел списка
    std::vector<size_t> freeIds;   // освободившиеся номера
    size_t allocId(DNode* node);
public:
    explicit LRUCache(size_t cap);   // емкость кэша задается в конструкторе
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    void set(const std::string& key, const std::string& value);   // SET x y
    bool get(const std::string& key, std::string& value);         // GET x: false, если ключа нет (выводится -1)
    size_t size() const { return index.size(); }
    std::string show();   // содержимое от самого давнего к самому недавнему: "2 -> 3, 1 -> 5"
};
