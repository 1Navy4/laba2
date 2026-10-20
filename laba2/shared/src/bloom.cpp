#include "bloom.h"

#include "hashfn.h"

using namespace std;

BloomFilter::BloomFilter(size_t bits, int hashes) : m(bits), k(hashes) { reset(bits); }

void BloomFilter::reset(size_t bits) {
    m = bits < 64 ? 64 : bits;
    words.assign((m + 63) / 64, 0);
}

// i-я хеш-функция строится из двух базовых: g_i = h1 + i * h2 (двойное хеширование).
size_t BloomFilter::bitIndex(const string& key, int i) const {
    return (hashDjb2(key) + (uint64_t)i * (hashFnv1a(key) | 1)) % m;
}

void BloomFilter::add(const string& key) {
    for (int i = 0; i < k; ++i) {
        size_t b = bitIndex(key, i);
        words[b / 64] |= (uint64_t)1 << (b % 64);
    }
}

bool BloomFilter::mightContain(const string& key) const {
    for (int i = 0; i < k; ++i) {
        size_t b = bitIndex(key, i);
        if (!((words[b / 64] >> (b % 64)) & 1)) return false;   // хоть один бит не выставлен - ключа нет
    }
    return true;
}

size_t BloomFilter::setBits() const {
    size_t n = 0;
    for (uint64_t w : words) n += __builtin_popcountll(w);
    return n;
}
