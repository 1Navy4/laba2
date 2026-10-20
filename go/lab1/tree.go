// Complete Binary Tree (T). Хранит только целые числа (в том числе отрицательные).
package lab1

import (
	"sort"
	"strconv"
	"strings"
)

// Все уровни заполнены полностью, кроме, возможно, последнего; последний заполняется
// слева направо без пропусков. Вставка кладет узел на первое свободное место при обходе
// в ширину, поэтому дерево из одних вставок всегда complete. В файле дерево хранится как
// прямой обход с '#' на месте пустых мест, так что там можно записать и НЕполное дерево.

type TNode struct {
	key         string
	left, right *TNode
}

type CBTree struct{ root *TNode }

func NewCBTree() *CBTree { return &CBTree{} }

// nodes возвращает все узлы в порядке обхода в ширину; сам срез служит очередью.
func (t *CBTree) nodes() []*TNode {
	var o []*TNode
	if t.root != nil {
		o = append(o, t.root)
	}
	for i := 0; i < len(o); i++ {
		if o[i].left != nil {
			o = append(o, o[i].left)
		}
		if o[i].right != nil {
			o = append(o, o[i].right)
		}
	}
	return o
}

func (t *CBTree) Insert(k string) string { // O(n); возвращает сохраненное число
	z := &TNode{key: NormInt(k)} // не число -> ошибка до изменения дерева
	if t.root == nil {
		t.root = z
		return z.key
	}
	for _, x := range t.nodes() { // первое свободное место при обходе в ширину: дерево остается complete
		if x.left == nil {
			x.left = z
			return z.key
		}
		if x.right == nil {
			x.right = z
			return z.key
		}
	}
	return z.key
}

func (t *CBTree) Contains(k string) bool { // O(n) - дерево не упорядочено
	key := NormInt(k)
	for _, x := range t.nodes() {
		if x.key == key {
			return true
		}
	}
	return false
}

// IsComplete, O(n): обход в ширину вместе с пустыми местами (nil).
// Если после первого пустого места встретился настоящий узел - дерево не complete.
func (t *CBTree) IsComplete() bool {
	o := []*TNode{t.root}
	gap := false
	for i := 0; i < len(o); i++ {
		if o[i] == nil {
			gap = true
			continue
		}
		if gap {
			return false
		}
		o = append(o, o[i].left)
		o = append(o, o[i].right)
	}
	return true
}

// Три обхода в глубину отличаются только местом, где записывается сам узел.
func preorder(x *TNode, o *[]string) { // прямой: узел, левое, правое
	if x == nil {
		return
	}
	*o = append(*o, x.key)
	preorder(x.left, o)
	preorder(x.right, o)
}

func inorder(x *TNode, o *[]string) { // симметричный: левое, узел, правое
	if x == nil {
		return
	}
	inorder(x.left, o)
	*o = append(*o, x.key)
	inorder(x.right, o)
}

func postorder(x *TNode, o *[]string) { // обратный: левое, правое, узел
	if x == nil {
		return
	}
	postorder(x.left, o)
	postorder(x.right, o)
	*o = append(*o, x.key)
}

func (t *CBTree) Traverse(order string) []string {
	var o []string
	switch order {
	case "PREORDER":
		preorder(t.root, &o)
	case "INORDER":
		inorder(t.root, &o)
	case "POSTORDER":
		postorder(t.root, &o)
	case "SORTED": // значения по возрастанию; само дерево не меняется
		for _, x := range t.nodes() {
			o = append(o, x.key)
		}
		sort.Slice(o, func(i, j int) bool {
			a, _ := strconv.ParseInt(o[i], 10, 64)
			b, _ := strconv.ParseInt(o[j], 10, 64)
			return a < b
		})
	case "BFS":
		for _, x := range t.nodes() {
			o = append(o, x.key)
		}
	default:
		Fail("unknown order: " + order + " (inorder, preorder, postorder, bfs, sorted)")
	}
	return o
}

func dumpRec(x *TNode, o *[]string) {
	if x == nil {
		*o = append(*o, "#")
		return
	}
	*o = append(*o, x.key)
	dumpRec(x.left, o)
	dumpRec(x.right, o)
}

func build(tok []string, i *int) *TNode {
	if *i >= len(tok) {
		return nil
	}
	key := tok[*i]
	*i = *i + 1
	if key == "#" {
		return nil
	}
	x := &TNode{key: NormInt(key)} // в дереве только числа
	x.left = build(tok, i)
	x.right = build(tok, i)
	return x
}

func (t *CBTree) Type() byte { return 'T' }
func (t *CBTree) Dump() []string {
	var o []string
	dumpRec(t.root, &o)
	return o
}
func (t *CBTree) Load(tok []string) {
	i := 0
	t.root = build(tok, &i)
}
// ---------- Красивый вертикальный вывод ----------
// Каждое поддерево рисуется как прямоугольный блок строк. Блоки левого и правого ребенка
// ставятся рядом, а родитель центрируется над ними и соединяется перекладиной:
//        10
//    ┌────┴────┐
//    20        30

type block struct {
	rows  [][]rune
	width int // ширина блока
	root  int // колонка, в которой стоит корень блока
}

func spaces(n int) []rune {
	if n < 0 {
		n = 0
	}
	r := make([]rune, n)
	for i := range r {
		r[i] = ' '
	}
	return r
}

func fit(s []rune, w int) []rune { // дополнить пробелами до ширины w
	r := make([]rune, w)
	for i := range r {
		if i < len(s) {
			r[i] = s[i]
		} else {
			r[i] = ' '
		}
	}
	return r
}

func buildBlock(x *TNode) block {
	label := []rune(x.key)
	n := len(label)
	if x.left == nil && x.right == nil {
		return block{rows: [][]rune{label}, width: n, root: (n - 1) / 2}
	}

	const gap = 3 // пустое место между блоками детей
	var L, R block
	hl, hr := x.left != nil, x.right != nil
	if hl {
		L = buildBlock(x.left)
	}
	if hr {
		R = buildBlock(x.right)
	}

	rOff := 0 // где начинается блок правого ребенка
	if hl {
		rOff = L.width + gap
	}
	lr, rr := L.root, rOff+R.root
	var c int // колонка родителя
	switch {
	case hl && hr:
		c = (lr + rr) / 2
	case hl:
		c = lr + 2 // один левый ребенок: родитель правее и выше
	default:
		c = rr - 2 // один правый ребенок: родитель левее и выше
	}
	start := c - (n-1)/2 // первая буква подписи родителя
	shift := 0
	if start < 0 { // если вылезли влево - сдвигаем все вправо
		shift = -start
	}
	lr += shift
	rr += shift
	c += shift
	start += shift

	childW := R.width
	if hl && hr {
		childW = L.width + gap + R.width
	} else if hl {
		childW = L.width
	}
	width := shift + childW
	if start+n > width {
		width = start + n
	}

	res := block{width: width, root: c}
	top := spaces(width)
	copy(top[start:start+n], label)
	res.rows = append(res.rows, top)

	bar := spaces(width) // перекладина между родителем и детьми
	from, to := c, c
	if hl {
		from = lr
	}
	if hr {
		to = rr
	}
	for i := from; i <= to; i++ {
		bar[i] = '─'
	}
	if hl {
		bar[lr] = '┌'
	}
	if hr {
		bar[rr] = '┐'
	}
	switch {
	case hl && hr:
		bar[c] = '┴'
	case hl:
		bar[c] = '┘'
	default:
		bar[c] = '└'
	}
	res.rows = append(res.rows, bar)

	h := len(L.rows)
	if len(R.rows) > h {
		h = len(R.rows)
	}
	for i := 0; i < h; i++ {
		line := spaces(shift)
		if hl {
			var row []rune
			if i < len(L.rows) {
				row = L.rows[i]
			}
			line = append(line, fit(row, L.width)...)
		}
		if hl && hr {
			line = append(line, spaces(gap)...)
		}
		if hr {
			var row []rune
			if i < len(R.rows) {
				row = R.rows[i]
			}
			line = append(line, fit(row, R.width)...)
		}
		res.rows = append(res.rows, fit(line, width))
	}
	return res
}

func (t *CBTree) Show() string {
	if t.root == nil {
		return "(empty tree)"
	}
	b := buildBlock(t.root)
	lines := make([]string, 0, len(b.rows))
	for _, r := range b.rows {
		lines = append(lines, strings.TrimRight(string(r), " "))
	}
	return strings.Join(lines, "\n")
}
