#include "scheduler.h"
#include "proc.h"
#include <iostream>

Scheduler::Scheduler(unsigned int q)
  : procTimeslice(q) {
}


Proc* Scheduler::getNextProc() {
	if (prior1Proc.empty()) {
		if(prior0Proc.empty())
			return 0;
		else {
			Proc* p = prior0Proc.front();
			prior0Proc.pop();
			return p;
		}
	} else {
		Proc* p = prior1Proc.front();
		prior1Proc.pop();
		return p;
	}
}

void Scheduler::addProc(Proc *p) {
	vproc.push_back(p);

}

void Scheduler::init()
{
    procNum = vproc.size ();
}

void Scheduler::updateProcQueue(unsigned int cnt) {
  for(unsigned int i = 0; i < vproc.size (); i++) {
      if (vproc[i]->procStart == cnt )
        distribProc (vproc[i]);
    }
}

void Scheduler::distribProc(Proc* proc) {
	int prior = proc->getPriority(procTimeslice);
	if (prior == 0)
		prior0Proc.push(proc);
	else
		prior1Proc.push(proc);
	}

Scheduler::~Scheduler() {
	for(unsigned int i = 0; i < vproc.size(); i++) {
		delete vproc[i];
	}
}

void Scheduler::decreaseProc() {
  procNum--;
}

bool Scheduler::hasProc() {
  return procNum != 0;
}
