// Задание 4 (вариант 5). Контрольная по ударениям. Массив из ЛР1 и множество из задания 2.
// Ввод (stdin или --file путь): N, затем N слов словаря (заглавная буква - ударение),
// затем строка текста с ударениями, расставленными Петей.
// Вывод: количество ошибок, найденных Васей.
//
// Правила: слово есть в словаре (без учета регистра) - оно должно совпасть с одним из
// вариантов ударения из словаря; слова нет в словаре - оно верно, только если в нем
// ровно одна заглавная буква.
//
// Алгоритм: словарь кладем в два множества - варианты как записаны и слова в нижнем регистре.
// Для каждого слова текста проверка за O(1), всего O(N + длина текста).
package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"

	"laba2go/lab1"
	"laba2go/shared"
)

func upperCount(s string) int {
	n := 0
	for i := 0; i < len(s); i++ {
		if s[i] >= 'A' && s[i] <= 'Z' {
			n++
		}
	}
	return n
}

func main() {
	tok := strings.Fields(shared.ReadInput(os.Args[1:]))
	if len(tok) == 0 {
		fmt.Fprintln(os.Stderr, "ERROR: в первой строке должно быть число слов словаря N")
		os.Exit(1)
	}
	n, err := strconv.Atoi(tok[0])
	if err != nil || n < 0 {
		fmt.Fprintln(os.Stderr, "ERROR: в первой строке должно быть число слов словаря N")
		os.Exit(1)
	}
	if len(tok)-1 < n {
		fmt.Fprintln(os.Stderr, "ERROR: в словаре меньше слов, чем N")
		os.Exit(1)
	}

	dict := lab1.NewArray() // слова словаря
	for _, w := range tok[1 : 1+n] {
		dict.Push(w)
	}
	text := lab1.NewArray() // слова текста Пети
	for _, w := range tok[1+n:] {
		text.Push(w)
	}

	variants, words := shared.NewHashSet(), shared.NewHashSet()
	for i := 0; i < dict.Size(); i++ {
		variants.Add(dict.Get(i))                  // как записано: cAnnot
		words.Add(strings.ToLower(dict.Get(i))) // без ударений: cannot
	}

	errors := 0
	for i := 0; i < text.Size(); i++ {
		w := text.Get(i)
		if words.Contains(strings.ToLower(w)) {
			if !variants.Contains(w) { // слово из словаря, но ударение не из списка
				errors++
			}
		} else if upperCount(w) != 1 { // слова нет в словаре: нужно ровно одно ударение
			errors++
		}
	}
	fmt.Println(errors)
}
