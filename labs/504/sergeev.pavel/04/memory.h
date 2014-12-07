#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void mem_init(FILE* input);
void mem_deinit();

int32_t mem_translate_address(uint32_t logical_address, uint32_t* physical_address);

#endif