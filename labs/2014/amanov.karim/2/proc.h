#ifndef PROC_H
#define PROC_H
#include <list>
#include <string>
class Processor;
struct Proc {
	unsigned int workCycles;
	Processor& processor;
	std::string procName;
	unsigned int procStart;
	unsigned int procDuration;
	std::list<std::pair<unsigned int, unsigned int> > ioOperations;
	unsigned int currentIO;
	Proc(Processor& p);
	void exec();
	bool execIO();
	bool isIO;
	int getPriority(unsigned int q);
	int checkIO();
	bool isEnd();
	void init (std::string& str);
};
#endif // PROC_H
