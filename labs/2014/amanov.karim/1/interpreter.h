#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <sstream>

class Interpreter
{

void (*mysignal(int signo, void (*hndlr) (int))) (int); 
static void sigHndlr(int signo);

public:
	void run();
	void startInterpret();
};



#endif //INTERPRETER_H
