#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <wait.h>

#define BUFFER_SIZE 100

static void command_ls() {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(".");
    if (dir == NULL) {
        perror("Cannot open directory");
        return;
    };
    while ((entry = readdir(dir)) != NULL) {
        printf("%s \n",entry->d_name);
    };
    printf("\n");
    closedir(dir);
}

static void command_pwd() {
    char* cur_dir = getcwd(NULL, 0);
    if (cur_dir == NULL) {
        perror("Cannot read directory");
    };
    printf("%s\n", cur_dir);
}
static void command_kill(pid_t pid, int no) {
    if (kill(pid, SIGTERM) != 0) {
        perror("Cannot to kill");
    }
}
static void command_ps() {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir("/proc")) == NULL) {
        perror("Unable to open /proc");
    };


    if (dir == NULL) {
        perror("Cannot open /proc");
        return;
    };
    printf("%.12s%25s\n", "PPID", "COMM");
    while((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && is_num(entry->d_name)) {
            char *proc_path = (char*) malloc(100);
            sprintf(proc_path, "%s/%s/comm", "/proc",entry->d_name);
            //printf("%s\n",proc_path);
            int comm;
            comm = open(proc_path, O_RDONLY);
            if (comm < 0) {
                perror("Unable to open comm");
                return;
            };
            char buffer[BUFFER_SIZE + 1];
            ssize_t read_bytes;

            while ((read_bytes = read(comm, buffer, BUFFER_SIZE)) > 0) {
                buffer[read_bytes] = 0;
                printf("%.12s%25s", entry->d_name,buffer);
            }
            close(comm);
        }
    }
    printf("\n");
}

void command_exe(char *argc) {
    pid_t pid = fork();
    //char* arg =strcpy(arg, argc);
    char* argv[2];
    argv[0] = argc;
    argv[1] = NULL;
    if (pid == 0) { //child
        errno = 0;
        int exec_return = execvp(argc,argv);
        if (exec_return == -1) {
            perror("Exec error");
        }
        //printf("%d\n",exec_return);
        _exit(0);
    }
    else if (pid < 0) {
        perror("fork error");
    }
    else {
        int status;
        waitpid(pid, &status, WUNTRACED| WCONTINUED);
        //printf("%s", (char*) WIFSIGNALED(status));
    }
}

void read_command(char *argc, char argv[][100]) {
    char c;
    int i = 0;
    argc[0] = '\0';
    while ((c = getchar()) != ' ' && c != '\t') {
        if (c == '\n') {
            return;
        }
        argc[i++] = c;
    }
    argc[i] = '\0';

    i = 0;
    for (i = 0; (c = getchar()) != '\n'; i++) {
        int j = 0;
        argv[i][j] = c;
        for(j = 1; (c = getchar()) != ' ' && c != '\n'; j++) {
            argv[i][j] = c;
        }
        argv[i][j] = '\0';
        if (c == '\n') {
            break;
        }
    }
    //while ((c = getchar()) != ' ' &&c != '\n'&& c != '\t') {
    //    argv[i++] = c;
    //}
    //argv[i] = '\0';
}
int str_to_int(char* arg) {
    char *ptr = NULL;
    const int ans = (int) strtol(arg, &ptr, 0);
    //printf("ans is%ld\n", ans);
    if (*ptr =='\0' && strlen(arg) > 0 && errno != ERANGE) {
        //printf("No error %ld\n",ans);
        return ans;
    }
    else {
        //printf("error\n");
        return -1;
    }
}

void signal_handler(int sig) {
    switch(sig) {
        case SIGINT:
            printf("%d", getpid());
            //kill(getpid(), SIGINT);
            //exit(0);
            break;
        default:
            break;
    }
}

int main (int c, char *v[]) {

    char argc[100];
    char argv[100][100];
    signal(SIGINT, signal_handler);
    printf(">");
    read_command(argc, argv);
    //printf("%d",SIGTERM);

    while (strcmp(argc,"exit") != 0) {
        if (strcmp(argc, "ps") == 0) {
            command_ps();
        }
        else if (strcmp(argc,"ls") == 0) {
            command_ls();
        }
        else if (strcmp(argc, "pwd") == 0) {
            command_pwd();
        }
        else if (strcmp(argc, "kill") == 0) {
            //printf("its kill\n");
            if (strlen(argv[0]) > 0) {
                if(strcmp(argv[0], "-s") == 0) {
                    if(strlen(argv[1]) > 0 && strlen(argv[2]) > 0) {
                        int pid = str_to_int(argv[2]);
                        int signal = str_to_int(argv[1]);
                        if (pid >= 0 && signal >= 0) {
                            command_kill(pid, signal);
                        }
                    }
                    else {
                        printf("Usage kill -s <signa> ppid");
                    }
                }
                else {
                    int pid = str_to_int(argv[0]);
                    command_kill(pid, SIGINT);
                }
            }
        }
        else if (argc[0] != '\0') {
            command_exe(argc);
        }
        memset(argc, '\0', 100);
        printf(">");
        read_command(argc, argv);
    }
    return 0;
}