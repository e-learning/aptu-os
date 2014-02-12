#include <stdio.h>
#include "task.h"

#define QUANT ((int) 10)

List g_ready;
List g_active;
List g_io;
List g_input; // incoming task queue

int g_q; // time into current quant [0...QUANT-1]
int g_currentTick = 0;

bool g_isRescheduled = false;

void input()
{
	static Task task1, task2;
	static IoBurst io1 = {2,1};
	static IoBurst io2 = {6,7};
	static IoBurst io3 = {5,2};

	init(&g_ready);


  init(&task1,"task1", 10, 1);
  init(&task2,"task2", 15, 3);
  
	addItem(&g_input,&task1);
	addIo(&task1,&io1);
	addIo(&task1,&io3);

	addItem(&g_input,&task2);
	addIo(&task2,&io2);

  printf("g_input: ");
	print(&g_input);
	printf("\n");
}

void pingTasks()
{
	g_isRescheduled = false;

  for(int i=0;i<g_active.size;)
	{
		Task *t = (Task*) getItem(&g_active,i);
		if(t==NULL)
		{
			printf("E:invalid program state in pingtask\n");
			return; //something bad
		}
		
		tick(t);
		if(isFinished(t))
		{
			pop(&g_active);
			g_isRescheduled = true;
	    printf("%u: %s finished \n",g_currentTick, t->name);
			continue;
		}
		if(isIoState(t))
		{
			pop(&g_active);
			addItem(&g_io,t);

			g_isRescheduled = true;

	    printf("%u: %s starts I/O Burst \n",g_currentTick, t->name);
			continue;
		}
		i++;
	}

	for(int i=0;i<g_io.size;)
	{
		Task *t = (Task*) getItem(&g_io,i);
		
		if(t==NULL)
		{
			printf("E:invalid program state in pingtask\n");
			return; //something bad
		}
		tick(t);
		if(!isIoState(t))
		{ // I/O burst is finished
			extract(&g_io,i);
			addItem(&g_ready,t);

			// a little bit magic, starting time machine,
			// move the task back in time
			t->currentTime--;

	    printf("%u: %s finished I/O \n",g_currentTick, t->name);
			g_isRescheduled = true;
		}
		i++;
	}
}

void checkInputQueue()
{

	for(int i=0;i<g_input.size;)
	{

    Task *t = (Task*)getItem(&g_input,i);

  	if( t!=NULL && t->startTime <= g_currentTick)
  	{
	    printf("%u: %s appeared in system\n",g_currentTick, t->name);
			extract(&g_input,i);
			addItem(&g_ready,t);
			continue;
		}
		i++;
	}
}

void schedule()
{
  g_q--;

	checkInputQueue();
	pingTasks();

	if(g_q>0 && !g_isRescheduled && g_active.size >0 )
		return; //just work

	g_q = QUANT;	
		

	Task *t = (Task*)pop(&g_active);

	if(t!=NULL)
	{ // if CPU was busy
	  addItem(&g_ready,t);
	  printf("%u:%s leaves CPU\n",g_currentTick,t->name);
  }

	t = (Task*)pop(&g_ready);
	
	if(t == NULL)
		return; // no ready tasks, it's time to think about stopping
	
	addItem(&g_active,t);
	printf("%u: %s accupes CPU\n",g_currentTick, t->name);

}

int main(int c, char **v)
{
  const int MAX_COUNTS = 500;

  input();

 	g_q = QUANT;
	while(g_currentTick <= MAX_COUNTS)
	{
    schedule();
    
		printf("CPU0[%3u] %s\n",g_currentTick, g_active.size>0 
			? ((Task*)getItem(&g_active,0))->name : "IDLE");

	  g_currentTick++;

		if(g_ready.size ==0 && g_active.size == 0 && g_io.size == 0 && g_input.size == 0)
			break; // enf of life
	}
	printf("DONE.\n");
}
