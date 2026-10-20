// Ключи случайные: присутствующие начинаются с 'k', отсутствующие с 'z', поэтому не пересекаются.
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "hashtable.h"

using namespace std;

static mt19937_64 rng;

static string randomKey(char prefix) {
    string s(1, prefix);
    for (int i = 0; i < 12; ++i) s += (char)('a' + rng() % 26);
    return s;
}

// Теоретические средние числа проб (Кнут) при коэффициенте заполнения a
static double theorySuccess(Method m, double a) {
    if (m == LINEAR) return 0.5 * (1 + 1 / (1 - a));
    return log(1 / (1 - a)) / a;
}
static double theoryFail(Method m, double a) {
    if (m == LINEAR) return 0.5 * (1 + 1 / ((1 - a) * (1 - a)));
    return 1 / (1 - a);
}

static void analyze(Method m, size_t cap, size_t samples) {
    cout << (m == LINEAR ? "Линейное апробирование" : "Двойное хеширование") << "  (ячеек: " << cap << ")\n";
    cout << " заполнение | вставка | успешный поиск (теория) | неуспешный поиск (теория)\n";
    cout << "------------+---------+-------------------------+--------------------------\n";
    HashTable t(m, cap, false, false);
    vector<string> keys;
    for (int pct = 10; pct <= 90; pct += 10) {
        size_t target = cap * pct / 100, inserted = 0;
        long long before = t.probes;
        while (keys.size() < target) {
            string k = randomKey('k');
            if (t.put(k, "v")) { keys.push_back(k); ++inserted; }
        }
        double ins = (double)(t.probes - before) / inserted;

        string v;
        before = t.probes;
        for (size_t i = 0; i < samples; ++i) t.get(keys[rng() % keys.size()], v);
        double succ = (double)(t.probes - before) / samples;

        before = t.probes;
        for (size_t i = 0; i < samples; ++i) t.get(randomKey('z'), v);
        double fail = (double)(t.probes - before) / samples;

        double a = (double)keys.size() / cap;
        cout << fixed << setprecision(2) << "    " << setw(3) << pct << "%    | " << setw(7) << ins << " | "
             << setw(9) << succ << " (" << setw(6) << theorySuccess(m, a) << ")      | "
             << setw(9) << fail << " (" << setw(7) << theoryFail(m, a) << ")\n";
    }
    cout << "\n";
}

static void bloomEffect(size_t cap, size_t samples) {
    cout << "Фильтр Блума в таблице с двойным хешированием (" << cap << " ячеек, 10 бит на ячейку, 4 хеш-функции)\n";
    cout << " заполнение | проб на неуспешный поиск: без фильтра -> с фильтром | отсеяно фильтром | ложных срабатываний\n";
    cout << "------------+------------------------------------------------------+------------------+--------------------\n";
    for (int pct : {30, 50, 70}) {
        HashTable plain(DOUBLE, cap, false, false), withBloom(DOUBLE, cap, false, true);
        size_t n = cap * pct / 100;
        for (size_t i = 0; i < n;) {
            string k = randomKey('k');
            if (plain.put(k, "v")) { withBloom.put(k, "v"); ++i; }
        }
        vector<string> absent;
        for (size_t i = 0; i < samples; ++i) absent.push_back(randomKey('z'));
        string v;
        plain.resetCounters();
        withBloom.resetCounters();
        for (auto& k : absent) { plain.get(k, v); withBloom.get(k, v); }
        cout << fixed << setprecision(3) << "    " << setw(3) << pct << "%    | "
             << setw(22) << (double)plain.probes / samples << "  ->  " << setw(10) << (double)withBloom.probes / samples
             << "         | " << setw(8) << setprecision(2) << 100.0 * withBloom.bloomRejects / samples << "%        | "
             << setw(8) << setprecision(3) << 100.0 * withBloom.bloomFalse / samples << "%\n";
    }
    cout << "\n";
}

int main(int argc, char** argv) {
    size_t cap = 100003, samples = 20000;
    unsigned long long seed = 12345;
    for (int i = 1; i + 1 < argc; i = i + 2) {
        string key = argv[i];
        if (key == "--cap") cap = stoul(argv[i + 1]);
        if (key == "--seed") seed = stoull(argv[i + 1]);
        if (key == "--samples") samples = stoul(argv[i + 1]);
    }
    rng.seed(seed);
    cout << "Среднее число проб (просмотренных ячеек), " << samples << " запросов на каждый уровень заполнения\n\n";
    analyze(LINEAR, cap, samples);
    analyze(DOUBLE, cap, samples);
    bloomEffect(cap, samples);
}
