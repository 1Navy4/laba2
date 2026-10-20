// Задание 7 (вариант 1). LRU-кэш.
// Ввод (stdin или --file путь): емкость кэша cap, число запросов Q и сами запросы:
//     2 8
//     SET 1 2  SET 2 3  SET 1 5  SET 4 5  SET 6 7  GET 4  SET 1 2  GET 3
// (допускается и запись как в условии: "cap = 2, Q = 8" и "Queries = ...").
// Запросы: SET x y - записать значение (при переполнении удаляется давно не использованный элемент),
//          GET x   - вернуть значение или -1, если ключа нет.
// Вывод: результаты всех GET через пробел. Ключ "--trace" печатает состояние кэша после каждого запроса.
package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"

	"laba2go/lab1"
	"laba2go/shared"
)

func isNumber(s string) bool { return s != "" && strings.Trim(s, "0123456789") == "" }

func main() {
	defer shared.CatchErrors()
	trace := false
	for _, a := range os.Args[1:] {
		if a == "--trace" {
			trace = true
		}
	}

	text := strings.NewReplacer(",", " ", "=", " ").Replace(shared.ReadInput(os.Args[1:]))
	t := strings.Fields(text)
	i := 0
	skipWord := func(name string) {
		if i < len(t) && strings.ToLower(t[i]) == name {
			i++
		}
	}
	number := func() int {
		if i >= len(t) || !isNumber(t[i]) {
			fmt.Fprintln(os.Stderr, "ERROR: ожидалось неотрицательное число (емкость и число запросов)")
			os.Exit(1)
		}
		n, _ := strconv.Atoi(t[i])
		i++
		return n
	}
	skipWord("cap")
	capacity := number()
	skipWord("q")
	q := number()
	skipWord("queries")

	cache := NewLRUCache(capacity)
	var results []string
	for n := 0; n < q; n++ {
		if i >= len(t) {
			lab1.Fail("запросов меньше, чем Q")
		}
		cmd := strings.ToUpper(t[i])
		i++
		switch cmd {
		case "SET":
			if i+1 >= len(t) {
				lab1.Fail("для SET нужны ключ и значение")
			}
			key, value := t[i], t[i+1]
			i += 2
			cache.Set(key, value)
			if trace {
				fmt.Println("SET "+key+" "+value+":", cache.Show())
			}
		case "GET":
			if i >= len(t) {
				lab1.Fail("для GET нужен ключ")
			}
			key := t[i]
			i++
			v, found := cache.Get(key)
			if !found {
				v = "-1"
			}
			results = append(results, v)
			if trace {
				fmt.Println("GET "+key+":", v, "("+cache.Show()+")")
			}
		default:
			lab1.Fail("неизвестный запрос: " + t[i-1])
		}
	}
	fmt.Println(strings.Join(results, " "))
}
