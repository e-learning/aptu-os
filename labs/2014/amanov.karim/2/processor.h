#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <list>

class Proc;
class Scheduler;

class Processor
{

	unsigned int procTimeslice;
	Scheduler& scheduler;
	unsigned int cycle;
	Proc* curProc;
	std::list<Proc*> ioProcesses;
	std::list<Proc*>::iterator it;
	void execIO();
	void printState();

public:
	int globalCnt;
	Processor(unsigned int t, Scheduler& s);

	void endProcIOHandler(Proc* proc);

	void endProcHandler();

	void startProcIoHandler(Proc* proc);

	void run();

};
#endif // PROCESSOR_H
