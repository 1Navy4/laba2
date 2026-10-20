// Хеш-таблица с открытой адресацией (строка -> строка) и встроенным фильтром Блума.
// Два метода разрешения коллизий (вариант 1):
//   Linear - линейное апробирование: i-я проба = (h1 + i) mod N, шаг всегда 1;
//   Double - двойное хеширование:    i-я проба = (h1 + i * h2) mod N, шаг зависит от ключа.
// Размер таблицы N - простое число (для двойного хеширования это гарантирует обход всех ячеек).
// h1 = djb2(ключ) mod N, h2 = 1 + fnv1a(ключ) mod (N - 1) (метод деления).
// Ячейки хранятся в массиве Array из ЛР1: "" - свободна, "\x01" - удалена (надгробие),
// иначе "ключ\x1Fзначение". При заполнении (с надгробиями) более 70% таблица пересобирается: удваивается,
// если живых элементов больше половины, иначе размер сохраняется и просто убираются надгробия.
// Фильтр Блума при этом пересобирается.
package shared

import (
	"fmt"
	"strconv"
	"strings"

	"laba2go/lab1"
)

type Method int

const (
	Linear Method = iota
	Double
)

const sep = "\x1f"     // разделяет ключ и значение внутри ячейки
const deleted = "\x01" // метка удаленной ячейки
const none = -1

type HashTable struct {
	method   Method
	slots    *lab1.Array
	cap      int // число ячеек (простое)
	count    int // живых элементов
	tombs    int // надгробий
	autoGrow bool
	useBloom bool
	bloom    *BloomFilter

	Probes       int64 // сколько ячеек просмотрено (для эмпирического анализа)
	BloomRejects int64 // запросов, отсеянных фильтром без обращения к таблице
	BloomFalse   int64 // фильтр сказал "возможно есть", а в таблице ключа нет
}

func isPrime(n int) bool {
	if n < 2 {
		return false
	}
	for d := 2; d*d <= n; d++ {
		if n%d == 0 {
			return false
		}
	}
	return true
}

func nextPrime(n int) int {
	for !isPrime(n) {
		n++
	}
	return n
}

// Ячейка s хранит ключ key? Сравнение без создания временных строк.
func slotHasKey(s, key string) bool {
	return len(s) > len(key) && s[len(key)] == 0x1f && s[:len(key)] == key
}

func NewHashTable(m Method, capacity int, grow, bloomOn bool) *HashTable {
	h := &HashTable{slots: lab1.NewArray(), autoGrow: grow, useBloom: bloomOn, bloom: NewBloomFilter(64, 4)}
	h.reset(m, capacity)
	return h
}

func (h *HashTable) reset(m Method, capacity int) {
	h.method = m
	if capacity < 3 {
		capacity = 3
	}
	h.cap = nextPrime(capacity)
	for h.slots.Size() > 0 { // очистить массив
		h.slots.Erase(h.slots.Size() - 1)
	}
	for i := 0; i < h.cap; i++ {
		h.slots.Push("")
	}
	h.count = 0
	h.tombs = 0
	h.bloom.Reset(uint64(h.cap) * 10) // 10 бит на ячейку
}

func (h *HashTable) home(key string) int { return int(hashDjb2(key) % uint64(h.cap)) }

func (h *HashTable) stepOf(key string) int {
	if h.method == Linear {
		return 1
	}
	return 1 + int(hashFnv1a(key)%uint64(h.cap-1)) // от 1 до N-1: ячейка не повторится
}

func (h *HashTable) findIndex(key string) int {
	idx, step := h.home(key), h.stepOf(key)
	for n := 0; n < h.cap; n++ {
		h.Probes++
		s := h.slots.Get(idx)
		if s == "" { // свободная ячейка - цепочка кончилась, ключа нет
			return none
		}
		if s != deleted && slotHasKey(s, key) { // надгробия пропускаем, идем дальше
			return idx
		}
		idx = (idx + step) % h.cap
	}
	return none
}

// insertFresh: ключа точно нет, роста нет.
func (h *HashTable) insertFresh(key, value string) {
	idx, step := h.home(key), h.stepOf(key)
	for n := 0; n < h.cap; n++ {
		h.Probes++
		s := h.slots.Get(idx)
		if s == "" || s == deleted {
			if s == deleted {
				h.tombs--
			}
			h.slots.Set(idx, key+sep+value)
			h.count++
			h.bloom.Add(key)
			return
		}
		idx = (idx + step) % h.cap
	}
	lab1.Fail("hash table is full")
}

func (h *HashTable) rehash(newCap int) {
	type pair struct{ key, value string }
	var live []pair
	for i := 0; i < h.cap; i++ {
		s := h.slots.Get(i)
		if s == "" || s == deleted {
			continue
		}
		p := strings.Index(s, sep)
		live = append(live, pair{s[:p], s[p+1:]})
	}
	h.reset(h.method, newCap) // новая пустая таблица и пустой фильтр
	for _, kv := range live {
		h.insertFresh(kv.key, kv.value) // все ключи хешируются заново
	}
}

// Put: true - новый ключ, false - значение обновлено.
func (h *HashTable) Put(key, value string) bool {
	if strings.Contains(key, sep) {
		lab1.Fail("key must not contain the character 0x1F")
	}
	if h.autoGrow && (h.count+h.tombs+1)*10 > h.cap*7 { // слишком много занятых ячеек (живых и надгробий)
		if (h.count+1)*2 > h.cap { // живых много - удвоить, иначе только убрать надгробия
			h.rehash(h.cap * 2)
		} else {
			h.rehash(h.cap)
		}
	}
	idx, step, firstFree := h.home(key), h.stepOf(key), none
	for n := 0; n < h.cap; n++ {
		h.Probes++
		s := h.slots.Get(idx)
		if s == "" { // ключа нет
			if firstFree == none {
				firstFree = idx
			}
			break
		}
		if s == deleted {
			if firstFree == none { // запомним первое надгробие
				firstFree = idx
			}
		} else if slotHasKey(s, key) {
			h.slots.Set(idx, key+sep+value) // ключ уже есть: обновляем
			return false
		}
		idx = (idx + step) % h.cap
	}
	if firstFree == none {
		lab1.Fail("hash table is full")
	}
	if h.slots.Get(firstFree) == deleted {
		h.tombs--
	}
	h.slots.Set(firstFree, key+sep+value)
	h.count++
	h.bloom.Add(key)
	return true
}

// Get: значение по ключу и признак, найден ли ключ.
func (h *HashTable) Get(key string) (string, bool) {
	if h.useBloom && !h.bloom.MightContain(key) { // фильтр: точно нет
		h.BloomRejects++
		return "", false
	}
	idx := h.findIndex(key)
	if idx == none {
		if h.useBloom {
			h.BloomFalse++ // ложное срабатывание
		}
		return "", false
	}
	s := h.slots.Get(idx)
	return s[strings.Index(s, sep)+1:], true
}

func (h *HashTable) Contains(key string) bool {
	_, ok := h.Get(key)
	return ok
}

func (h *HashTable) Erase(key string) bool {
	idx := h.findIndex(key)
	if idx == none {
		return false
	}
	h.slots.Set(idx, deleted) // нельзя просто очистить ячейку: оборвется цепочка проб других ключей
	h.count--
	h.tombs++
	return true
}

func (h *HashTable) Size() int          { return h.count }
func (h *HashTable) Capacity() int      { return h.cap }
func (h *HashTable) MethodOf() Method   { return h.method }
func (h *HashTable) ResetCounters()     { h.Probes, h.BloomRejects, h.BloomFalse = 0, 0, 0 }

// Dump (для записи в файл): метод, размер, число живых, число надгробий,
// затем тройки (индекс ключ значение), затем индексы надгробий.
func (h *HashTable) Dump() []string {
	var live, tomb []string
	for i := 0; i < h.cap; i++ {
		s := h.slots.Get(i)
		if s == "" {
			continue
		}
		if s == deleted {
			tomb = append(tomb, strconv.Itoa(i))
			continue
		}
		p := strings.Index(s, sep)
		live = append(live, strconv.Itoa(i), s[:p], s[p+1:])
	}
	name := "double"
	if h.method == Linear {
		name = "linear"
	}
	r := []string{name, strconv.Itoa(h.cap), strconv.Itoa(h.count), strconv.Itoa(h.tombs)}
	r = append(r, live...)
	r = append(r, tomb...)
	return r
}

// Load восстанавливает таблицу в точности с той же раскладкой ячеек.
func (h *HashTable) Load(t []string) {
	if len(t) < 4 || (t[0] != "linear" && t[0] != "double") {
		lab1.Fail("corrupted data file")
	}
	c, e1 := strconv.Atoi(t[1])
	nLive, e2 := strconv.Atoi(t[2])
	nTomb, e3 := strconv.Atoi(t[3])
	if e1 != nil || e2 != nil || e3 != nil || nLive < 0 || nTomb < 0 || len(t) != 4+3*nLive+nTomb || !isPrime(c) {
		lab1.Fail("corrupted data file")
	}
	m := Double
	if t[0] == "linear" {
		m = Linear
	}
	h.reset(m, c)
	for k := 0; k < nLive; k++ {
		idx, err := strconv.Atoi(t[4+3*k])
		if err != nil || idx < 0 || idx >= h.cap {
			lab1.Fail("corrupted data file")
		}
		h.slots.Set(idx, t[5+3*k]+sep+t[6+3*k])
		h.bloom.Add(t[5+3*k])
		h.count++
	}
	for k := 0; k < nTomb; k++ {
		idx, err := strconv.Atoi(t[4+3*nLive+k])
		if err != nil || idx < 0 || idx >= h.cap {
			lab1.Fail("corrupted data file")
		}
		h.slots.Set(idx, deleted)
		h.tombs++
	}
}

// Show: параметры и все занятые ячейки.
func (h *HashTable) Show() string {
	r := h.Stats()
	for i := 0; i < h.cap; i++ {
		s := h.slots.Get(i)
		if s == "" {
			continue
		}
		if s == deleted {
			r += "\n[" + strconv.Itoa(i) + "] (deleted)"
			continue
		}
		p := strings.Index(s, sep)
		r += "\n[" + strconv.Itoa(i) + "] " + lab1.Quote(s[:p]) + " => " + lab1.Quote(s[p+1:])
	}
	return r
}

func pct(x float64) string { // процент: целый, а для малых значений с долями
	if x < 0.1 {
		return fmt.Sprintf("%.2f%%", x*100)
	}
	return fmt.Sprintf("%.0f%%", x*100)
}

// Stats: параметры таблицы и фильтра Блума.
func (h *HashTable) Stats() string {
	fill := float64(h.bloom.SetBits()) / float64(h.bloom.Bits())
	fp := 1.0
	for i := 0; i < h.bloom.Hashes(); i++ {
		fp *= fill // оценка ложных срабатываний: (доля битов)^k
	}
	name := "double hashing"
	if h.method == Linear {
		name = "linear probing"
	}
	return "method: " + name + ", capacity: " + strconv.Itoa(h.cap) + ", size: " + strconv.Itoa(h.count) +
		", deleted: " + strconv.Itoa(h.tombs) + ", load: " + pct(float64(h.count)/float64(h.cap)) +
		"\nbloom: " + strconv.FormatUint(h.bloom.Bits(), 10) + " bits, " + strconv.Itoa(h.bloom.Hashes()) +
		" hashes, bits set: " + pct(fill) + ", false positive estimate: " + pct(fp)
}
