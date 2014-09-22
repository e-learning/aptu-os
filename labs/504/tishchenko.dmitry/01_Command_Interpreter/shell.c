#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

char* const* tokenize(char *command, int* count)
{
    char **result;
    char *token;
    char *command_copy;
    int index;
    command_copy = strdup(command);
    token = strtok(command_copy, " ");
    (*count) = 0;
    while(token!=NULL)
    {
        (*count)++;
        token = strtok(NULL, " ");
    }
    result = malloc((*count)*sizeof(char*));
    free(command_copy);
    command_copy = strdup(command);
    token = strtok(command_copy, " ");
    index = 0;
    while(token!=NULL)
    {
        result[index] = token;
        index++;
        token = strtok(NULL, " ");
    }
    return result;
};

char* const *copyArgs(int argc, char* const *argv)
{
    char **result;
    int i;
    result = malloc((argc+1)*sizeof(char*));
    for(i=0; i<argc; i++)
    {
        result[i]=argv[i];
    }
    result[argc]=NULL;
    return result;
}

void ls(int argc, char* const *argv)
{
    DIR *directory;
    struct dirent *entry;
    if(argc>2)
    {
        printf("ls: Usage: ls <directory>\n");
        return;
    }
    if(argc==1) directory = opendir(".");
    else directory = opendir(argv[1]);
    if(!directory)
    {
        perror("ls");
        return;
    }
    while((entry = readdir(directory))!=NULL)
    {
        printf("%s\n", entry->d_name);
    }
    closedir(directory);
}

void pwd(int argc, char* const *argv)
{
    char dirname[1024];
    if(argc!=1)
    {
        printf("pwd doesn't accept any arguments\n");
    }
    getcwd(dirname, 1024);
    printf("pwd: %s\n", dirname);
}

void ps(int argc, char* const *argv)
{
    DIR* proc;
    char cmdline[1024];
    struct dirent *entry;
    FILE *cmdFile;
    char *cmdlineContent;
    unsigned int cmdlineLength = 1024;
    proc = opendir("/proc");
    cmdlineContent = (char*)malloc(1024*sizeof(char));
    while((entry=readdir(proc))!=NULL)
    {
        if(strspn(entry->d_name, "0123456789")==strlen(entry->d_name))
        {
            sprintf(cmdline, "/proc/%s/cmdline", entry->d_name);

            cmdFile = fopen(cmdline, "r");
            getline(&cmdlineContent, &cmdlineLength, cmdFile);
            printf("%s %s\n", entry->d_name, cmdlineContent);

            fclose(cmdFile);
        }
    }
    free(cmdlineContent);
}

int myexit(int argc, char* const *argv)
{
    if(argc>1)
    {
        printf("exit doesn't accept any arguments\n");
        return 0;
    }
    return 1;
}
void mykill(int argc, char* const *argv)
{
    int signal, pid;
    char *endsym;
    if(argc!=3)
    {
        printf("kill: Usage: kill <signal> <pid>\n");
        return;
    }
    errno = 0;
    signal = (int)strtol(argv[1],&endsym, 10);
    if(errno==ERANGE)
    {
        perror("kill signal error");
        return;
    }
    pid = (int)strtol(argv[2],&endsym, 10);
    if(errno==ERANGE)
    {
        perror("kill pid error");
        return;
    }
    kill(pid, signal);
    switch(errno)
    {
        case EINVAL:
            fprintf(stderr,"kill: Invalid signal code.\n");
            break;
        case EPERM:
            fprintf(stderr,"kill: No permission.\n");
            break;
        case ESRCH:
            fprintf(stderr,"kill: Invalid pid.\n");
            break;
    }
}

void execute(int argc, char* const *argv)
{
    pid_t pid = fork();
    char* const *args;

	if (pid == 0)
	{
        args = copyArgs(argc, argv);
		printf("execcode:%d\n",execvp(args[0],args));
		free((void*)args);
    }
	else if (pid > 0)
	{
        int status;
        waitpid(pid, &status, 0);
    }
	else
		perror("cannot start child process");
}

int parseAndExecute(int argc, char* const *argv)
{
    if(strcmp(argv[0],"ls")==0)
    {
        ls(argc, argv);
        return 0;
    }
    if(strcmp(argv[0], "pwd")==0)
    {
        pwd(argc, argv);
        return 0;
    }
    if(strcmp(argv[0],"ps")==0)
    {
        ps(argc, argv);
        return 0;
    }
    if(strcmp(argv[0], "kill")==0)
    {
        mykill(argc, argv);
        return 0;
    }
    if(strcmp(argv[0], "exit")==0)
        return myexit(argc, argv);
    execute(argc, argv);
    return 0;
}

int main()
{
    char command[1024];
    int argc=0;
    char* const *argv;
    int exit=0;
    do
    {
        printf(">");
        fgets(command, 1024, stdin);
        size_t ln = strlen(command) - 1;
        if (command[ln] == '\n') command[ln] = '\0';
        if(strlen(command)!=0)
        {
            argv = tokenize(command, &argc);
//            printf("Args: %d\n", argc);
//            printf("Command name: %s\n", argv[0]);
            exit = parseAndExecute(argc, argv);
        }
    }
    while(exit==0);
    return 0;
}
