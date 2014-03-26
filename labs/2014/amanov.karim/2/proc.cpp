#include "proc.h"
#include "processor.h"
#include <sstream>
#include <iostream>

Proc::Proc(Processor& p)
  : workCycles(0)
  , processor(p)
  , currentIO(0)
  , isIO(false) {

}

void Proc::exec() {
	workCycles++;
	if (procDuration - workCycles == 0) {
		processor.endProcHandler();
	} else if (currentIO = checkIO()) {
		isIO = true;
		processor.startProcIoHandler(this);
	}
}

bool Proc::execIO() {
	workCycles++;
	currentIO--;
	if (currentIO == 0) {
		isIO = false;
		processor.endProcIOHandler(this);
		return false;
	}
	return true;
}

int Proc::getPriority(unsigned int q) {
	if (ioOperations.size() > 0) {
		if (ioOperations.front().first - workCycles < q)
			return 1;
	}
	if (procDuration - workCycles <= q)
		return 1;
	return 0;
}

int Proc::checkIO() {
	std::list<std::pair<unsigned int, unsigned int> >::iterator it = ioOperations.begin();
	while (it != ioOperations.end()) {
		if (it->first == workCycles) {
			unsigned int ioDuration = it->second;
			ioOperations.erase(it);
			return ioDuration;
		}
		++it;
	}
	return 0;
}

bool Proc::isEnd() {
	return workCycles == procDuration;
}

void Proc::init (std::string& str) {
	std::stringstream stream(str);
	stream >> procName >> procStart >> procDuration;
	int io[2];
	int i = 0;
	while (stream >> io[i]) {
		if (i == 1) {
			ioOperations.push_back(std::make_pair(io[0], io[1]));
			i = 0;
		} else {
			i++;
		}
	}

}
