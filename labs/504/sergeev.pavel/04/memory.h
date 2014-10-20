#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>

void mem_init(FILE* input);
void mem_deinit();

int mem_translate_address(unsigned address, unsigned* physical_address);

#endif