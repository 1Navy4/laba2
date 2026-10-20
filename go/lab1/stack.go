// Стек (S) строк на основе односвязного списка (вершина - голова списка).
package lab1

type Stack struct{ l SList }

func NewStack() *Stack { return &Stack{} }

func (s *Stack) Push(v string) { s.l.PushHead(v) } // O(1)

func (s *Stack) Pop() string { // O(1)
	if s.l.n == 0 {
		Fail("stack is empty")
	}
	return s.l.PopHead()
}

func (s *Stack) Items() []string { return s.l.Items() } // от вершины к низу

func (s *Stack) Type() byte     { return 'S' }
func (s *Stack) Dump() []string { return reversed(s.l.Items()) } // от низа к вершине
func (s *Stack) Load(t []string) {
	for _, v := range t {
		s.Push(v)
	}
}
func (s *Stack) Show() string {
	v := s.l.Items()
	if len(v) == 0 {
		return "(empty stack)"
	}
	r := ""
	for i, x := range v {
		r = r + "| " + x + " |"
		if i == 0 {
			r = r + "  <- top"
		}
		r = r + "\n"
	}
	return r + "+---+"
}

// ---- добавлено в ЛР2 ----

func (s *Stack) Empty() bool { return s.l.n == 0 } // стек пуст?
func (s *Stack) Top() string { return s.l.At(0) }  // вершина без удаления, O(1)
