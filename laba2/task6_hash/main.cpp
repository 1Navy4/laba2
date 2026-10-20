// Задание 6 (6.1 и 6.2). Хеш-таблица с открытой адресацией и встроенным фильтром Блума (вариант 8).
// Запуск: ./task6_hash --file data.txt --query 'HSET имя ключ значение'
// Команды:
//   HNEW имя linear|double   создать пустую таблицу с выбранным методом (линейное апробирование / двойное хеширование)
//   HSET имя ключ значение   добавить или обновить (если таблицы нет - создается с двойным хешированием)
//   HGET имя ключ            значение по ключу или NOT FOUND (сначала проверяет фильтр Блума)
//   HHAS имя ключ            есть ли ключ: TRUE или FALSE
//   HDEL имя ключ            удалить ключ: TRUE или FALSE
//   HLEN имя                 число элементов
//   PRINT имя                таблица по ячейкам
//   HSTATS имя               параметры таблицы и фильтра Блума
// Файл: по строке на таблицу "HT <имя> <метод> <размер> <живых> <надгробий> <ячейки...>".
#include <fstream>
#include <iostream>
#include <map>
#include <memory>

#include "hashtable.h"

using namespace std;

typedef map<string, unique_ptr<HashTable>> Tables;

static void loadTables(Tables& tables, const string& path) {
    ifstream in(path);
    for (string line; getline(in, line);) {
        Strs tok = tokenize(line);
        if (tok.size() < 2 || tok[0] != "HT") continue;
        auto t = make_unique<HashTable>(DOUBLE);
        t->load(Strs(tok.begin() + 2, tok.end()));
        tables[tok[1]] = std::move(t);
    }
}

static void saveTables(const Tables& tables, const string& path) {
    ofstream out(path);
    if (!out) fail("cannot write file: " + path);
    for (auto& [name, t] : tables) {
        out << "HT " << quote(name);
        for (auto& s : t->dump()) out << ' ' << quote(s);
        out << '\n';
    }
}

static HashTable& find(Tables& tables, const string& name, bool create) {
    auto it = tables.find(name);
    if (it == tables.end()) {
        if (!create) fail("no such table: " + name);
        it = tables.emplace(name, make_unique<HashTable>(DOUBLE)).first;
    }
    return *it->second;
}

static string execute(Tables& tables, const Strs& a) {
    string cmd = upper(a[0]);
    const string& name = arg(a, 1);
    if (cmd == "HNEW") {
        string m = arg(a, 2);
        if (m != "linear" && m != "double") fail("method must be linear or double");
        if (tables.count(name)) fail("table already exists: " + name);
        tables.emplace(name, make_unique<HashTable>(m == "linear" ? LINEAR : DOUBLE));
        return name;
    }
    if (cmd == "HSET") { find(tables, name, true).put(arg(a, 2), arg(a, 3)); return a[3]; }
    if (cmd == "HGET") {
        string v;
        return find(tables, name, false).get(arg(a, 2), v) ? v : "NOT FOUND";
    }
    if (cmd == "HHAS") return find(tables, name, false).contains(arg(a, 2)) ? "TRUE" : "FALSE";
    if (cmd == "HDEL") return find(tables, name, false).erase(arg(a, 2)) ? "TRUE" : "FALSE";
    if (cmd == "HLEN") return to_string(find(tables, name, false).size());
    if (cmd == "PRINT") return find(tables, name, false).show();
    if (cmd == "HSTATS") return find(tables, name, false).stats();
    fail("unknown command: " + a[0]);
}

static bool readOnly(const string& c) {
    return c == "HGET" || c == "HHAS" || c == "HLEN" || c == "PRINT" || c == "HSTATS";
}

int main(int argc, char** argv) {
    string file = "file.data", query;
    bool hasQuery = false;
    for (int i = 1; i + 1 < argc; i = i + 2) {
        string key = argv[i];
        if (key == "--file") file = argv[i + 1];
        if (key == "--query") { query = argv[i + 1]; hasQuery = true; }
    }
    if (!hasQuery) { cerr << "usage: " << argv[0] << " --file file.data --query 'COMMAND name args'\n"; return 2; }
    try {
        Tables tables;
        loadTables(tables, file);
        Strs a = tokenize(query);
        if (a.empty()) fail("empty query");
        cout << execute(tables, a) << endl;
        if (!readOnly(upper(a[0]))) saveTables(tables, file);
    } catch (const exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
}
