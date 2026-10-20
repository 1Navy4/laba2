// Две независимые хеш-функции для строк. Метод деления (хеш % размер таблицы) применяется снаружи.
package shared

func hashDjb2(s string) uint64 {
	h := uint64(5381)
	for i := 0; i < len(s); i++ {
		h = h*33 + uint64(s[i])
	}
	return h
}

func hashFnv1a(s string) uint64 {
	h := uint64(1469598103934665603)
	for i := 0; i < len(s); i++ {
		h ^= uint64(s[i])
		h *= 1099511628211
	}
	return h
}
