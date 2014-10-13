#include "auxiliary.h"

#include <stdio.h> // perror, fgets, printf, fprintf
#include <string.h> // strlen, strcpy
#include <stdlib.h> // exit, malloc


char** split_str(char* str, size_t* arg_number)
{
	char **splitted_str;
	size_t argsnum = 0;
	
	size_t i = 0;
	char ch;
	while (str[i] == ' ' && str[i] != '\0')
		++i;
	if (str[i] != '\0')
		++argsnum;
    while(str[i] != '\0')
    {
		ch = str[i++];
		if (ch == '\"' ) {
			while( str[i++] != '\"' );
		} else if (ch == '\'' ) {
			while( str[i++] != '\'' );
		} else if( ch == ' ') {
			if (str[i-2] != ' ') {
				++argsnum;
			}
		} else if ( ch == '\n') {
			str[--i] = '\0';
			break;
		}
	}
	if (str[i-1] == ' ')
		--argsnum;
		
	splitted_str = (char**) malloc(sizeof(char*)*(argsnum+1));
	splitted_str[argsnum] = NULL;
	
	size_t arg = 0, argindex = 0;
	size_t currarglen;
	i = 0;
	while (str[i] == ' ' && str[i+1] != '\0')
		++i;
    while( str[i] != '\0')
    {
		ch = str[i];
		argindex = 0;
		currarglen = 0;
	
		switch (ch)
		{
		case '\"': 
		case'\'':
			++i;
			while( str[i] != ch && str[i] != '\0')
			{
				if (str[i+1] != '\0') break;
				++i;
				++currarglen;
			}
			
			splitted_str[arg] = (char*)malloc(currarglen+1);
			i -= currarglen;
			splitted_str[arg][argindex++] = ch; 
			while( str[i] != '\0' && str[i] != ch )
				splitted_str[arg][argindex++] = str[i++]; 
			if ( str[i] != '\0' )
				splitted_str[arg][argindex++] = str[i++]; 
			splitted_str[arg++][argindex] = '\0';
			break;
		case ' ':
			while (str[i] == ' ')
				++i;
			break;
		default:
			if ( arg < argsnum ) {
				while( str[i] != ' ' && str[i] != '\0' )
				{
					++i;
					++currarglen;
				}
				splitted_str[arg] = (char*)malloc(currarglen+1);
				i -= currarglen;
				while( str[i] != ' ' && str[i] != '\0' )
				{
					splitted_str[arg][argindex++] = str[i++]; 
				}
				splitted_str[arg++][argindex] = '\0';
				break;
			} else {
				++i;
			}
			
		}
	}
	
	*arg_number = argsnum;
	
	
	return splitted_str;
}


void free_commandlist(char** commandlist, size_t commandsnum)
{
	size_t i = 0;
	while (i < commandsnum+1)
		free(commandlist[i++]);
	free(commandlist);
}
