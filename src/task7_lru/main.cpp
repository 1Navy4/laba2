// Вывод: результаты всех GET через пробел. Ключ "--trace" печатает состояние кэша после каждого запроса.
#include <iostream>
#include <sstream>

#include "io.h"
#include "lru.h"

using namespace std;

static string lower(string s) {
    for (auto& c : s) if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
    return s;
}

int main(int argc, char** argv) {
    bool trace = false;
    for (int i = 1; i < argc; ++i) if (string(argv[i]) == "--trace") trace = true;

    string text = readInput(argc, argv);
    for (char& c : text) if (c == ',' || c == '=') c = ' ';
    istringstream in(text);
    Strs t;
    for (string w; in >> w;) t.push_back(w);

    size_t i = 0;
    auto skipWord = [&](const string& name) { if (i < t.size() && lower(t[i]) == name) ++i; };
    auto number = [&]() -> size_t {
        if (i >= t.size() || t[i].empty() || t[i].find_first_not_of("0123456789") != string::npos) {
            cerr << "ERROR: ожидалось неотрицательное число (емкость и число запросов)" << endl;
            exit(1);
        }
        return stoul(t[i++]);
    };
    skipWord("cap");
    size_t cap = number();
    skipWord("q");
    size_t q = number();
    skipWord("queries");

    LRUCache cache(cap);
    Strs results;
    try {
        for (size_t n = 0; n < q; ++n) {
            if (i >= t.size()) fail("запросов меньше, чем Q");
            string cmd = upper(t[i++]);
            if (cmd == "SET") {
                if (i + 1 >= t.size()) fail("для SET нужны ключ и значение");
                string key = t[i], value = t[i + 1];
                i += 2;
                cache.set(key, value);
                if (trace) cout << "SET " << key << " " << value << ": " << cache.show() << endl;
            } else if (cmd == "GET") {
                if (i >= t.size()) fail("для GET нужен ключ");
                string key = t[i++], value;
                bool found = cache.get(key, value);
                results.push_back(found ? value : "-1");
                if (trace) cout << "GET " << key << ": " << results.back() << " (" << cache.show() << ")" << endl;
            } else {
                fail("неизвестный запрос: " + t[i - 1]);
            }
        }
    } catch (const exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
    cout << join(results, " ") << endl;
}
