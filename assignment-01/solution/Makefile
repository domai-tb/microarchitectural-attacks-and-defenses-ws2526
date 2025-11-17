CC := gcc
CFLAGS := -O0 -Wall -I.

C_BASENAMES := task1 task2

DEP_task2 := task1.c

$(C_BASENAMES):
	$(CC) $(CFLAGS) $@.c $@_test.c $(DEP_$@) -o $@

clean:
	rm $(C_BASENAMES) *.csv

.PHONY: clean