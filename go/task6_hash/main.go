// Задание 6 (6.1 и 6.2). Хеш-таблица с открытой адресацией и встроенным фильтром Блума (вариант 8).
// Запуск: ./task6_hash --file data.txt --query 'HSET имя ключ значение'
// Команды:
//   HNEW имя linear|double   создать пустую таблицу с выбранным методом (линейное апробирование / двойное хеширование)
//   HSET имя ключ значение   добавить или обновить (если таблицы нет - создается с двойным хешированием)
//   HGET имя ключ            значение по ключу или NOT FOUND (сначала проверяет фильтр Блума)
//   HHAS имя ключ            есть ли ключ: TRUE или FALSE
//   HDEL имя ключ            удалить ключ: TRUE или FALSE
//   HLEN имя                 число элементов
//   PRINT имя                таблица по ячейкам
//   HSTATS имя               параметры таблицы и фильтра Блума
// Файл: по строке на таблицу "HT <имя> <метод> <размер> <живых> <надгробий> <ячейки...>".
package main

import (
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"

	"laba2go/lab1"
	"laba2go/shared"
)

func loadTables(tables map[string]*shared.HashTable, path string) {
	data, err := os.ReadFile(path)
	if err != nil { // нет файла - пустая база
		return
	}
	for _, line := range strings.Split(string(data), "\n") {
		tok := lab1.Tokenize(line)
		if len(tok) < 2 || tok[0] != "HT" {
			continue
		}
		t := shared.NewHashTable(shared.Double, 11, true, true)
		t.Load(tok[2:])
		tables[tok[1]] = t
	}
}

func saveTables(tables map[string]*shared.HashTable, path string) {
	names := make([]string, 0, len(tables))
	for name := range tables {
		names = append(names, name)
	}
	sort.Strings(names)
	var sb strings.Builder
	for _, name := range names {
		sb.WriteString("HT " + lab1.Quote(name))
		for _, s := range tables[name].Dump() {
			sb.WriteString(" " + lab1.Quote(s))
		}
		sb.WriteString("\n")
	}
	if err := os.WriteFile(path, []byte(sb.String()), 0644); err != nil {
		lab1.Fail("cannot write file: " + path)
	}
}

func find(tables map[string]*shared.HashTable, name string, create bool) *shared.HashTable {
	t, ok := tables[name]
	if !ok {
		if !create {
			lab1.Fail("no such table: " + name)
		}
		t = shared.NewHashTable(shared.Double, 11, true, true)
		tables[name] = t
	}
	return t
}

func yesNo(b bool) string {
	if b {
		return "TRUE"
	}
	return "FALSE"
}

func execute(tables map[string]*shared.HashTable, a []string) string {
	cmd := strings.ToUpper(a[0])
	name := lab1.Arg(a, 1)
	switch cmd {
	case "HNEW":
		m := lab1.Arg(a, 2)
		if m != "linear" && m != "double" {
			lab1.Fail("method must be linear or double")
		}
		if _, exists := tables[name]; exists {
			lab1.Fail("table already exists: " + name)
		}
		method := shared.Double
		if m == "linear" {
			method = shared.Linear
		}
		tables[name] = shared.NewHashTable(method, 11, true, true)
		return name
	case "HSET":
		find(tables, name, true).Put(lab1.Arg(a, 2), lab1.Arg(a, 3))
		return a[3]
	case "HGET":
		if v, ok := find(tables, name, false).Get(lab1.Arg(a, 2)); ok {
			return v
		}
		return "NOT FOUND"
	case "HHAS":
		return yesNo(find(tables, name, false).Contains(lab1.Arg(a, 2)))
	case "HDEL":
		return yesNo(find(tables, name, false).Erase(lab1.Arg(a, 2)))
	case "HLEN":
		return strconv.Itoa(find(tables, name, false).Size())
	case "PRINT":
		return find(tables, name, false).Show()
	case "HSTATS":
		return find(tables, name, false).Stats()
	}
	lab1.Fail("unknown command: " + a[0])
	return ""
}

func readOnly(c string) bool {
	return c == "HGET" || c == "HHAS" || c == "HLEN" || c == "PRINT" || c == "HSTATS"
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
		fmt.Fprintf(os.Stderr, "usage: %s --file file.data --query 'COMMAND name args'\n", args[0])
		os.Exit(2)
	}
	tables := map[string]*shared.HashTable{}
	loadTables(tables, file)
	a := lab1.Tokenize(query)
	if len(a) == 0 {
		lab1.Fail("empty query")
	}
	fmt.Println(execute(tables, a))
	if !readOnly(strings.ToUpper(a[0])) {
		saveTables(tables, file)
	}
}
