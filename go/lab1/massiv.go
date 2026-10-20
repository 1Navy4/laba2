// Массив (M): динамический массив строк.
package lab1

import (
	"strconv"
	"strings"
)

type Array struct{ data []string }

func NewArray() *Array { return &Array{} }

func (a *Array) Size() int { return len(a.data) }

func (a *Array) check(i int) {
	if i < 0 || i >= len(a.data) {
		Fail("index out of range: " + strconv.Itoa(i))
	}
}

func (a *Array) Insert(idx int, v string) { // O(n)
	if idx < 0 || idx > len(a.data) {
		Fail("index out of range: " + strconv.Itoa(idx))
	}
	a.data = append(a.data, "") // место под новый элемент; append сам удваивает емкость (в конец амортизированно O(1))
	for i := len(a.data) - 1; i > idx; i-- {
		a.data[i] = a.data[i-1]
	}
	a.data[idx] = v
}

func (a *Array) Push(v string) { a.Insert(len(a.data), v) } // O(1) амортизированно

func (a *Array) Get(i int) string { // O(1)
	a.check(i)
	return a.data[i]
}

func (a *Array) Set(i int, v string) { // O(1)
	a.check(i)
	a.data[i] = v
}

func (a *Array) Erase(i int) string { // O(n)
	a.check(i)
	r := a.data[i]
	for j := i; j+1 < len(a.data); j++ {
		a.data[j] = a.data[j+1]
	}
	a.data = a.data[:len(a.data)-1]
	return r
}

func (a *Array) Type() byte      { return 'M' }
func (a *Array) Dump() []string  { return a.data }
func (a *Array) Load(t []string) {
	for _, s := range t {
		a.Push(s)
	}
}
func (a *Array) Show() string {
	return "[" + strings.Join(a.data, ", ") + "] (len=" + strconv.Itoa(len(a.data)) + ")"
}
