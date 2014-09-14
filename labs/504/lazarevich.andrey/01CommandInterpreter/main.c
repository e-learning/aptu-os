#include "common.h"
#include <string.h>
int main (int argc, char **argv)
{
	char			*command_name;
	const char		ls_command[] = "ls",
					ps_command[] = "ps",
					pwd_command[] = "pwd",
					kill_command[] = "kill",
					exit_command[] = "exit";

	const size_t	ls_command_size = 2,
					ps_command_size = 2,
					pwd_command_size = 3,
					kill_command_size = 4,
					exit_command_size = 4;
								
	size_t			read_bytes;
	size_t			command_size = 16;

	command_name = (char *) malloc(command_size + 1);

	printf("Weclome to command interpreter. Now 5 commands available(ls, pwd, ps, kill, exit):\n");
	while (1)
	{
		read_bytes = getline(&command_name, &command_size, stdin);
		if ( read_bytes == -1 )
			exit(1);
		else
			if ( strncmp( ls_command, command_name, ls_command_size) == 0)
				ls_run();
			else
 				if ( strncmp( ps_command, command_name, ps_command_size) == 0)
					ps_run();
				else
					if ( strncmp( pwd_command, command_name, pwd_command_size) == 0)
						pwd_run();
				else
					if ( strncmp( kill_command, command_name, kill_command_size) == 0)
						kill_run(command_name);
					else
						if ( strncmp( exit_command, command_name, exit_command_size) == 0)
						{
							printf("Thank you, good bye\n");				
							break;
						}
						else
							try_run_programm(command_name);
	}
	return 0;
	
}
