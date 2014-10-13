#include <stdio.h> // printf
#include <stdlib.h> // malloc, free
#include <stddef.h> // size_t
#include <stdlib.h> // atol

#include "commands.h"
#include "auxiliary.h" // split_str

#define MAXARGSLEN 32

int main()
{
	void * buffer;
	size_t buffer_size;
	char** commandlist;
    size_t commandsnum; 
	char commandbuf[MAXARGSLEN+1];
	
	printf("Hello! Please input buffer size:\n > ");
	if(scanf("%zu", &buffer_size) != 1)
	{
		printf("Wrong buffer size! Sorry, good bye.\n");
		return 1;
	}

	buffer = init_buffer(buffer_size);
	if (buffer == NULL)
	{
		printf("Cannot allocate buffer with size %zu B (too small or too big)\n", buffer_size);
		return 1;
	}
	
	printf("Type 'exit' for quit or type 'help' to know how to use.\n");
	while(1)
	{
		if (fgets(commandbuf, MAXARGSLEN+1, stdin) && strlen(commandbuf) > 1)
		{
			commandlist = split_str(commandbuf, &commandsnum);
			if (commandsnum > 0)
			{
				if(strcmp(commandlist[0], "help") == 0) {
					show_help();
				} else if (strcmp(commandlist[0], "ALLOC") == 0 || strcmp(commandlist[0], "alloc") == 0 ) {
					if (commandsnum != 2)
					{
						printf("Error: ALLOC must have the one positive integer argument.\n");
						free_commandlist(commandlist, commandsnum);
						continue;
					}
					long alloc_size = atol(commandlist[1]); 
					if (alloc_size <= 0)
					{
						printf("Error: ALLOC argument must be positive integer.\n");
						free_commandlist(commandlist, commandsnum);
						continue;
					}
					size_t res = my_alloc(buffer, (size_t)alloc_size);
					if ( res == 0 )
					{
						printf("-\n");
						continue;
					}
					printf("+ %zu\n", res);
				} else if (strcmp(commandlist[0], "FREE") == 0 || strcmp(commandlist[0], "free") == 0 ) {
					if (commandsnum != 2)
					{
						printf("Error: FREE must have the one positive integer argument.\n");
						free_commandlist(commandlist, commandsnum);
						continue;
					}
					long free_address = atol(commandlist[1]); 
					if (free_address <= 0)
					{
						printf("Error: FREE argument must be positive integer.\n");
						free_commandlist(commandlist, commandsnum);
						continue;
					}
					if (my_free(buffer, (size_t)free_address) == 0)
						printf("+\n");
					else
						printf("-\n");
				} else if (strcmp(commandlist[0], "INFO") == 0 || strcmp(commandlist[0], "info") == 0 ) {
					show_info(buffer, buffer_size);
				} else if (strcmp(commandlist[0], "MAP") == 0 || strcmp(commandlist[0], "map") == 0 ) {
					show_map(buffer, buffer_size);
				} else if (strcmp(commandlist[0], "EXIT") == 0 || strcmp(commandlist[0], "exit") == 0 ) {
					printf("Thank you for using this allocator! Good bye.\n");
					break;
				} else {
					printf("Unknown command: %s\n", commandlist[0]);
				}
			}
			
			free_commandlist(commandlist, commandsnum);
		}	
		printf(" > ");
	}
	
	free(buffer);
	
	return 0;
}
