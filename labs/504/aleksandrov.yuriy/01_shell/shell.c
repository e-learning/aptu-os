#include <stdio.h> // perror, fgets, printf, fprintf

#include "auxiliary.h"
#include "commands.h"
#include <stdlib.h> // free, exit

#define	MAXARGSLEN 200 /* max length of command line's string */


int main()
{
	char** commandlist;
	size_t commandsnum; 
	char commandbuf[MAXARGSLEN+1]; /* read stuff here */
	
	printf("Hello! Please type your command:\n");
	while(1)
	{
		printf(" $ "); // shell prompt
		if (fgets(commandbuf, MAXARGSLEN+1, stdin) && strlen(commandbuf) > 1)
		{
			commandlist = split_str(commandbuf, &commandsnum);
			size_t i = 0;
			if (commandsnum > 0)
			{
				if(strcmp(commandlist[0], "ls") == 0) {
					exec_ls(commandlist+1);
				} else if (strcmp(commandlist[0], "pwd") == 0) {
					exec_pwd();
				} else if ( strcmp(commandlist[0], "ps") == 0) {
					execute(commandlist);
				} else if (strcmp(commandlist[0], "kill") == 0) {
					exec_kill(commandlist+1);
				} else if (strcmp(commandlist[0], "exit") == 0) {
					printf("Thank you for using this shell! Good bye.\n");
					exit(0);
				} else {
					execute(commandlist);
				}
			}
			
			i = 0;
			while (i < commandsnum)
				free(commandlist[i++]);
			free(commandlist);
		}	
	}
	
	return 0;
}








