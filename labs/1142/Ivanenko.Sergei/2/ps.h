#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>
#include <dlfcn.h>
#include <string>
#include <unistd.h>
using namespace std;
extern "C" 
{
	int Ps(string *finalyPs);
}
