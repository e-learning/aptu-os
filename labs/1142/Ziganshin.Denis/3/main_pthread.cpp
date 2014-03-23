#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>



static void *child(void *args){
	int i;
	FILE *f;
	printf("child\n");
	while(1){
		f = fopen("file","r+b");
		if(f==NULL){
			printf("can't open\n");
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
			fseek(f,0,SEEK_SET);			
			fwrite(&i,sizeof(i),1,f);
			fflush(f);
			printf("write:%d\n",i);
		}
		fclose(f);
		sleep(3);	
	}
	return NULL;
 }
 void parent(){
	 FILE *f;
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
	 FILE *f;
	 int i;
	 pthread_t thread;
	 f = fopen("file","wb");
	 fwrite(&i,sizeof(i),1,f);
	 fclose(f);
	 if(pthread_create(&thread, NULL ,child,NULL) != 0){
		 return EXIT_FAILURE;
	 }	 
	 parent();
	 pthread_join(thread, NULL);
	 return 0;
 }
