#ifndef GENERAL_H
#define GENERAL_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
using namespace std;

const int MAX_ARGS = 256;

enum PipeRedirect {PIPE, REDIRECT, NEITHER};

PipeRedirect parse_command(int, char**, char**, char**);

void pipe_cmd(char**, char**);

int read_args(char**);

void redirect_cmd(char**, char**);

void run_cmd(int, char**);

bool want_to_quit(string);

#endif // GENERAL_H
