// Задание 5 (вариант 8). Глубина добавляемых элементов.
// Ввод (stdin или --file путь): последовательность целых чисел, оканчивающаяся нулем
// (сам ноль в последовательность не входит).
// Вывод: глубина каждого добавленного числа в порядке добавления. Если число уже есть
// в дереве, оно не добавляется и ничего не выводится. Глубина - расстояние от корня
// до элемента включительно (у корня глубина 1).
//
// Дерево - бинарное дерево поиска на узлах TNode из ЛР1 (меньше - влево, больше - вправо).
#include <iostream>
#include <sstream>

#include "bst.h"
#include "io.h"

using namespace std;

int main(int argc, char** argv) {
    istringstream in(readInput(argc, argv));
    BST tree;
    bool first = true;
    long long v;
    while (in >> v && v != 0) {
        int depth = tree.insert(v);
        if (depth == 0) continue;                 // повтор
        if (!first) cout << ' ';
        cout << depth;
        first = false;
    }
    cout << endl;
}
