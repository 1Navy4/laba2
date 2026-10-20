#include "hashtable.h"

#include <cstdio>
#include <utility>
#include <vector>

#include "hashfn.h"

using namespace std;

static const char SEP = '\x1F';           // разделяет ключ и значение внутри ячейки
static const string DELETED = "\x01";     // метка удаленной ячейки
static const size_t NONE = (size_t)-1;

static bool isDeleted(const string& s) { return s == DELETED; }

// Ячейка s хранит ключ key? Сравнение без создания временных строк.
static bool slotHasKey(const string& s, const string& key) {
    return s.size() > key.size() && s[key.size()] == SEP && s.compare(0, key.size(), key) == 0;
}

static bool isPrime(size_t n) {
    if (n < 2) return false;
    for (size_t d = 2; d * d <= n; ++d) if (n % d == 0) return false;
    return true;
}

static size_t nextPrime(size_t n) {
    while (!isPrime(n)) ++n;
    return n;
}

void HashTable::init(Method m, size_t capacity) {
    method = m;
    cap = nextPrime(capacity < 3 ? 3 : capacity);
    while (slots.size() > 0) slots.erase(slots.size() - 1);  
    for (size_t i = 0; i < cap; ++i) slots.push("");
    count = 0;
    tombs = 0;
    bloom.reset(cap * 10);                            
}

HashTable::HashTable(Method m, size_t capacity, bool grow, bool bloomOn)
    : method(m), cap(0), autoGrow(grow), useBloom(bloomOn), bloom(64, 4) {
    init(m, capacity);
}

size_t HashTable::home(const string& key) const { return hashDjb2(key) % cap; }

size_t HashTable::stepOf(const string& key) const {
    if (method == LINEAR) return 1;
    return 1 + hashFnv1a(key) % (cap - 1);              
}

size_t HashTable::findIndex(const string& key) {
    size_t idx = home(key), step = stepOf(key);
    for (size_t n = 0; n < cap; ++n) {
        ++probes;
        const string& s = slots.get(idx);
        if (s.empty()) return NONE;                   
        if (!isDeleted(s) && slotHasKey(s, key)) return idx; 
        idx = (idx + step) % cap;
    }
    return NONE;
}

void HashTable::insertFresh(const string& key, const string& value) {  // ключа точно нет, роста нет
    size_t idx = home(key), step = stepOf(key);
    for (size_t n = 0; n < cap; ++n) {
        ++probes;
        const string& s = slots.get(idx);
        if (s.empty() || isDeleted(s)) {
            if (isDeleted(s)) --tombs;
            slots.set(idx, key + SEP + value);
            ++count;
            bloom.add(key);
            return;
        }
        idx = (idx + step) % cap;
    }
    fail("hash table is full");
}

void HashTable::rehash(size_t newCap) {
    vector<pair<string, string>> live;
    for (size_t i = 0; i < cap; ++i) {
        const string& s = slots.get(i);
        if (s.empty() || isDeleted(s)) continue;
        size_t p = s.find(SEP);
        live.push_back({s.substr(0, p), s.substr(p + 1)});
    }
    init(method, newCap);                                    
    for (auto& kv : live) insertFresh(kv.first, kv.second);  
}

bool HashTable::put(const string& key, const string& value) {
    if (key.find(SEP) != string::npos) fail("key must not contain the character 0x1F");
    if (autoGrow && (count + tombs + 1) * 10 > cap * 7)     
        rehash((count + 1) * 2 > cap ? cap * 2 : cap);        // живых много - удвоить, иначе только убрать надгробия
    size_t idx = home(key), step = stepOf(key), firstFree = NONE;
    for (size_t n = 0; n < cap; ++n) {
        ++probes;
        const string& s = slots.get(idx);
        if (s.empty()) { if (firstFree == NONE) firstFree = idx; break; }   
        if (isDeleted(s)) {
            if (firstFree == NONE) firstFree = idx;                        
        } else if (slotHasKey(s, key)) {
            slots.set(idx, key + SEP + value);                             
            return false;
        }
        idx = (idx + step) % cap;
    }
    if (firstFree == NONE) fail("hash table is full");
    if (isDeleted(slots.get(firstFree))) --tombs;
    slots.set(firstFree, key + SEP + value);
    ++count;
    bloom.add(key);
    return true;
}

bool HashTable::get(const string& key, string& value) {
    if (useBloom && !bloom.mightContain(key)) { ++bloomRejects; return false; }   // фильтр: точно нет
    size_t idx = findIndex(key);
    if (idx == NONE) {
        if (useBloom) ++bloomFalse;                                           
        return false;
    }
    const string& s = slots.get(idx);
    value = s.substr(s.find(SEP) + 1);
    return true;
}

bool HashTable::contains(const string& key) {
    string v;
    return get(key, v);
}

bool HashTable::erase(const string& key) {
    size_t idx = findIndex(key);
    if (idx == NONE) return false;
    slots.set(idx, DELETED);      
    --count;
    ++tombs;
    return true;
}

// Формат: метод, размер, число живых, число надгробий, затем тройки (индекс ключ значение), затем индексы надгробий.
Strs HashTable::dump() const {
    Strs live, tomb;
    for (size_t i = 0; i < cap; ++i) {
        const string& s = slots.get(i);
        if (s.empty()) continue;
        if (isDeleted(s)) { tomb.push_back(to_string(i)); continue; }
        size_t p = s.find(SEP);
        live.push_back(to_string(i));
        live.push_back(s.substr(0, p));
        live.push_back(s.substr(p + 1));
    }
    Strs r{method == LINEAR ? "linear" : "double", to_string(cap), to_string(count), to_string(tombs)};
    r.insert(r.end(), live.begin(), live.end());
    r.insert(r.end(), tomb.begin(), tomb.end());
    return r;
}

void HashTable::load(const Strs& t) {
    if (t.size() < 4 || (t[0] != "linear" && t[0] != "double")) fail("corrupted data file");
    size_t c = stoul(t[1]), nLive = stoul(t[2]), nTomb = stoul(t[3]);
    if (t.size() != 4 + 3 * nLive + nTomb || !isPrime(c)) fail("corrupted data file");
    init(t[0] == "linear" ? LINEAR : DOUBLE, c);
    for (size_t k = 0; k < nLive; ++k) {
        size_t idx = stoul(t[4 + 3 * k]);
        if (idx >= cap) fail("corrupted data file");
        slots.set(idx, t[5 + 3 * k] + SEP + t[6 + 3 * k]);
        bloom.add(t[5 + 3 * k]);
        ++count;
    }
    for (size_t k = 0; k < nTomb; ++k) {
        size_t idx = stoul(t[4 + 3 * nLive + k]);
        if (idx >= cap) fail("corrupted data file");
        slots.set(idx, DELETED);
        ++tombs;
    }
}

string HashTable::show() const {
    string r = stats();
    for (size_t i = 0; i < cap; ++i) {
        const string& s = slots.get(i);
        if (s.empty()) continue;
        if (isDeleted(s)) { r += "\n[" + to_string(i) + "] (deleted)"; continue; }
        size_t p = s.find(SEP);
        r += "\n[" + to_string(i) + "] " + quote(s.substr(0, p)) + " => " + quote(s.substr(p + 1));
    }
    return r;
}

string HashTable::stats() const {
    size_t set = bloom.setBits();
    double fill = (double)set / bloom.bits();
    double fp = 1;
    for (int i = 0; i < bloom.hashes(); ++i) fp *= fill;      // оценка ложных срабатываний: (доля битов)^k
    auto pct = [](double x) {                                 // процент: целый, а для малых значений с долями
        char buf[32];
        snprintf(buf, sizeof buf, x < 0.1 ? "%.2f%%" : "%.0f%%", x * 100);
        return string(buf);
    };
    return string("method: ") + (method == LINEAR ? "linear probing" : "double hashing") +
           ", capacity: " + to_string(cap) + ", size: " + to_string(count) + ", deleted: " + to_string(tombs) +
           ", load: " + pct((double)count / cap) + "\nbloom: " + to_string(bloom.bits()) + " bits, " +
           to_string(bloom.hashes()) + " hashes, bits set: " + pct(fill) + ", false positive estimate: " + pct(fp);
}
