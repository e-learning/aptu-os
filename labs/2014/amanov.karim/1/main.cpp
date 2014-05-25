#include "interpreter.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[], char* envp[])
{
	
	Interpreter interpreter;
	interpreter.run();

	return 0;
}
