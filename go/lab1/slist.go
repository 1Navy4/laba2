// Односвязный список (F) строк.
package lab1

import (
	"strconv"
	"strings"
)

// Память освобождает сборщик мусора Go, delete не нужен.

type SNode struct {
	val  string
	next *SNode
}

type SList struct {
	head, tail *SNode
	n          int
}

func NewSList() *SList { return &SList{} }

func (l *SList) findNode(v string) *SNode {
	for c := l.head; c != nil; c = c.next {
		if c.val == v {
			return c
		}
	}
	return nil
}

func (l *SList) mustFind(v string) *SNode {
	t := l.findNode(v)
	if t == nil {
		Fail("value not found: " + v)
	}
	return t
}

func (l *SList) PushHead(v string) { // O(1)
	l.head = &SNode{v, l.head}
	if l.tail == nil {
		l.tail = l.head
	}
	l.n++
}

func (l *SList) PushTail(v string) { // O(1), есть указатель на хвост
	x := &SNode{val: v}
	if l.tail != nil {
		l.tail.next = x
	} else {
		l.head = x
	}
	l.tail = x
	l.n++
}

func (l *SList) PushAfter(target, v string) { // O(n) - поиск узла
	t := l.mustFind(target)
	t.next = &SNode{v, t.next}
	if t == l.tail {
		l.tail = t.next
	}
	l.n++
}

func (l *SList) PushBefore(target, v string) { // O(n)
	l.mustFind(target)
	if l.head.val == target {
		l.PushHead(v)
		return
	}
	p := l.head
	for p.next.val != target { // p - узел перед целевым
		p = p.next
	}
	p.next = &SNode{v, p.next}
	l.n++
}

func (l *SList) PopHead() string { // O(1)
	if l.head == nil {
		Fail("list is empty")
	}
	t := l.head
	l.head = t.next
	if l.head == nil {
		l.tail = nil
	}
	l.n--
	return t.val
}

func (l *SList) PopTail() string { // O(n) - нужно найти предпоследний узел
	if l.head == nil {
		Fail("list is empty")
	}
	if l.head == l.tail {
		return l.PopHead()
	}
	p := l.head
	for p.next != l.tail {
		p = p.next
	}
	r := l.tail.val
	l.tail = p
	p.next = nil
	l.n--
	return r
}

func (l *SList) DelAfter(target string) string { // O(n)
	t := l.mustFind(target)
	if t.next == nil {
		Fail("no element after " + target)
	}
	x := t.next
	t.next = x.next
	if x == l.tail {
		l.tail = t
	}
	l.n--
	return x.val
}

func (l *SList) DelBefore(target string) string { // O(n)
	t := l.mustFind(target)
	if t == l.head {
		Fail("no element before " + target)
	}
	if l.head.next == t {
		return l.PopHead()
	}
	p := l.head
	for p.next.next != t { // p - узел за два до целевого
		p = p.next
	}
	x := p.next
	p.next = t
	l.n--
	return x.val
}

func (l *SList) DelValue(v string) bool { // O(n)
	if l.findNode(v) == nil {
		return false
	}
	if l.head.val == v {
		l.PopHead()
		return true
	}
	p := l.head
	for p.next.val != v {
		p = p.next
	}
	x := p.next
	p.next = x.next
	if x == l.tail {
		l.tail = p
	}
	l.n--
	return true
}

func (l *SList) Find(v string) int { // O(n)
	i := 0
	for c := l.head; c != nil; c = c.next {
		if c.val == v {
			return i
		}
		i++
	}
	return -1
}

func (l *SList) At(i int) string { // O(n)
	if i >= l.n {
		Fail("index out of range: " + strconv.Itoa(i))
	}
	c := l.head
	for ; i > 0; i-- {
		c = c.next
	}
	return c.val
}

func (l *SList) Items() []string {
	var r []string
	for c := l.head; c != nil; c = c.next {
		r = append(r, c.val)
	}
	return r
}

func (l *SList) Type() byte      { return 'F' }
func (l *SList) Dump() []string  { return l.Items() }
func (l *SList) Load(t []string) {
	for _, s := range t {
		l.PushTail(s)
	}
}
func (l *SList) Show() string {
	if l.head == nil {
		return "NULL"
	}
	return strings.Join(l.Items(), " -> ") + " -> NULL"
}
