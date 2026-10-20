// Задание 2. АТД Множество. Все операции выполняются за O(1) в среднем.
// Запуск: ./task2_set --file data.txt --query 'SETADD s 5'
// Команды: SETADD имя v - добавить, SETDEL имя v - удалить, SET_AT имя v - есть ли элемент,
//          PRINT имя - показать множество. Файл: по строке на множество "H <имя> <элементы>".
package main

import (
	"fmt"
	"os"
	"sort"
	"strings"

	"laba2go/lab1"
	"laba2go/shared"
)

func loadSets(sets map[string]*shared.HashSet, path string) {
	data, err := os.ReadFile(path)
	if err != nil { // нет файла - пустая база
		return
	}
	for _, line := range strings.Split(string(data), "\n") {
		tok := lab1.Tokenize(line)
		if len(tok) < 2 || tok[0] != "H" {
			continue
		}
		s := shared.NewHashSet()
		s.Load(tok[2:])
		sets[tok[1]] = s
	}
}

func saveSets(sets map[string]*shared.HashSet, path string) {
	names := make([]string, 0, len(sets))
	for name := range sets {
		names = append(names, name)
	}
	sort.Strings(names)
	var sb strings.Builder
	for _, name := range names {
		sb.WriteString("H " + lab1.Quote(name))
		for _, v := range sets[name].Dump() {
			sb.WriteString(" " + lab1.Quote(v))
		}
		sb.WriteString("\n")
	}
	if err := os.WriteFile(path, []byte(sb.String()), 0644); err != nil {
		lab1.Fail("cannot write file: " + path)
	}
}

func find(sets map[string]*shared.HashSet, name string, create bool) *shared.HashSet {
	s, ok := sets[name]
	if !ok {
		if !create {
			lab1.Fail("no such set: " + name)
		}
		s = shared.NewHashSet()
		sets[name] = s
	}
	return s
}

func yesNo(b bool) string {
	if b {
		return "TRUE"
	}
	return "FALSE"
}

func execute(sets map[string]*shared.HashSet, a []string) string {
	cmd := strings.ToUpper(a[0])
	name := lab1.Arg(a, 1)
	switch cmd {
	case "SETADD":
		return yesNo(find(sets, name, true).Add(lab1.Arg(a, 2)))
	case "SETDEL":
		return yesNo(find(sets, name, false).Remove(lab1.Arg(a, 2)))
	case "SET_AT":
		return yesNo(find(sets, name, false).Contains(lab1.Arg(a, 2)))
	case "PRINT":
		return find(sets, name, false).Show()
	}
	lab1.Fail("unknown command: " + a[0])
	return ""
}

func main() {
	defer shared.CatchErrors()
	file, query, hasQuery := "file.data", "", false
	args := os.Args
	for i := 1; i+1 < len(args); i += 2 {
		switch args[i] {
		case "--file":
			file = args[i+1]
		case "--query":
			query, hasQuery = args[i+1], true
		}
	}
	if !hasQuery {
		fmt.Fprintf(os.Stderr, "usage: %s --file file.data --query 'COMMAND name value'\n", args[0])
		os.Exit(2)
	}
	sets := map[string]*shared.HashSet{}
	loadSets(sets, file)
	a := lab1.Tokenize(query)
	if len(a) == 0 {
		lab1.Fail("empty query")
	}
	fmt.Println(execute(sets, a))
	if c := strings.ToUpper(a[0]); c == "SETADD" || c == "SETDEL" { // файл меняют только добавление и удаление
		saveSets(sets, file)
	}
}
