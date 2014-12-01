#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "constants.h"
#include "parse_input.h"
#include "execute.h"
#include "kill_by_pid.h"
#include "handler.h"

typedef void (*fun)(void);

int main()
{
	int i;
	fun ptr_array[5] = {normal, out_dir, in_dir, pip,exe_cu};

	printf("$ ");
	while(fgets(inp_buf, sizeof(inp_buf), stdin) != NULL) {
			i = parse();
			(*ptr_array[i-1])();
			printf("$ ");
	}
	return EXIT_SUCCESS;
}
