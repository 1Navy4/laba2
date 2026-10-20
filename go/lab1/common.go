// Общие вспомогательные функции: ошибки, разбор запроса, строки, числа.
package lab1

import (
	"strconv"
	"strings"
)

// Ошибки: fail прерывает выполнение через panic, main ловит ее и печатает "ERROR: ...".
type DbError string

func Fail(msg string) { panic(DbError(msg)) }

func Spaced(v []string) string { // элементы через пробел; строки с пробелами - в кавычках
	if len(v) == 0 {
		return "(empty)"
	}
	q := make([]string, len(v))
	for i, s := range v {
		q[i] = Quote(s)
	}
	return strings.Join(q, " ")
}

func reversed(v []string) []string {
	r := make([]string, len(v))
	for i, s := range v {
		r[len(v)-1-i] = s
	}
	return r
}

// arg возвращает i-й аргумент запроса или завершает программу с ошибкой.
func Arg(a []string, i int) string {
	if i >= len(a) {
		Fail("missing argument for " + a[0])
	}
	return a[i]
}

func ToIndex(s string) int {
	if s == "" || len(s) > 9 || strings.Trim(s, "0123456789") != "" {
		Fail("bad index: " + s)
	}
	n, _ := strconv.Atoi(s)
	return n
}

func isSpace(c byte) bool { return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' }

// quote превращает значение в токен: в двойных кавычках, если есть пробелы, кавычки или оно пустое.
func Quote(s string) string {
	need := s == ""
	for i := 0; i < len(s); i++ {
		if isSpace(s[i]) || s[i] == '"' || s[i] == '\\' {
			need = true
		}
	}
	if !need {
		return s
	}
	var sb strings.Builder
	sb.WriteByte('"')
	for i := 0; i < len(s); i++ {
		if s[i] == '"' || s[i] == '\\' {
			sb.WriteByte('\\')
		}
		sb.WriteByte(s[i])
	}
	sb.WriteByte('"')
	return sb.String()
}

// tokenize разбивает строку на токены. Токен в двойных кавычках может содержать пробелы:
//   MPUSH arr "hello world"   ->   ["MPUSH", "arr", "hello world"]
// Внутри кавычек \" и \\ означают символы " и \.
func Tokenize(s string) []string {
	var r []string
	i, n := 0, len(s)
	for i < n {
		for i < n && isSpace(s[i]) {
			i++
		}
		if i >= n {
			break
		}
		var tok []byte
		if s[i] == '"' {
			i++
			closed := false
			for i < n {
				c := s[i]
				i++
				if c == '\\' && i < n {
					tok = append(tok, s[i])
					i++
				} else if c == '"' {
					closed = true
					break
				} else {
					tok = append(tok, c)
				}
			}
			if !closed {
				Fail("unterminated quote in: " + s)
			}
		} else {
			for i < n && !isSpace(s[i]) {
				tok = append(tok, s[i])
				i++
			}
		}
		r = append(r, string(tok))
	}
	return r
}

// Целые числа (в том числе отрицательные) - для дерева.
func IsInteger(s string) bool {
	i := 0
	if len(s) > 0 && s[0] == '-' {
		i = 1
	}
	if i >= len(s) || len(s)-i > 18 { // пусто или не помещается в int64
		return false
	}
	return strings.Trim(s[i:], "0123456789") == ""
}

// normInt приводит число к обычному виду ("-007" -> "-7"); не число - ошибка.
func NormInt(s string) string {
	if !IsInteger(s) {
		Fail("tree stores integers only, got: " + s)
	}
	n, _ := strconv.ParseInt(s, 10, 64)
	return strconv.FormatInt(n, 10)
}
