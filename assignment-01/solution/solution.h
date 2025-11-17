#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdint.h>

void *map(char *file_name, uint64_t offset);
uint32_t flush_reload(void *p);

#endif
