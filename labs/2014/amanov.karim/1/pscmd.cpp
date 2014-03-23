#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "pscmd.h"

int select(const struct dirent * d) 
{
	return 1;

}

void execPs()
{
	int res = 0;
	struct dirent ** entry;
	res = scandir("/proc", &entry, select, alphasort);
	if (res == -1) {
		perror("ps:");
		return;
	}
	std::cout << "PID" << "   " << "CMD" << std::endl;	
	for(int i = 2; i < res; i++) {
		if(checkIsNumber(entry[i]->d_name)) {
			std::cout << entry[i]->d_name << "   " << getProcName(entry[i]->d_name) << std::endl;	
		}
	}
	exit(EXIT_SUCCESS);
}

std::string getProcName(char* dir) 
{
	std::string path = "/proc/";
	path += dir;
	path += "/comm";
	std::ifstream comm(path, std::ifstream::in);
	std::string name;
	comm >> name;
	return name;
}

bool checkIsNumber(char* value)
{
	int length = strlen(value);

	for(int i = 0; i < length; i++) {
		if(!isdigit(value[i]))
			return false;
	}
	return true;
}
