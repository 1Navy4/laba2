// Хеш-таблица с открытой адресацией (строка -> строка) и встроенным фильтром Блума.
#pragma once
#include <string>

#include "bloom.h"
#include "massiv.h"

enum Method { LINEAR, DOUBLE };

struct HashTable {
private:
    Method method;
    Array slots;
    size_t cap;            // число ячеек (простое)
    size_t count = 0;      // живых элементов
    size_t tombs = 0;      // надгробий
    bool autoGrow;
    bool useBloom;
    BloomFilter bloom;

    size_t home(const std::string& key) const;
    size_t stepOf(const std::string& key) const;
    size_t findIndex(const std::string& key);                   // индекс ячейки
    void insertFresh(const std::string& key, const std::string& value);
    void rehash(size_t newCap);
    void init(Method m, size_t capacity);
public:
    long long probes = 0;        // сколько ячеек просмотрено (для эмпирического анализа)
    long long bloomRejects = 0;  // запросов, отсеянных фильтром без обращения к таблице
    long long bloomFalse = 0;    // фильтр сказал "возможно есть", а в таблице ключа нет

    HashTable(Method m, size_t capacity = 11, bool grow = true, bool bloomOn = true);
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    bool put(const std::string& key, const std::string& value);  // true - новый ключ, false - значение обновлено
    bool get(const std::string& key, std::string& value);        // найден ли ключ
    bool contains(const std::string& key);
    bool erase(const std::string& key);
    size_t size() const { return count; }
    size_t capacity() const { return cap; }
    Method methodOf() const { return method; }
    void resetCounters() { probes = bloomRejects = bloomFalse = 0; }

    Strs dump() const;                  // для записи в файл: метод, размер, ячейки с индексами
    void load(const Strs& t);           // восстановление в точности той же раскладки
    std::string show() const;           // все занятые ячейки
    std::string stats() const;          // параметры таблицы и фильтра
};
