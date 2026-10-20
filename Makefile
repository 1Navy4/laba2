# Сборка ЛР2 (C++ и Go).
#   make          - собрать C++ и Go версии
#   make cpp      - только C++  (программы в bin/cpp/, объекты в build/)
#   make go       - только Go   (программы в bin/go/)
#   make demo     - собрать всё и запустить demo.sh
#   make clean
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude -MMD -MP
.RECIPEPREFIX = >

TASKS   := $(patsubst src/%/,%,$(wildcard src/task*/))
LIBSRC  := $(wildcard src/*.cpp)
LIBOBJ  := $(LIBSRC:src/%.cpp=build/%.o)

all: cpp go

cpp: $(addprefix bin/cpp/,$(TASKS))

go:
> @mkdir -p bin/go
> cd go && go build -o ../bin/go/ ./...

build/%.o: src/%.cpp
> @mkdir -p $(dir $@)
> $(CXX) $(CXXFLAGS) -c $< -o $@

build/liblab.a: $(LIBOBJ)
> @rm -f $@
> ar rcs $@ $^

define TASK_RULE
bin/cpp/$(1): $$(patsubst src/%.cpp,build/%.o,$$(wildcard src/$(1)/*.cpp)) build/liblab.a
> @mkdir -p bin/cpp
> $$(CXX) $$(CXXFLAGS) -o $$@ $$^
endef
$(foreach t,$(TASKS),$(eval $(call TASK_RULE,$(t))))

demo: all
> ./demo.sh

clean:
> rm -rf build bin

-include $(shell find build -name '*.d' 2>/dev/null)
.PHONY: all cpp go demo clean
