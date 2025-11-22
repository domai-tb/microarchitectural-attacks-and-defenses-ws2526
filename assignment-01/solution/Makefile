CC := gcc
CFLAGS := -O0 -Wall -I.

C_BASENAMES := task1 task2 task3 task4 task5 task6

DEP_task2 := ./task1/task1.c
DEP_task3 := $(DEP_task2) ./task2/task2.c 
DEP_task4 := $(DEP_task3) ./task3/task3.c 
DEP_task5 := $(DEP_task2) ./task2/task2.c 
DEP_task6 := $(DEP_task4) ./task5/task5.c

all: build_everthing

$(C_BASENAMES):
	$(CC) $(CFLAGS) ./$@/$@.c ./$@/$@_test.c $(DEP_$@) -o ./$@/$@.bin 2>/dev/null || \
	$(CC) $(CFLAGS) ./$@/$@.c $(DEP_$@) -o ./$@/$@.bin

clean:
	rm **/*.bin

pdf:
	pandoc solution.md -o answers.pdf

build_everthing:
	make task1
	make task2
	make task3
	make task4
	make task5
	make task6
	cp ./task5/task5.bin ./task5/fr_trace


.PHONY: clean $(C_BASENAMES)