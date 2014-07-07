#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <queue>
#include <vector>

class Proc;
class Scheduler
{
	std::vector<Proc*> vproc;
	std::queue<Proc*> prior1Proc;
	std::queue<Proc*> prior0Proc;
	unsigned int procTimeslice;
	int procNum;
public:
	Scheduler(unsigned int q);

	void updateProcQueue(unsigned int cnt);
	void decreaseProc();

	bool hasProc();

	Proc* getNextProc();
	void addProc(Proc *p);
	void init();
	void distribProc(Proc* proc);
	~Scheduler();
};
#endif // SCHEDULER_H
