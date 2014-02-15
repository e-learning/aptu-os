#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

#include <iostream>

void Run(std::string command);
void Pwd();
void Ps();
void Ls();
void Kill(std::string pid, std::string sig);

#endif // SHELL_COMMANDS_H
