// LRU-кэш (Least Recently Used). Строится на структурах из предыдущих работ:
//   DList (ЛР1)     - порядок использования ключей: голова - использован последним, хвост - давнее всех;
//   HashTable (з.6) - быстрый поиск по ключу: ключ -> "номер узла списка" и значение.
// Благодаря этому Set и Get выполняются за O(1) (в среднем).
package main

import (
	"strconv"
	"strings"

	"laba2go/lab1"
	"laba2go/shared"
)

const sep = "\x1f" // разделяет номер узла и значение в записи индекса

type LRUCache struct {
	capacity int
	order    *lab1.DList
	index    *shared.HashTable
	nodes    []*lab1.DNode // номер узла -> узел списка
	freeIds  []int         // освободившиеся номера
}

// NewLRUCache: емкость кэша задается при создании.
func NewLRUCache(capacity int) *LRUCache {
	return &LRUCache{
		capacity: capacity,
		order:    lab1.NewDList(),
		index:    shared.NewHashTable(shared.Double, 11, true, true),
	}
}

func (c *LRUCache) allocId(node *lab1.DNode) int {
	if n := len(c.freeIds); n > 0 {
		id := c.freeIds[n-1]
		c.freeIds = c.freeIds[:n-1]
		c.nodes[id] = node
		return id
	}
	c.nodes = append(c.nodes, node)
	return len(c.nodes) - 1
}

func entryId(entry string) int {
	id, _ := strconv.Atoi(entry[:strings.Index(entry, sep)])
	return id
}

// Set: SET x y.
func (c *LRUCache) Set(key, value string) {
	if c.capacity == 0 {
		return
	}
	if entry, ok := c.index.Get(key); ok { // ключ уже есть: перезаписать значение
		id := entryId(entry)
		c.index.Put(key, strconv.Itoa(id)+sep+value)
		c.order.MoveToHead(c.nodes[id]) // и сделать самым недавним
		return
	}
	if c.index.Size() == c.capacity { // кэш полон: убрать элемент, который дольше всех не запрашивался
		oldKey := c.order.TailNode().Val()
		e, _ := c.index.Get(oldKey)
		c.freeIds = append(c.freeIds, entryId(e))
		c.index.Erase(oldKey)
		c.order.PopTail()
	}
	node := c.order.PushHeadNode(key) // новый элемент - самый недавний
	c.index.Put(key, strconv.Itoa(c.allocId(node))+sep+value)
}

// Get: GET x. Второе значение false, если ключа нет (выводится -1).
func (c *LRUCache) Get(key string) (string, bool) {
	entry, ok := c.index.Get(key)
	if !ok {
		return "", false
	}
	c.order.MoveToHead(c.nodes[entryId(entry)]) // обращение делает ключ самым недавним
	return entry[strings.Index(entry, sep)+1:], true
}

// Show: содержимое от самого давнего к самому недавнему: "2 -> 3, 1 -> 5".
func (c *LRUCache) Show() string {
	var parts []string
	for _, key := range c.order.ItemsReversed() { // от хвоста (давний) к голове (недавний)
		e, _ := c.index.Get(key)
		parts = append(parts, key+" -> "+e[strings.Index(e, sep)+1:])
	}
	if len(parts) == 0 {
		return "(empty)"
	}
	return strings.Join(parts, ", ")
}
