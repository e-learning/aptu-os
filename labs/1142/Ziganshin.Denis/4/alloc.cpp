#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define SIZE 30

int getSt(char *st){
	char c;
	int i=0;
	fflush(stdin);
	while((c=getchar()) != '\n'){
		st[i] = c;
		i++;	
	}
	st[i] = '\0';
	return i;
}

int getFreeSpace(char *m,int lon,int sz){
	int i,nlon,start,go;
	nlon = lon;
	for(i=0,go=0;i<sz;i++){
		if(m[i] == 0){
			if(!go){
				start = i;
				go = 1;
				nlon++;
			}
			nlon--;
		}else{
			nlon = lon;
			start = 0;
			go = 0;
		}
		
		if(!nlon)
			return start;
	}
	return -1;
}

int main(){
	char *m;
	char *p;
	int sz,i,pos,lon,mem,blocks,free,go,max;
	char arg;
	char s[SIZE];

	printf("memory (100 - 1000):");
	scanf("%d",&sz);
	m = (char*)malloc(sz);
	memset(m,0,sz);
	if(m==NULL)
		return 2;
	while(1){
		getSt(s);
		if((p=strstr(s,"ALLOC")) != NULL){
			p = &p[5];
			sscanf(p,"%d",&arg);
			arg = (arg<127)?arg:127;
			//printf("free\n");
			if((pos=getFreeSpace(m,arg,sz)) != -1){
				//printf("arg\n");
				m[pos] = arg|0x80;
				printf("+%d\n",pos);
				pos++;
				for(i=0;i<arg;i++)
					m[pos+i]='X';
			}else  printf("-can't alloc (%db)\n",arg);
		}
		if((p=strstr(s,"FREE")) != NULL){
			p = &p[4];
			sscanf(p,"%d",&arg);
			arg = arg>0?arg:0;
			arg = arg<sz?arg:sz-1;
			if((m[arg]&0x80) == 0x80){
				lon = m[arg] & 0x7f;
				lon++;
				for(i=arg;i<arg+lon;i++){
					m[i] = 0;
				}
			}else  printf("!block\n");
		}
		if((p=strstr(s,"INFO")) != NULL){
			max = 0;
			blocks = 0;
			free = 0;
			mem = 0;
			for(i=0;i<sz;i++){
				if(m[i] == 0){
					if(!go)
						go = 1;
					free++;
				}else{
					go = 0;
					if(free>max)
						max = free;
					free=0;
					if(m[i] == 'X')
						mem++;
					else
						blocks++;
				}
			}
			if(free>max)
				max = free;
			printf("%d %d %d\n",blocks,mem,max==0?0:max-1);
		}
		if((p=strstr(s,"MAP")) != NULL){
			for(i=0;i<sz;i++){
				switch(m[i]){
				case 0:printf("f"); break;
				case 'X':printf("u"); break;
				default: printf("m"); break;
				}
			}
			printf("\n");
		}
		if((p=strstr(s,"EXIT")) != NULL){
			break;
		}
	}
	return 0;
}
