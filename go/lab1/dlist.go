// Двусвязный список (L) строк.
package lab1

import (
	"strconv"
	"strings"
)

type DNode struct {
	val        string
	prev, next *DNode
}

type DList struct {
	head, tail *DNode
	n          int
}

func NewDList() *DList { return &DList{} }

func (l *DList) findNode(v string) *DNode {
	for c := l.head; c != nil; c = c.next {
		if c.val == v {
			return c
		}
	}
	return nil
}

func (l *DList) mustFind(v string) *DNode {
	t := l.findNode(v)
	if t == nil {
		Fail("value not found: " + v)
	}
	return t
}

func (l *DList) unlink(x *DNode) string { // O(1) - главное преимущество двусвязного списка
	if x.prev != nil {
		x.prev.next = x.next
	} else {
		l.head = x.next
	}
	if x.next != nil {
		x.next.prev = x.prev
	} else {
		l.tail = x.prev
	}
	l.n--
	return x.val
}

func (l *DList) insertAfter(t *DNode, v string) {
	x := &DNode{v, t, t.next}
	if t.next != nil {
		t.next.prev = x
	} else {
		l.tail = x
	}
	t.next = x
	l.n++
}

func (l *DList) PushHead(v string) { // O(1)
	x := &DNode{val: v, next: l.head}
	if l.head != nil {
		l.head.prev = x
	} else {
		l.tail = x
	}
	l.head = x
	l.n++
}

func (l *DList) PushTail(v string) { // O(1)
	x := &DNode{val: v, prev: l.tail}
	if l.tail != nil {
		l.tail.next = x
	} else {
		l.head = x
	}
	l.tail = x
	l.n++
}

func (l *DList) PushAfter(target, v string) { // O(n) - поиск
	l.insertAfter(l.mustFind(target), v)
}

func (l *DList) PushBefore(target, v string) { // O(n) - поиск
	t := l.mustFind(target)
	if t == l.head {
		l.PushHead(v)
	} else {
		l.insertAfter(t.prev, v)
	}
}

func (l *DList) PopHead() string { // O(1)
	if l.head == nil {
		Fail("list is empty")
	}
	return l.unlink(l.head)
}

func (l *DList) PopTail() string { // O(1), в отличие от односвязного
	if l.head == nil {
		Fail("list is empty")
	}
	return l.unlink(l.tail)
}

func (l *DList) DelAfter(target string) string {
	t := l.mustFind(target)
	if t.next == nil {
		Fail("no element after " + target)
	}
	return l.unlink(t.next)
}

func (l *DList) DelBefore(target string) string {
	t := l.mustFind(target)
	if t.prev == nil {
		Fail("no element before " + target)
	}
	return l.unlink(t.prev)
}

func (l *DList) DelValue(v string) bool {
	x := l.findNode(v)
	if x == nil {
		return false
	}
	l.unlink(x)
	return true
}

func (l *DList) Find(v string) int {
	i := 0
	for c := l.head; c != nil; c = c.next {
		if c.val == v {
			return i
		}
		i++
	}
	return -1
}

func (l *DList) At(i int) string {
	if i >= l.n {
		Fail("index out of range: " + strconv.Itoa(i))
	}
	c := l.head
	for ; i > 0; i-- {
		c = c.next
	}
	return c.val
}

func (l *DList) Items() []string {
	var r []string
	for c := l.head; c != nil; c = c.next {
		r = append(r, c.val)
	}
	return r
}

func (l *DList) ItemsReversed() []string {
	var r []string
	for c := l.tail; c != nil; c = c.prev {
		r = append(r, c.val)
	}
	return r
}

func (l *DList) Type() byte      { return 'L' }
func (l *DList) Dump() []string  { return l.Items() }
func (l *DList) Load(t []string) {
	for _, s := range t {
		l.PushTail(s)
	}
}
func (l *DList) Show() string {
	if l.head == nil {
		return "NULL"
	}
	return "NULL <-> " + strings.Join(l.Items(), " <-> ") + " <-> NULL"
}

// ---- добавлено в ЛР2 (нужно для LRU-кэша): операции над узлом за O(1) ----

func (x *DNode) Val() string { return x.val }

// PushHeadNode добавляет в голову и возвращает созданный узел.
func (l *DList) PushHeadNode(v string) *DNode {
	l.PushHead(v)
	return l.head
}

func (l *DList) TailNode() *DNode { return l.tail }

// MoveToHead переносит узел списка в голову.
func (l *DList) MoveToHead(x *DNode) {
	if x == l.head {
		return
	}
	x.prev.next = x.next // вырезаем узел из текущего места (x не голова, значит prev есть)
	if x.next != nil {
		x.next.prev = x.prev
	} else {
		l.tail = x.prev
	}
	x.prev = nil // вставляем перед текущей головой
	x.next = l.head
	l.head.prev = x
	l.head = x
}
