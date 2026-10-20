// Задание 1 (вариант 7). XML. Решается стеком из ЛР1.
// В XML-строке ровно один символ заменен на другой. Нужно восстановить исходную строку
// или вывести Incorrect. Ввод: строка из stdin или из файла (--file путь).
//
// Алгоритм: перебираем все позиции и все возможные символы замены (строчные буквы и < > /),
// для каждого варианта проверяем корректность строки стеком открытых тегов.
// Проверка за O(n), вариантов 29 * n, итого O(n^2) - при n <= 1000 это мгновенно.
package main

import (
	"fmt"
	"os"
	"strings"

	"laba2go/lab1"
	"laba2go/shared"
)

// Корректна ли строка из тегов <имя> и </имя>: каждый закрывающий тег должен
// закрывать последний открытый, в конце стек должен быть пуст.
func isValidXml(s string) bool {
	st := lab1.NewStack()
	n := len(s)
	i := 0
	for i < n {
		if s[i] != '<' {
			return false
		}
		i++
		closing := i < n && s[i] == '/'
		if closing {
			i++
		}
		begin := i
		for i < n && s[i] >= 'a' && s[i] <= 'z' { // имя тега: непустая строка из a-z
			i++
		}
		if i == begin || i >= n || s[i] != '>' {
			return false
		}
		name := s[begin:i]
		i++
		if !closing {
			st.Push(name) // открывающий тег - в стек
		} else {
			if st.Empty() || st.Top() != name { // закрывающий должен совпасть с вершиной
				return false
			}
			st.Pop()
		}
	}
	return st.Empty()
}

func main() {
	text := shared.ReadInput(os.Args[1:])
	if eol := strings.IndexByte(text, '\n'); eol >= 0 {
		text = text[:eol]
	}
	var sb strings.Builder // пробелы между тегами (как в примерах задания) отбрасываем
	for i := 0; i < len(text); i++ {
		if c := text[i]; c != ' ' && c != '\r' && c != '\t' {
			sb.WriteByte(c)
		}
	}
	s := []byte(sb.String())
	hadSpaces := len(s) != len(text)

	const alphabet = "abcdefghijklmnopqrstuvwxyz</>"
	// Позиции перебираем с конца: если исправить можно и открывающий, и закрывающий тег
	// (<a></b>), выбирается исправление закрывающего тега - <a></a>.
	for i := len(s) - 1; i >= 0; i-- {
		original := s[i]
		for j := 0; j < len(alphabet); j++ {
			c := alphabet[j]
			if c == original { // символ именно заменен на другой
				continue
			}
			s[i] = c
			if isValidXml(string(s)) {
				var out strings.Builder // в исходном виде: если были пробелы, ставим их между тегами
				for _, ch := range s {
					out.WriteByte(ch)
					if hadSpaces && ch == '>' {
						out.WriteByte(' ')
					}
				}
				fmt.Println(strings.TrimRight(out.String(), " "))
				return
			}
		}
		s[i] = original
	}
	fmt.Println("Incorrect")
}
