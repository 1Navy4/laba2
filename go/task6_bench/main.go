// Задание 6.1. Эмпирический анализ: линейное апробирование и двойное хеширование.
// Задание 6.2. Эффект фильтра Блума, встроенного в таблицу.
// Запуск: ./task6_bench [--cap N] [--seed S] [--samples M]
//
// Измеряется среднее число проб (просмотренных ячеек) - оно не зависит от компьютера.
// Ключи случайные: присутствующие начинаются с 'k', отсутствующие с 'z', поэтому не пересекаются.
package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"

	"laba2go/shared"
)

var rng *rand.Rand

func randomKey(prefix byte) string {
	b := make([]byte, 13)
	b[0] = prefix
	for i := 1; i < len(b); i++ {
		b[i] = byte('a' + rng.Intn(26))
	}
	return string(b)
}

// Теоретические средние числа проб (Кнут) при коэффициенте заполнения a.
func theorySuccess(m shared.Method, a float64) float64 {
	if m == shared.Linear {
		return 0.5 * (1 + 1/(1-a))
	}
	return math.Log(1/(1-a)) / a
}

func theoryFail(m shared.Method, a float64) float64 {
	if m == shared.Linear {
		return 0.5 * (1 + 1/((1-a)*(1-a)))
	}
	return 1 / (1 - a)
}

func analyze(m shared.Method, capacity, samples int) {
	if m == shared.Linear {
		fmt.Println("Линейное апробирование  (ячеек:", capacity, ")")
	} else {
		fmt.Println("Двойное хеширование  (ячеек:", capacity, ")")
	}
	fmt.Println(" заполнение | вставка | успешный поиск (теория) | неуспешный поиск (теория)")
	fmt.Println("------------+---------+-------------------------+--------------------------")
	t := shared.NewHashTable(m, capacity, false, false)
	var keys []string
	for pct := 10; pct <= 90; pct += 10 {
		target, inserted := capacity*pct/100, 0
		before := t.Probes
		for len(keys) < target {
			k := randomKey('k')
			if t.Put(k, "v") {
				keys = append(keys, k)
				inserted++
			}
		}
		ins := float64(t.Probes-before) / float64(inserted)

		before = t.Probes
		for i := 0; i < samples; i++ {
			t.Get(keys[rng.Intn(len(keys))])
		}
		succ := float64(t.Probes-before) / float64(samples)

		before = t.Probes
		for i := 0; i < samples; i++ {
			t.Get(randomKey('z'))
		}
		unsucc := float64(t.Probes-before) / float64(samples)

		a := float64(len(keys)) / float64(capacity)
		fmt.Printf("    %3d%%    | %7.2f | %9.2f (%6.2f)      | %9.2f (%7.2f)\n",
			pct, ins, succ, theorySuccess(m, a), unsucc, theoryFail(m, a))
	}
	fmt.Println()
}

func bloomEffect(capacity, samples int) {
	fmt.Printf("Фильтр Блума в таблице с двойным хешированием (%d ячеек, 10 бит на ячейку, 4 хеш-функции)\n", capacity)
	fmt.Println(" заполнение | проб на неуспешный поиск: без фильтра -> с фильтром | отсеяно фильтром | ложных срабатываний")
	fmt.Println("------------+------------------------------------------------------+------------------+--------------------")
	for _, pct := range []int{30, 50, 70} {
		plain := shared.NewHashTable(shared.Double, capacity, false, false)
		withBloom := shared.NewHashTable(shared.Double, capacity, false, true)
		n := capacity * pct / 100
		for i := 0; i < n; {
			k := randomKey('k')
			if plain.Put(k, "v") {
				withBloom.Put(k, "v")
				i++
			}
		}
		absent := make([]string, samples)
		for i := range absent {
			absent[i] = randomKey('z')
		}
		plain.ResetCounters()
		withBloom.ResetCounters()
		for _, k := range absent {
			plain.Get(k)
			withBloom.Get(k)
		}
		s := float64(samples)
		fmt.Printf("    %3d%%    | %22.3f  ->  %10.3f         | %8.2f%%        | %8.3f%%\n",
			pct, float64(plain.Probes)/s, float64(withBloom.Probes)/s,
			100*float64(withBloom.BloomRejects)/s, 100*float64(withBloom.BloomFalse)/s)
	}
	fmt.Println()
}

func main() {
	capacity, samples, seed := 100003, 20000, int64(12345)
	args := os.Args
	for i := 1; i+1 < len(args); i += 2 {
		v, err := strconv.ParseInt(args[i+1], 10, 64)
		if err != nil {
			continue
		}
		switch args[i] {
		case "--cap":
			capacity = int(v)
		case "--seed":
			seed = v
		case "--samples":
			samples = int(v)
		}
	}
	rng = rand.New(rand.NewSource(seed))
	fmt.Printf("Среднее число проб (просмотренных ячеек), %d запросов на каждый уровень заполнения\n\n", samples)
	analyze(shared.Linear, capacity, samples)
	analyze(shared.Double, capacity, samples)
	bloomEffect(capacity, samples)
}
