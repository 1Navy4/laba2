// Множество (АТД) на хеш-таблице с цепочками. Корзины - односвязные списки из ЛР1.
// Операции Add / Remove / Contains выполняются за O(1) в среднем: при заполнении
// таблицы (число элементов больше числа корзин) она увеличивается вдвое.
package shared

import (
	"sort"
	"strings"

	"laba2go/lab1"
)

type HashSet struct {
	buckets []*lab1.SList
	count   int
}

func NewHashSet() *HashSet {
	h := &HashSet{}
	for i := 0; i < 8; i++ {
		h.buckets = append(h.buckets, lab1.NewSList())
	}
	return h
}

func (h *HashSet) index(v string) int { return int(hashDjb2(v) % uint64(len(h.buckets))) }

func (h *HashSet) grow() { // удвоение числа корзин и перенос элементов
	bigger := make([]*lab1.SList, len(h.buckets)*2)
	for i := range bigger {
		bigger[i] = lab1.NewSList()
	}
	for _, b := range h.buckets {
		for _, s := range b.Items() {
			bigger[int(hashDjb2(s)%uint64(len(bigger)))].PushHead(s)
		}
	}
	h.buckets = bigger
}

// Contains: принадлежит ли элемент множеству.
func (h *HashSet) Contains(v string) bool { return h.buckets[h.index(v)].Find(v) >= 0 }

// Add: true - добавлен, false - уже был.
func (h *HashSet) Add(v string) bool {
	if h.Contains(v) {
		return false
	}
	if h.count >= len(h.buckets) {
		h.grow()
	}
	h.buckets[h.index(v)].PushHead(v)
	h.count++
	return true
}

// Remove: true - удален, false - не было.
func (h *HashSet) Remove(v string) bool {
	removed := h.buckets[h.index(v)].DelValue(v)
	if removed {
		h.count--
	}
	return removed
}

func (h *HashSet) Size() int { return h.count }

// Items возвращает элементы по возрастанию (для вывода и файла).
func (h *HashSet) Items() []string {
	var r []string
	for _, b := range h.buckets {
		r = append(r, b.Items()...)
	}
	sort.Strings(r)
	return r
}

func (h *HashSet) Dump() []string { return h.Items() }

func (h *HashSet) Load(t []string) {
	for _, s := range t {
		h.Add(s)
	}
}

func (h *HashSet) Show() string {
	v := h.Items()
	q := make([]string, len(v))
	for i, s := range v {
		q[i] = lab1.Quote(s)
	}
	return "{" + strings.Join(q, ", ") + "}"
}
