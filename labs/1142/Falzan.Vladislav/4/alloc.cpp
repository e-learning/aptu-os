#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define size_of_com 15

int get_string(char *st)
{
	char c;
	int i = 0;
	fflush(stdin);
	while((c = getchar()) != '\n')
	{
		st[i] = c;
		i++;	
	}
	st[i] = '\0';
	return i;
}

int get_free_space(char *mas,int len,int size){
	int i,tmp_len,start,go;
	tmp_len = len;
	for(i=0,go=0;i<size;i++){
		if(mas[i] == 0){
			if(!go){
				start = i;
				go = 1;
				tmp_len++;
			       }
			tmp_len--;
				}else
		{
			tmp_len = len;
			start = 0;
			go = 0;
		}
		
		if(!tmp_len)
			return start;
	}
	return -1;
}

int main(){
	char *mas;
	char *str;
	int size,i,pos,len,mem,blocks,free,go,max;
	char arg;
	char com[size_of_com];

	printf("Enter a value of the memory (100 - 10000) : ");
	scanf("%d",&size);
	mas = (char*)malloc(size);
	memset(mas,0,size);
	if(mas == NULL)
		return 2;
	while(1){
		get_string(com);
		if((str=strstr(com,"alloc")) != NULL){
			str = &str[5];
			sscanf(str,"%d",&arg);
			if((pos=get_free_space(mas,arg,size)) != -1){
				mas[pos] = arg|0x80;
				printf("+%d\n",pos);
				pos++;
				for(i=0;i<arg;i++)
					mas[pos+i]='X';
			}else  printf("- (%db)\n",arg);
		}
		if((str=strstr(com,"free")) != NULL){
			str = &str[4];
			sscanf(str,"%d",&arg);
			arg = arg>0?arg:0;
			arg = arg<size?arg:size-1;
			if((mas[arg]&0x80) == 0x80){
				len = mas[arg] & 0x7f;
				len++;
				for(i=arg;i<arg+len;i++){
					mas[i] = 0;
				}
			       printf("+\n");
			}else  printf("-\n");
		}
		if((str=strstr(com,"info")) != NULL){
			max = 0;
			blocks = 0;
			free = 0;
			mem = 0;
			for(i=0;i<size;i++){
				if(mas[i] == 0){
					if(!go)
						go = 1;
					free++;
				}else{
					go = 0;
					if(free>max)
						max = free;
					free=0;
					if(mas[i] == 'X')
						mem++;
					else
						blocks++;
				}
			}
			if(free>max)
				max = free;
			printf("%d %d %d\n",blocks,mem,max==0?0:max-1);
		}
		if((str=strstr(com,"map")) != NULL){
			for(i=0;i<size;i++){
				switch(mas[i]){
				case 0:printf("f"); break;
				case 'X':printf("u"); break;
				default: printf("m"); break;
				}
			}
			printf("\n");
		}
		if((str=strstr(com,"exit")) != NULL){
			break;
		}
	}
	return 0;
}
