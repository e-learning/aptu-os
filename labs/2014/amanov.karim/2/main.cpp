#include <iostream>
#include <string>
#include <sstream>
#include "scheduler.h"
#include "proc.h"
#include "processor.h"


int main()
{

	unsigned int procTimeslice;
	std::string procName;
	std::string line;
	std::getline(std::cin, line);
	std::stringstream stream(line);
	stream >> procTimeslice;
	Scheduler scheduler(procTimeslice);
	Processor processor(procTimeslice, scheduler);

	while (std::getline(std::cin, line)) {
		Proc* proc = new Proc(processor);
		proc->init(line);
		scheduler.addProc(proc);
	}

	processor.run();

return 0;
}
