// Фильтр Блума: битовый массив и k хеш-функций.
// Add(key) выставляет k битов. MightContain(key): false - ключа точно нет (ошибок нет),
// true - ключ, возможно, есть (бывают ложные срабатывания). Удаление не поддерживается:
// фильтр пересобирается вместе с таблицей. Операции O(k), то есть O(1).
package shared

import (
	"math/bits"
)

type BloomFilter struct {
	words []uint64
	m     uint64 // число бит
	k     int    // число хеш-функций
}

func NewBloomFilter(nbits uint64, hashes int) *BloomFilter {
	b := &BloomFilter{k: hashes}
	b.Reset(nbits)
	return b
}

// Reset задает новый размер и сбрасывает все биты.
func (b *BloomFilter) Reset(nbits uint64) {
	if nbits < 64 {
		nbits = 64
	}
	b.m = nbits
	b.words = make([]uint64, (nbits+63)/64)
}

// i-я хеш-функция строится из двух базовых: g_i = h1 + i * h2 (двойное хеширование).
func (b *BloomFilter) bitIndex(key string, i int) uint64 {
	return (hashDjb2(key) + uint64(i)*(hashFnv1a(key)|1)) % b.m
}

func (b *BloomFilter) Add(key string) {
	for i := 0; i < b.k; i++ {
		bit := b.bitIndex(key, i)
		b.words[bit/64] |= uint64(1) << (bit % 64)
	}
}

func (b *BloomFilter) MightContain(key string) bool {
	for i := 0; i < b.k; i++ {
		bit := b.bitIndex(key, i)
		if (b.words[bit/64]>>(bit%64))&1 == 0 {
			return false // хоть один бит не выставлен - ключа нет
		}
	}
	return true
}

func (b *BloomFilter) Bits() uint64   { return b.m }
func (b *BloomFilter) Hashes() int    { return b.k }

// SetBits: сколько битов выставлено.
func (b *BloomFilter) SetBits() uint64 {
	var n uint64
	for _, w := range b.words {
		n += uint64(bits.OnesCount64(w))
	}
	return n
}
