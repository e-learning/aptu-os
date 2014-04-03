#include "ps.h"

void command_ps()
{
    DIR *dir_p;
    struct dirent *dir_entry_p;
    char dir_name[256];
    FILE* stat_file;
    int pid;
    char comm[256];

    printf("%6s %51s\n", "PID", "Имя процесса");
    printf("***********************************************\n");
    dir_p = opendir("/proc/");// открываем каталог /proc/
    while((dir_entry_p = readdir(dir_p)) != NULL) {// Reading /proc/ entries
        if(strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) {// проверяем является ли имя каталога числом, т.е. PID процесса 
            strcpy(dir_name, "/proc/");
            strcat(dir_name, dir_entry_p->d_name);
            strcat(dir_name, "/stat");
            stat_file = fopen(dir_name, "rt");
            if(stat_file == NULL)
            {
                cout << "Ошибка при открытии файла статистики для процесса с PID = " << dir_entry_p->d_name << endl;
                return;
            }
            fscanf(stat_file, "%d %s", &pid, &comm);
            printf("%6d %40s\n", pid, comm);
            fclose(stat_file);
        }
    }
    closedir(dir_p);  
}