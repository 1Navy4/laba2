// Задание 4 (вариант 5). Контрольная по ударениям. Массив из ЛР1 и множество из задания 2.
// Ввод (stdin или --file путь): N, затем N слов словаря (заглавная буква - ударение),
// затем строка текста с ударениями, расставленными Петей.
// Вывод: количество ошибок, найденных Васей.
//
// Правила: слово есть в словаре (без учета регистра) - оно должно совпасть с одним из
// вариантов ударения из словаря; слова нет в словаре - оно верно, только если в нем
// ровно одна заглавная буква.
//
// Алгоритм: словарь кладем в два множества - варианты как записаны и слова в нижнем регистре.
// Для каждого слова текста проверка за O(1), всего O(N + длина текста).
#include <iostream>
#include <sstream>

#include "hashset.h"
#include "io.h"
#include "massiv.h"

using namespace std;

static string lower(string s) {
    for (auto& c : s) if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
    return s;
}

static int upperCount(const string& s) {
    int n = 0;
    for (char c : s) if (c >= 'A' && c <= 'Z') ++n;
    return n;
}

int main(int argc, char** argv) {
    istringstream in(readInput(argc, argv));
    long long n;
    if (!(in >> n) || n < 0) {
        cerr << "ERROR: в первой строке должно быть число слов словаря N" << endl;
        return 1;
    }

    Array dict;                                   // слова словаря
    for (long long i = 0; i < n; ++i) {
        string w;
        if (!(in >> w)) { cerr << "ERROR: в словаре меньше слов, чем N" << endl; return 1; }
        dict.push(w);
    }
    Array text;                                   // слова текста Пети
    for (string w; in >> w;) text.push(w);

    HashSet variants, words;
    for (size_t i = 0; i < dict.size(); ++i) {
        variants.add(dict.get(i));                // как записано: cAnnot
        words.add(lower(dict.get(i)));            // без ударений: cannot
    }

    long long errors = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        const string& w = text.get(i);
        if (words.contains(lower(w))) {
            if (!variants.contains(w)) ++errors;  // слово из словаря, но ударение не из списка
        } else if (upperCount(w) != 1) {
            ++errors;                             // слова нет в словаре: нужно ровно одно ударение
        }
    }
    cout << errors << endl;
}
