// Фильтр Блума: битовый массив и k хеш-функций.
// add(key) выставляет k битов. mightContain(key): false - ключа точно нет (ошибок нет),
// true - ключ, возможно, есть (бывают ложные срабатывания). Удаление не поддерживается:
// фильтр пересобирается вместе с таблицей. Операции O(k), то есть O(1).
#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct BloomFilter {
private:
    std::vector<std::uint64_t> words;
    size_t m;      // число бит
    int k;         // число хеш-функций
    size_t bitIndex(const std::string& key, int i) const;
public:
    explicit BloomFilter(size_t bits = 64, int hashes = 4);
    void reset(size_t bits);                           // новый размер, все биты сброшены
    void add(const std::string& key);
    bool mightContain(const std::string& key) const;
    size_t bits() const { return m; }
    int hashes() const { return k; }
    size_t setBits() const;                            // сколько битов выставлено
};
