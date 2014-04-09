#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>


static void *proc_A(void *input_args);
void proc_B();

static void *proc_A(void *input_args)
 {
	FILE *_file;
	int i;
	printf("Process A started\n\r");
	while(1)
	{
		if((_file = fopen("tmp","r+b"))==NULL)
		{
			printf("Error: can't open file..\n\r");
			break;
		}
		fseek(_file,0,SEEK_SET);
		while(fread(&i,sizeof(i),1,_file)==0)
		{ ; }
		printf("Reading from A: %d\n\r",i);
		if(i == 10)
		{
			printf("Process A done.\n\r");break;
		}
		if(i != 0)
		{
			sleep(1);
			i=0;
			printf("Writing from process A: %d\n\r",i);
			fseek(_file,0,SEEK_SET);			
			fwrite(&i,sizeof(i),1,_file);
			fflush(_file);
		}
		fclose(_file);	
		sleep(1);	
	}
	return NULL;
 }
 void proc_B()
 {
	 FILE *_file;
	 int i;
	 printf("Process B started\n\r");
	 srandom(time(NULL));
	 while(1)
	 {	
		if((_file = fopen("tmp","r+b"))==NULL)
		{
			printf("Error: can't open file..\n\r");
			break;
		}	
		fseek(_file,0,SEEK_SET);
		while(fread(&i,sizeof(i),1,_file)==0)
		{ ; }
		printf("Reading from process B: %d\n\r",i);
		if(i != 0)
		{
			sleep(1);
		}
		else
		{
			i = random()%15+1;
			printf("Process B: %d\n\r",i);
			fseek(_file,0,SEEK_SET);
			fwrite(&i,sizeof(i),1,_file);
			fflush(_file);
		}
		fclose(_file);
		if(i == 10)
		{
			printf("Process B done.\n\r");break;	
		}	
		sleep(1);
	}
 }
 int main()
 {
	 FILE *_file;
	 pthread_t my_thread;
	 int i=0;
	 printf("3 Lab. Operation systems. Fork.\n\r");
	 _file = fopen("tmp","wb");
	 fwrite(&i,sizeof(i),1,_file);
	 fclose(_file);
	 if(pthread_create(&my_thread, NULL ,proc_A,NULL) != 0)
	 {
		 return EXIT_FAILURE;
	 }	 
	 proc_B();
	 pthread_join(my_thread, NULL);
	 return 0;
}
