// Задание 1 (вариант 7). XML. Решается стеком из ЛР1.
// В XML-строке ровно один символ заменен на другой. Нужно восстановить исходную строку
// или вывести Incorrect. Ввод: строка из stdin или из файла (--file путь).
//
// Алгоритм: перебираем все позиции и все возможные символы замены (строчные буквы и < > /),
// для каждого варианта проверяем корректность строки стеком открытых тегов.
// Проверка за O(n), вариантов 29 * n, итого O(n^2) - при n <= 1000 это мгновенно.
#include <iostream>

#include "io.h"
#include "stack.h"

using namespace std;

// Корректна ли строка из тегов <имя> и </имя>: каждый закрывающий тег должен
// закрывать последний открытый, в конце стек должен быть пуст.
static bool isValidXml(const string& s) {
    Stack st;
    size_t i = 0, n = s.size();
    while (i < n) {
        if (s[i] != '<') return false;
        ++i;
        bool closing = i < n && s[i] == '/';
        if (closing) ++i;
        size_t begin = i;
        while (i < n && s[i] >= 'a' && s[i] <= 'z') ++i;       // имя тега: непустая строка из a-z
        if (i == begin || i >= n || s[i] != '>') return false;
        string name = s.substr(begin, i - begin);
        ++i;
        if (!closing) {
            st.push(name);                                      // открывающий тег - в стек
        } else {
            if (st.empty() || st.top() != name) return false;   // закрывающий должен совпасть с вершиной
            st.pop();
        }
    }
    return st.empty();
}

int main(int argc, char** argv) {
    string text = readInput(argc, argv);
    size_t eol = text.find('\n');
    if (eol != string::npos) text = text.substr(0, eol);

    string s;                                  // пробелы между тегами (как в примерах задания) отбрасываем
    for (char c : text) if (c != ' ' && c != '\r' && c != '\t') s += c;
    bool hadSpaces = s.size() != text.size();

    const string alphabet = "abcdefghijklmnopqrstuvwxyz</>";
    // Позиции перебираем с конца: если исправить можно и открывающий, и закрывающий тег
    // (<a></b>), выбирается исправление закрывающего тега - <a></a>.
    for (size_t i = s.size(); i-- > 0;) {
        char original = s[i];
        for (char c : alphabet) {
            if (c == original) continue;       // символ именно заменен на другой
            s[i] = c;
            if (isValidXml(s)) {
                string out;
                for (char ch : s) {            // в исходном виде: если были пробелы, ставим их между тегами
                    out += ch;
                    if (hadSpaces && ch == '>') out += ' ';
                }
                while (!out.empty() && out.back() == ' ') out.pop_back();
                cout << out << endl;
                return 0;
            }
        }
        s[i] = original;
    }
    cout << "Incorrect" << endl;
    return 0;
}
