#include <stdio.h>
#include <string.h>
#include "task.h"

void print(Task* task)
{
	printf("{%s, len=%u startTime=%u currentTime=%u state: %s ios: ",
	   task->name,task->length, task->startTime, task->currentTime, isIoState(task)?"I/O":"CPU");

	for(int i=0;i<task->io.size;++i)
	{
		size_t ioStart = ((IoBurst*)getItem(&task->io,i))->start;
		size_t ioLen   = ((IoBurst*)getItem(&task->io,i))->length;
		printf("(%u,%u) ",ioStart,ioLen);
	}

	print(&(task->io));
	printf("\n");
}

void init(Task* task, const char* name, int length, int startTime)
{
	strncpy(task->name, name, MAX_NAME-1);
  task->name[MAX_NAME-1]=0;

  
	task->currentTime = 0;
	task->startTime = startTime;
	task->length = length;

  init(& (task->io)); 	
}


void addIo(Task* task, IoBurst *io)
{
  addItem(& (task->io), io);
	print(task);
}

void tick(Task* task)
{
  // user pipgs us if we are in some "runnable" state
	if(task->currentTime < task->length)
		task->currentTime++;
}

bool isFinished(Task* task)
{
	return task->currentTime>=task->length;
}

bool isIoState(Task *task)
{
// cannot call print here
	for(int i=0; i<task->io.size;i++)
	{
		IoBurst *io = (IoBurst*)getItem(&task->io,i);
		if(task->currentTime >= io->start && task->currentTime-io->start <= io->length)
			return true;
	}
	return false;
}


bool isStarted(Task *task, int tick)
{
	return task->startTime>=tick;
}


