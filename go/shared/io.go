// Ввод данных для программ ЛР2 и обработка ошибок.
package shared

import (
	"fmt"
	"io"
	"os"

	"laba2go/lab1"
)

// ReadInput читает весь ввод: из файла (--file путь) или из стандартного ввода.
func ReadInput(args []string) string {
	for i := 0; i+1 < len(args); i++ {
		if args[i] == "--file" {
			data, err := os.ReadFile(args[i+1])
			if err != nil {
				fmt.Fprintln(os.Stderr, "ERROR: cannot open file:", args[i+1])
				os.Exit(1)
			}
			return string(data)
		}
	}
	data, _ := io.ReadAll(os.Stdin)
	return string(data)
}

// CatchErrors печатает "ERROR: ..." вместо падения программы. Вызывается как: defer shared.CatchErrors()
func CatchErrors() {
	if r := recover(); r != nil {
		if e, ok := r.(lab1.DbError); ok {
			fmt.Fprintln(os.Stderr, "ERROR:", string(e))
			os.Exit(1)
		}
		panic(r)
	}
}
