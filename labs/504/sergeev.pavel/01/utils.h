#ifndef _UTILS_H_
#define _UTILS_H_

#define MAX_UTIL_NAME_LENGTH 8
#define UTILS_COUNT 5

typedef void (*util_fun_ptr)(char*);

struct util
{
	char name[MAX_UTIL_NAME_LENGTH];
	util_fun_ptr fun;
};

extern struct util utils[UTILS_COUNT];

void init_utils();
void ls_util(char* command);
void pwd_util(char* command);
void ps_util(char* command);
void kill_util(char* command);
void exit_util(char* command);

#endif