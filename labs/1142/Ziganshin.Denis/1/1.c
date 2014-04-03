#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define PWD 1
#define PS 2
#define KILL 3
#define LS 4
#define EXIT 5
#define EXE 6

int getPWD(char *st){
FILE *f;
int lon,i;
char buf[200];
char *s;

	f = fopen("/proc/self/environ","r");
	if(f == NULL)
		return 1;
	while( (lon = fread(&buf,1,199,f)) > 0){
		for(i=0;i<lon;i++)
			if(buf[i] == 0)
				buf[i] = '\n';
	}
	if((s = strstr(buf,"PWD")) != NULL){
		s = &s[4];
		i=0;
		while(s[i] != '\n'){
			st[i] = s[i];
			//printf("%c",s[i]);
			i++;
		}
		st[i] = '\0';
	}
return 0;
}

int getls(char *st){
	DIR *dir;
    struct dirent *entry;

    dir = opendir(st);
    if (!dir) {
        return 1;
    }
    while ( (entry = readdir(dir)) != NULL) {
        printf("%s\n",entry->d_name);
    }

    closedir(dir);
    return 0;
}
int equ(char *des,char *sour,int firstSpace){
	int i=0;
	if(firstSpace && (des[i] != ' ')){
		return 0;
	}
	while(sour[i] != '\0'){
		if(des[i] != sour[i])
			return 0;
		i++;
	}
	if((des[i] != 0))
		return 0;
	return 1;
}

int parse(char *s){
	int i = 0;
	while(s[i] != '\0'){
		switch(s[i]){
			case '/': return EXE;
			case 'l':	if(equ(&s[i],"ls",i>0?1:0)){
							return LS;
						}
			case 'p': 	if(equ(&s[i],"pwd",i>0?1:0)){
							return PWD;
						}
						if(equ(&s[i],"ps",i>0?1:0)){
							return PS;
						}
			case 'k':	if(equ(&s[i],"kill",i>0?1:0)){
							return KILL;
						}
			case 'e':	if(equ(&s[i],"exit",i>0?1:0)){
							return EXIT;
						}
			default:break;
		}
		i++;
	}
	return 0;
}

int getSt(char *st){
	int i = 0;
	char c;
	c = getchar();
	while(c != '\n'){		
		st[i] = c;
		i++;
		c = getchar();
	}
	st[i] = '\0';
	return i;
}

int main(int argc, char *argv[]){
char st[30];
char s[60];
char ext = 0;
		printf("%s\n",argv[0]);
	while(!ext){
		printf(">");
		getSt(st);
		switch(parse(st)){
			case KILL:system(st); break;
			case PS: system("ps"); break;
			case PWD: 	getPWD(s); 
						printf("%s\n",s);
						memset(&s,0,sizeof(s)); 
						break;
			case LS: 	getPWD(s);
						getls(s);
						memset(&s,0,sizeof(s));
						break;
			case EXIT:ext = 1;break;
			case EXE:system(st); break;
			default:break;
		}
	}
return 0;
}
