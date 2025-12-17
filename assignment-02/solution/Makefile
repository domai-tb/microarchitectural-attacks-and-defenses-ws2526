CC := gcc
CFLAGS := -O4 -Wall -I.

AES_DIR := ../material/AES
AES_DEP := $(AES_DIR)/aes.c $(AES_DIR)/aes_core.c

C_BASENAMES := task1 task2 task3 task4

DEP_task3 := $(AES_DEP)
DEP_task4 := $(AES_DEP)

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

.PHONY: clean $(C_BASENAMES)