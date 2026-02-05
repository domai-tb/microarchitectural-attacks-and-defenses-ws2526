CC := gcc
CFLAGS := -O4 -Wall -I.

C_BASENAMES := task1 task2 task3 task4 task5

DEP_task2 := ./task1/task1.c
DEP_task3 := ./task1/task1.c ./task2/task2.c
DEP_task4 := ./task1/task1.c ./task2/task2.c ./task3/task3.c
DEP_task5 := ./task1/task1.c ./task2/task2.c ./task3/task3.c

all: build_everthing

$(C_BASENAMES):
	$(CC) $(CFLAGS) ./$@/$@.c ./$@/$@_test.c $(DEP_$@) -o ./$@/$@.bin 2>/dev/null || \
	$(CC) $(CFLAGS) ./$@/$@.c $(DEP_$@) -o ./$@/$@.bin

clean:
	rm **/*.bin

pdf:
	pandoc --pdf-engine=xelatex solution.md -o answers.pdf

build_everthing:
	make task1
	make task2
	make task3
	make task4
	make task5

.PHONY: clean $(C_BASENAMES)
