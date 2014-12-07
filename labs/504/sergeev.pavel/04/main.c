#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "memory.h"

int main()
{
    FILE* input = stdin;
    uint32_t logical_address;
    uint32_t physical_address;
    fscanf(input, "%x", &logical_address);
    mem_init(input);
    if (mem_translate_address(logical_address, &physical_address) == 0)
    {
        printf("%x\n", physical_address);
    }
    else
    {
        printf("INVALID\n");
    }
    mem_deinit();
    return 0;
}