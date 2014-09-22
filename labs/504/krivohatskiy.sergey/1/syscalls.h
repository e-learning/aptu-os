#ifndef SYSCALLS_H
#define SYSCALLS_H

void my_kill(int pid, int sig);

void my_get_current_dir_name();

void my_ps();

void my_read_dir(char* dirname);

void my_execute(char** args, int args_count);

#endif // SYSCALLS_H
