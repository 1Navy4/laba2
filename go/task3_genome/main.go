// Задание 3 (вариант 5). Инопланетный геном. Использует множество (HashSet) из задания 2.
// Ввод: две строки из заглавных латинских букв (stdin или --file путь).
// Вывод: степень близости первого генома второму - количество пар соседних оснований
// первого генома, которые встречаются во втором геноме.
//
// Алгоритм: все пары соседних оснований второго генома кладем в множество (O(m)),
// затем для каждой пары первого генома проверяем принадлежность множеству (O(1)).
// Итого O(n + m) вместо O(n * m) при сравнении "каждая с каждой".
package main

import (
	"fmt"
	"os"
	"strings"

	"laba2go/shared"
)

func onlyLetters(s string) bool {
	for i := 0; i < len(s); i++ {
		if s[i] < 'A' || s[i] > 'Z' {
			return false
		}
	}
	return true
}

func main() {
	lines := strings.Split(shared.ReadInput(os.Args[1:]), "\n")
	first, second := "", ""
	if len(lines) > 0 {
		first = strings.TrimSpace(lines[0])
	}
	if len(lines) > 1 {
		second = strings.TrimSpace(lines[1])
	}
	if first == "" || second == "" || !onlyLetters(first) || !onlyLetters(second) {
		fmt.Fprintln(os.Stderr, "ERROR: нужны две непустые строки из заглавных латинских букв A-Z")
		os.Exit(1)
	}

	pairs := shared.NewHashSet() // пары второго генома
	for i := 0; i+1 < len(second); i++ {
		pairs.Add(second[i : i+2])
	}

	closeness := 0
	for i := 0; i+1 < len(first); i++ { // каждая пара первого генома (с повторами)
		if pairs.Contains(first[i : i+2]) {
			closeness++
		}
	}
	fmt.Println(closeness)
}
