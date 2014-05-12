#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define size_blocks 20
void Commands()
{
	printf("-------------------------------------\n");
	printf("Allocate a block of memory: 	  'alloc'\n");
	printf("Clear the memory block:  	  'free'\n");
	printf("Information of memory:            'info'\n");
	printf("The contents of the memory block: 'map'\n");
	printf("-------------------------------------\n");
}
	
int wait_print(char *st)
{
	char chr;
	int i = 0;
	while ((chr = getchar()) != '\n')
	{
		st[i] = chr;
		i++;
	}
	st[i] = '\0';
	return i;
}

int empty(char *massiv, int len, int size){
	int i, 
		ch_len, 
		start, 
		go;
	ch_len = len;
	for (i = 0, go = 0; i<size; i++){
		if (massiv[i] == 0)
		{
			if (!go)
			{
				start = i;
				go = 1;
				ch_len++;
			}
			ch_len--;
		}
		else
		{
			ch_len = len;
			start = 0;
			go = 0;
		}

		if (!ch_len)
			return start;
	}
	return -1;
}

int main(){
	char *massiv;
	char *str;
	int size, i, pos, len, mem, blocks, free, go, max;
	char arg;
	char com[size_blocks];

	Commands();
	printf("Input size of the memory field : ");
	scanf("%d", &size);
	massiv = (char*)malloc(size);
	memset(massiv, 0, size);
	if (massiv == NULL)
		return 2;
	while (1){
		wait_print(com);
		if ((str = strstr(com, "alloc")) != NULL){
			str = &str[5];
			sscanf(str, "%d", &arg);
			if ((pos = empty(massiv, arg, size)) != -1){
				massiv[pos] = arg | 0x80;
				printf("+%d\n", pos);
				pos++;
				for (i = 0; i<arg; i++)
					massiv[pos + i] = 'X';
			}
			else  printf("- (%db)\n", arg);
		}
		if ((str = strstr(com, "free")) != NULL){
			str = &str[4];
			sscanf(str, "%d", &arg);
			arg = arg>0 ? arg : 0;
			arg = arg<size ? arg : size - 1;
			if ((massiv[arg] & 0x80) == 0x80){
				len = massiv[arg] & 0x7f;
				len++;
				for (i = arg; i<arg + len; i++){
					massiv[i] = 0;
				}
				printf("+\n");
			}
			else  printf("-\n");
		}
		if ((str = strstr(com, "info")) != NULL){
			max = 0;
			blocks = 0;
			free = 0;
			mem = 0;
			for (i = 0; i<size; i++){
				if (massiv[i] == 0){
					if (!go)
						go = 1;
					free++;
				}
				else{
					go = 0;
					if (free>max)
						max = free;
					free = 0;
					if (massiv[i] == 'X')
						mem++;
					else
						blocks++;
				}
			}
			if (free>max)
				max = free;
			printf("b_m_mx\n");
			printf("%d %d %d\n", blocks, mem, max == 0 ? 0 : max - 1);
		}
		if ((str = strstr(com, "map")) != NULL){
			for (i = 0; i<size; i++){
				switch (massiv[i]){
				case 0:printf("f"); break;
				case 'X':printf("u"); break;
				default: printf("m"); break;
				}
			}
			printf("\n");
		}
		if ((str = strstr(com, "exit")) != NULL){
			break;
		}
	}
	return 0;
}
