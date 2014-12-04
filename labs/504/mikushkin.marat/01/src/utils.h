#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

const int MAX_ARGS = 256;

void handle (int parameter);

enum ProcessCommunicationType {
	PIPE, REDIRECT, ANOTHER
};

ProcessCommunicationType parse(int, char**, char**, char**);
void pipeCommand(char**, char**);
int readArguments(char**);
void redirectCommand(char**, char**);
void runCommand(int, char**);
