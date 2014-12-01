#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "execute.h"
#include "dir.h"
#include "parse_input.h"
#include "pipes.h"

typedef void (*fun)(void);

int main()
{
	char inp_buf[MAX];
	char cmd1[MAX];
	char cmd2[MAX];

	int i;

	printf("$ ");
	while(fgets(inp_buf, sizeof(inp_buf), stdin) != NULL) {
			i = parse_input(inp_buf, cmd1, cmd2) - 1;
			switch (i) {
			case 0:
				execute1(cmd1);
				break;
			case 1:
				out_dir(cmd1, cmd2);
				break;
			case 2:
				in_dir(cmd1, cmd2);
				break;
			case 3:
				pipes(cmd1, cmd2);
				break;
			case 4:
				execute2(cmd1, cmd2);
				break;
			default:
				puts("ERROR");
			}
			printf("$ ");
	}
	return EXIT_SUCCESS;
}
