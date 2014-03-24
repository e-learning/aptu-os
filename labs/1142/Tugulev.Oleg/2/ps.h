#include <string>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <signal.h>

using namespace std;

extern "C"
{
	int MyPs(string *buf);
}
