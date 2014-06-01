#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
 
FILE *f;
int k;
 
 void child(){
	int i;
	printf("child\n");
	while(1){
		f = fopen("file","r+b");
		if(f==NULL){
			break;
		}
		fseek(f,0,SEEK_SET);
		while(fread(&i,sizeof(i),1,f)==0);
		printf("read_child:%d\n",i);
		if(i == 6){
			printf("done\n");
			break;
		}
		if(i != 0){
			sleep(1);
			i=0;
			printf("write:%d\n",i);
			fseek(f,0,SEEK_SET);			
			fwrite(&i,sizeof(i),1,f);
			fflush(f);
		}
		fclose(f);	
		sleep(3);	
	}
 }
 void parent(){
	 int i;
	 printf("parent\n");
	 srandom(time(NULL));
	 while(1){	
		f = fopen("file","r+b");
		if(f==NULL){
			break;
		}	
		fseek(f,0,SEEK_SET);
		while(fread(&i,sizeof(i),1,f)==0);
		printf("read_parent:%d\n",i);
		if(i != 0){
			sleep(1);
		}else{
			i = random()%10+1;
			printf("parent:%d\n",i);
			fseek(f,0,SEEK_SET);
			fwrite(&i,sizeof(i),1,f);
			fflush(f);
		}
		fclose(f);
		if(i == 6){
			printf("done\n");
			break;	
		}	
		sleep(3);
	}
 }
 int main(){
	 int i=0;
	 printf("fork\n");
	 f = fopen("file","wb");
	 fwrite(&i,sizeof(i),1,f);
	 fclose(f);
	 if(fork()){
		 child();
	 }else{
		 parent();
	 }
	 return 0;
}
