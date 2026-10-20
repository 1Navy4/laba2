// Бинарное дерево поиска для целых чисел (ЛР2, задание 5). Узел - TNode из дерева ЛР1.
// Слева от узла значения меньше, справа - больше. Повторы не добавляются. Без балансировки.
package lab1

import "strconv"

type BST struct{ root *TNode }

func NewBST() *BST { return &BST{} }

// Сравнение целых чисел, записанных строками в обычном виде (без ведущих нулей):
// -1 если a < b, 0 если равны, 1 если a > b. Работает за O(длина) без преобразования в число.
func cmpInt(a, b string) int {
	na, nb := a[0] == '-', b[0] == '-'
	if na != nb { // отрицательное меньше неотрицательного
		if na {
			return -1
		}
		return 1
	}
	r := 0
	if len(a) != len(b) { // длиннее - больше по модулю
		if len(a) < len(b) {
			r = -1
		} else {
			r = 1
		}
	} else if a != b {
		if a < b {
			r = -1
		} else {
			r = 1
		}
	}
	if na { // у отрицательных порядок обратный
		return -r
	}
	return r
}

// Insert добавляет число. Возвращает его глубину (корень - 1) или 0, если число уже есть в дереве.
// O(h), где h - высота дерева: в среднем O(log n), в худшем O(n).
func (t *BST) Insert(v int64) int {
	key := strconv.FormatInt(v, 10)
	if t.root == nil {
		t.root = &TNode{key: key}
		return 1
	}
	depth := 1
	x := t.root
	for {
		c := cmpInt(key, x.key)
		if c == 0 {
			return 0 // уже есть - ничего не добавляем
		}
		next := &x.right // больше - вправо
		if c < 0 {
			next = &x.left // меньше - влево
		}
		depth++ // спустились на уровень ниже
		if *next == nil {
			*next = &TNode{key: key}
			return depth
		}
		x = *next
	}
}
