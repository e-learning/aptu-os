#include <stdio.h>
#include <limits.h>

#include "memory.h"

int main()
{
	FILE* input = stdin;
	unsigned addr;
	unsigned raw_add;
	fscanf(input, "%x", &addr);
	mem_init(input);
	if (mem_translate_address(addr, &raw_add) == 0)
	{
		printf("%x\n", raw_add);
	}
	else
	{
		printf("INVALID\n");
	}
	mem_deinit();
	return 0;
}