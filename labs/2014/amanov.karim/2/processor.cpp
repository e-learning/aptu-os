#include "processor.h"
#include "scheduler.h"
#include "proc.h"
#include <iostream>

void Processor::execIO() {
	it = ioProcesses.begin();
	while (it != ioProcesses.end()) {
		if((*it)->execIO())
			++it;
	}
}


Processor::Processor(unsigned int t, Scheduler& s)
	: procTimeslice(t)
	, scheduler(s)
	, cycle(0)
	, curProc(0)
	, globalCnt(0)
{
}

void Processor::endProcIOHandler(Proc* proc) {
	it = ioProcesses.erase(it);
	scheduler.distribProc(proc);
}

void Processor::endProcHandler() {
	//scheduler.decreaseProc();
	cycle = procTimeslice - 1;
}

void Processor::startProcIoHandler(Proc* proc) {
	ioProcesses.push_back(proc);
	cycle = procTimeslice - 1;
}

void Processor::printState() {

    std::cout << globalCnt << " ";
    if (curProc == 0)
      std::cout << "IDLE";
    else
      std::cout << curProc->procName;

    std::cout << std::endl;

}

void Processor::run() {
	scheduler.init ();
	scheduler.updateProcQueue(globalCnt);
	curProc = scheduler.getNextProc();
	printState();
	while(scheduler.hasProc()) {
		execIO();
		if (curProc != 0) {
		  curProc->exec();
		 } else {
		    cycle = procTimeslice - 1;
		  }
		globalCnt++;
		cycle++;
		scheduler.updateProcQueue(globalCnt);
		if ( cycle == procTimeslice) {
		    if (curProc != 0) {
			if(!curProc->isEnd()) {
			    if (!curProc->isIO)
				scheduler.distribProc(curProc);
			} else {
			    scheduler.decreaseProc();
			  }
		      }
			Proc* p = curProc;
			curProc = scheduler.getNextProc();
			cycle = 0;
			if (curProc || p) {
				if (scheduler.hasProc())
					printState ();
			}
		}
	}
}
