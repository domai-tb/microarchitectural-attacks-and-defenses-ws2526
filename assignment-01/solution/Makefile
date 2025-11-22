CC := gcc
CFLAGS := -O0 -Wall -I.

C_BASENAMES := task1 task2 task3 task4

DEP_task2 := ./task1/task1.c
DEP_task3 := $(DEP_task2) ./task2/task2.c 
DEP_task4 := $(DEP_task3) ./task3/task3.c 

$(C_BASENAMES):
	$(CC) $(CFLAGS) ./$@/$@.c ./$@/$@_test.c $(DEP_$@) -o ./$@/$@.bin 2>/dev/null || \
	$(CC) $(CFLAGS) ./$@/$@.c $(DEP_$@) -o ./$@/$@.bin

clean:
	rm **/*.bin

.PHONY: clean $(C_BASENAMES)