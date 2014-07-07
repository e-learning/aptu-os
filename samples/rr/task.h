#ifndef TASK_H
#define TASK_H

#include "list.h"

#define MAX_NAME ((size_t)100)

struct IoBurst
{
	int start;
	int length;
};

struct Task 
{
  char name[MAX_NAME];
	int currentTime;
	int startTime;
 
  int length;
  List io;
};

void init(Task *task, const char *name, int length, int startTime);
void addIo(Task* task, IoBurst *io);
void print(Task* task);
void tick(Task* task);
bool isFinished(Task* task);
bool isIoState(Task *task);
bool isStarted(Task *task, int tick);


#endif // TASK_H
