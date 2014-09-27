#ifndef _SHELL_H_
#define _SHELL_H_

#define MAX_COMMAND_LENGTH 256

void sigint_handler(int signum);
void setup();
void run();
void parse(char* command);
void execute(char* command);

#endif