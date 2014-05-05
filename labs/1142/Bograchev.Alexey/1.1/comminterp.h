#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fstream>


std::string s_comm_args(std::string in_zn, int mode);
int fLS(void);
int fKILL(std::string);
int fPS(void);