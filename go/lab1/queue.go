// Очередь (Q) строк на основе односвязного списка (начало - голова, конец - хвост).
package lab1

import (
	"strings"
)

type Queue struct{ l SList }

func NewQueue() *Queue { return &Queue{} }

func (q *Queue) Push(v string) { q.l.PushTail(v) } // O(1)

func (q *Queue) Pop() string { // O(1)
	if q.l.n == 0 {
		Fail("queue is empty")
	}
	return q.l.PopHead()
}

func (q *Queue) Items() []string { return q.l.Items() } // от начала к концу

func (q *Queue) Type() byte     { return 'Q' }
func (q *Queue) Dump() []string { return q.l.Items() }
func (q *Queue) Load(t []string) {
	for _, v := range t {
		q.Push(v)
	}
}
func (q *Queue) Show() string {
	if q.l.n == 0 {
		return "(empty queue)"
	}
	return "front -> " + strings.Join(q.l.Items(), ", ") + " <- back"
}
