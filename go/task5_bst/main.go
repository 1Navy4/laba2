// Задание 5 (вариант 8). Глубина добавляемых элементов.
// Ввод (stdin или --file путь): последовательность целых чисел, оканчивающаяся нулем
// (сам ноль в последовательность не входит).
// Вывод: глубина каждого добавленного числа в порядке добавления. Если число уже есть
// в дереве, оно не добавляется и ничего не выводится. Глубина - расстояние от корня
// до элемента включительно (у корня глубина 1).
//
// Дерево - бинарное дерево поиска на узлах TNode из ЛР1 (меньше - влево, больше - вправо).
package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"

	"laba2go/lab1"
	"laba2go/shared"
)

func main() {
	tree := lab1.NewBST()
	var depths []string
	for _, w := range strings.Fields(shared.ReadInput(os.Args[1:])) {
		v, err := strconv.ParseInt(w, 10, 64)
		if err != nil || v == 0 { // ноль (или не число) - конец последовательности
			break
		}
		if depth := tree.Insert(v); depth != 0 { // повтор не добавляется и не выводится
			depths = append(depths, strconv.Itoa(depth))
		}
	}
	fmt.Println(strings.Join(depths, " "))
}
