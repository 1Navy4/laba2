// Две независимые хеш-функции для строк. Метод деления (хеш % размер таблицы) применяется снаружи.
#pragma once
#include <cstdint>
#include <string>

inline std::uint64_t hashDjb2(const std::string& s) {
    std::uint64_t h = 5381;
    for (unsigned char c : s) h = h * 33 + c;
    return h;
}

inline std::uint64_t hashFnv1a(const std::string& s) {
    std::uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : s) { h ^= c; h *= 1099511628211ULL; }
    return h;
}
