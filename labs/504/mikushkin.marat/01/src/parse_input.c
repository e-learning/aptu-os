#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse_input.h"
#include "constants.h"

int parse_input()
{
	int i = 0 ,j = 0, k = 0, val;
	while(inp_buf[i] != '>' && inp_buf[i] != '<' && inp_buf[i] != '|' && inp_buf[i] != '\n' && inp_buf[i] != ' ') {
		cmd1[k++] = inp_buf[i++];
	}
	cmd1[k] = '\0';

	if(inp_buf[i] == '\n') {
			val = 1;
			return val;
	}

	printf("%s",cmd1);
	if(inp_buf[i] == ' ')
		i++;
	if(inp_buf[i] == '>' || inp_buf[i] == '<' || inp_buf[i] == '|') {
		switch(inp_buf[i++]) {
		case '>': val = 2;
		break;
		case '<': val = 3;
		break;
		case '|': val = 4;
		break;
		}
	} else {
		val = 5;
	}

	if(inp_buf[i] == ' ')
		i++;
	while(inp_buf[i] != '\n')
		cmd2[j++] = inp_buf[i++];
	cmd2[j] = '\0';
	printf("%s",cmd2);
	return val;

}
