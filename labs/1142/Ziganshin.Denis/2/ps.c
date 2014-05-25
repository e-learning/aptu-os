#include "ps.h"
#define NAME 50

int getPS(char **buf){
	DIR *dir;
	int i,j,bufCount;
	char c;
	char ProcName[NAME];
	char Path[200];
	FILE *f;
    struct dirent **entry;
    
	entry = malloc(sizeof(struct dirent*)*400);
    dir = opendir("//proc");
    if (!dir) {
        return 1;
    }
    i = 0;
    bufCount = 0;
    while ( (entry[i] = readdir(dir)) != NULL) {
        if(isdigit(entry[i]->d_name[0])){
			strcpy(Path,"/proc/");
			strcat(Path,entry[i]->d_name);
			strcat(Path,"/");
			strcat(Path,"status");
			f = fopen(Path,"rt");
			c = getc(f);
			j = 0;
			while(c != '\n'){
				ProcName[j] = c;
				j++;
				c = getc(f);
			}
			ProcName[j] = '\0';
			strcpy(buf[bufCount],ProcName);
			//printf("%s\n",buf[bufCount]);
			bufCount++;
			fclose(f);
			memset(&Path,0,sizeof(Path));
			memset(&ProcName,0,sizeof(ProcName));
		}
        i++;
    }
	free(entry);
    closedir(dir);
    return bufCount;
}
