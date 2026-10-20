#pragma once
#include <vector>

#include "common.h"
#include "slist.h"

struct HashSet {
private:
    std::vector<SList*> buckets;
    size_t count = 0;
    static size_t hashOf(const std::string& v);
    void grow();
public:
    HashSet();
    HashSet(const HashSet&) = delete;
    HashSet& operator=(const HashSet&) = delete;
    ~HashSet();

    bool add(const std::string& v);              // true - добавлен, false - уже был
    bool remove(const std::string& v);           // true - удален, false - не было
    bool contains(const std::string& v) const;   // принадлежит ли множеству
    size_t size() const { return count; }
    Strs items() const;                          // элементы по возрастанию (для вывода и файла)

    Strs dump() const { return items(); }
    void load(const Strs& t);
    std::string show() const;
};
