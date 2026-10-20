#include <fstream>
#include <iostream>
#include <map>
#include <memory>

#include "hashset.h"

using namespace std;

typedef map<string, unique_ptr<HashSet>> Sets;

static void loadSets(Sets& sets, const string& path) {
    ifstream in(path);
    for (string line; getline(in, line);) {
        Strs tok = tokenize(line);
        if (tok.size() < 2 || tok[0] != "H") continue;
        sets[tok[1]] = make_unique<HashSet>();
        sets[tok[1]]->load(Strs(tok.begin() + 2, tok.end()));
    }
}

static void saveSets(const Sets& sets, const string& path) {
    ofstream out(path);
    if (!out) fail("cannot write file: " + path);
    for (auto& [name, s] : sets) {
        out << "H " << quote(name);
        for (auto& v : s->dump()) out << ' ' << quote(v);
        out << '\n';
    }
}

static HashSet& find(Sets& sets, const string& name, bool create) {
    auto it = sets.find(name);
    if (it == sets.end()) {
        if (!create) fail("no such set: " + name);
        it = sets.emplace(name, make_unique<HashSet>()).first;
    }
    return *it->second;
}

static string execute(Sets& sets, const Strs& a) {
    string cmd = upper(a[0]);
    const string& name = arg(a, 1);
    if (cmd == "SETADD") return find(sets, name, true).add(arg(a, 2)) ? "TRUE" : "FALSE";
    if (cmd == "SETDEL") return find(sets, name, false).remove(arg(a, 2)) ? "TRUE" : "FALSE";
    if (cmd == "SET_AT") return find(sets, name, false).contains(arg(a, 2)) ? "TRUE" : "FALSE";
    if (cmd == "PRINT") return find(sets, name, false).show();
    fail("unknown command: " + a[0]);
}

int main(int argc, char** argv) {
    string file = "file.data", query;
    bool hasQuery = false;
    for (int i = 1; i + 1 < argc; i = i + 2) {
        string key = argv[i];
        if (key == "--file") file = argv[i + 1];
        if (key == "--query") { query = argv[i + 1]; hasQuery = true; }
    }
    if (!hasQuery) { cerr << "usage: " << argv[0] << " --file file.data --query 'COMMAND name value'\n"; return 2; }
    try {
        Sets sets;
        loadSets(sets, file);
        Strs a = tokenize(query);
        if (a.empty()) fail("empty query");
        cout << execute(sets, a) << endl;
        string c = upper(a[0]);
        if (c == "SETADD" || c == "SETDEL") saveSets(sets, file);   
    } catch (const exception& e) {
        cerr << "ERROR: " << e.what() << endl;
        return 1;
    }
}
