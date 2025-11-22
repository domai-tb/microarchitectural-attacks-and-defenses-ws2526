#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdint.h>

void *map(char *file_name, uint64_t offset);
uint32_t flush_reload(void *p);
int monitor(void* p, uint32_t period, uint64_t maxwait);
void fr_trace(void *p1, void *p2, uint32_t period, uint32_t count, uint32_t *trace);

#endif
