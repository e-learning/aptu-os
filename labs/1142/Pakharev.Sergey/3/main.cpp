#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

FILE *_file;
void proc_A();
void proc_B();

void proc_A()
 {
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
 }
 void proc_B()
 {
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
	 pid_t pid;
	 int i=0;
	 printf("3 Lab. Operation systems. Fork.\n\r");
	 _file = fopen("tmp","wb");
	 fwrite(&i,sizeof(i),1,_file);
	 fclose(_file);
	 if(pid=fork())
	 {
		 printf("Process A has PID = %d\n\r",pid);
		 proc_A();		 
	 }
	 else
	 {
		 printf("Process B has PID = %d, it is child process\n\r",getpid());
		 proc_B();
	 }
	 return 0;
}
