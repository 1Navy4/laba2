#include "common.h"

#include <cctype>
#include <stdexcept>

using namespace std;

void fail(const string& msg) { throw runtime_error(msg); }

string join(const Strs& v, const string& sep) {
    string r;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) r += sep;      // += дописывает в конец без копирования всей строки (раньше было r = r + ..., O(n^2))
        r += v[i];
    }
    return r;
}

string quote(const string& s) {
    bool need = s.empty();
    for (char c : s)
        if (isspace((unsigned char)c) || c == '"' || c == '\\') need = true;
    if (!need) return s;
    string r = "\"";
    for (char c : s) {
        if (c == '"' || c == '\\') r += '\\';
        r += c;
    }
    return r + "\"";
}

string spaced(const Strs& v) {
    if (v.empty()) return "(empty)";
    Strs q;
    for (auto& s : v) q.push_back(quote(s));
    return join(q, " ");
}

string upper(string s) {
    for (auto& c : s) c = toupper((unsigned char)c);
    return s;
}

bool endsWith(const string& s, const string& suf) {
    return s.size() >= suf.size() && s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

Strs tokenize(const string& s) {
    Strs r;
    size_t i = 0, n = s.size();
    while (i < n) {
        while (i < n && isspace((unsigned char)s[i])) ++i;
        if (i >= n) break;
        string tok;
        if (s[i] == '"') {
            ++i;
            bool closed = false;
            while (i < n) {
                char c = s[i++];
                if (c == '\\' && i < n) tok += s[i++];
                else if (c == '"') { closed = true; break; }
                else tok += c;
            }
            if (!closed) fail("unterminated quote in: " + s);
        } else {
            while (i < n && !isspace((unsigned char)s[i])) tok += s[i++];
        }
        r.push_back(tok);
    }
    return r;
}

const string& arg(const Strs& a, size_t i) {
    if (i >= a.size()) fail("missing argument for " + a[0]);
    return a[i];
}

size_t toIndex(const string& s) {
    if (s.empty() || s.size() > 9) fail("bad index: " + s);                        // пусто или слишком длинно
    if (s.find_first_not_of("0123456789") != string::npos) fail("bad index: " + s);  // есть не цифры (в т.ч. минус)
    return stoul(s);
}

bool isInteger(const string& s) {
    size_t i = (!s.empty() && s[0] == '-') ? 1 : 0;
    if (i >= s.size() || s.size() - i > 18) return false;  // пусто или не помещается в long long
    return s.find_first_not_of("0123456789", i) == string::npos;
}

string normInt(const string& s) {
    if (!isInteger(s)) fail("tree stores integers only, got: " + s);
    return to_string(stoll(s));
}
