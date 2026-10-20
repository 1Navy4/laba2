# Сборка ЛР2: каждое задание (папка taskN_...) - отдельная программа в bin/.
# Структуры из ЛР1 лежат в lab1/, общий код ЛР2 - в shared/. Они собираются в библиотеку.
#   make            - собрать все задания
#   make bin/task1_xml   - собрать одно задание
#   make clean
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Ilab1/include -Ishared/include -MMD -MP
.RECIPEPREFIX = >

TASKS   := $(patsubst %/,%,$(wildcard task*/))
LIBSRC  := $(wildcard lab1/src/*.cpp shared/src/*.cpp)
LIBOBJ  := $(LIBSRC:%.cpp=build/%.o)

all: $(addprefix bin/,$(TASKS))

build/%.o: %.cpp
> @mkdir -p $(dir $@)
> $(CXX) $(CXXFLAGS) -c $< -o $@

build/liblab.a: $(LIBOBJ)
> @rm -f $@
> ar rcs $@ $^

define TASK_RULE
bin/$(1): $$(patsubst %.cpp,build/%.o,$$(wildcard $(1)/*.cpp)) build/liblab.a
> @mkdir -p bin
> $$(CXX) $$(CXXFLAGS) -o $$@ $$^
endef
$(foreach t,$(TASKS),$(eval $(call TASK_RULE,$(t))))

clean:
> rm -rf build bin

-include $(shell find build -name '*.d' 2>/dev/null)
.PHONY: all clean
