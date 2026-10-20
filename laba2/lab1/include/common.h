// Общие вспомогательные функции: ошибки, разбор запроса, строки, числа.
#pragma once
#include <cstddef>
#include <string>
#include <vector>

using Strs = std::vector<std::string>;

// Ошибки: fail бросает исключение, main его ловит и печатает "ERROR: ...".
[[noreturn]] void fail(const std::string& msg);

std::string join(const Strs& v, const std::string& sep);
std::string quote(const std::string& s);   // строка -> токен (в кавычках, если есть пробелы)
std::string spaced(const Strs& v);         // элементы через пробел, "(empty)" если пусто
std::string upper(std::string s);
bool endsWith(const std::string& s, const std::string& suf);

// Разбор строки на токены. Токен в двойных кавычках может содержать пробелы:
//   MPUSH arr "hello world"   ->   {"MPUSH", "arr", "hello world"}
// Внутри кавычек \" и \\ означают символы " и \.
Strs tokenize(const std::string& line);

const std::string& arg(const Strs& a, size_t i);  // a[i] или ошибка "missing argument"
size_t toIndex(const std::string& s);             // неотрицательный индекс

// Целые числа (в том числе отрицательные), для дерева.
bool isInteger(const std::string& s);
std::string normInt(const std::string& s);        // "-007" -> "-7"; ошибка, если не число
