#include <sys/types.h> 
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <stdlib.h>

using namespace std;

void parent_process_func()
{
    FILE *file = NULL;
    int value;
    pid_t pid = getpid();
    cout << pid << " Родительский процесс, PID = " << pid << endl;
    while(1)
    {
        file = fopen("fork", "r+b");
        if(file == NULL)
        {
            cout << "Ошибка открытия файла." << endl;
            break;
        }
        fread(&value, sizeof(value), 1, file);
        
        cout << pid << " Родительский процесс: прочтено = " << value;
        
        if(value == -1)
        {
            cout << ", КОНЕЦ" <<endl;
            break;
        }
        
        if(value == 0)
        {
            cout << ", спит." << endl;
            sleep((rand() % 3) + 1);
        }
        else
        {            
            cout << ", обнуляет значение." << endl;
            value = 0;
            fseek(file, 0, SEEK_SET);
            fwrite(&value, sizeof(value), 1, file);
            fflush(file);
        }
        fclose(file);
    }
}

void child_process_func()
{
    FILE *file = NULL;
    int value;
    int counter = 10;
    pid_t pid = getpid();
    srand(time(NULL));
    cout << "Процесс потомок, PID = " << pid << endl;
    while(1)
    {
        file = fopen("fork", "r+b");
        if(file == NULL)
        {
            cout << pid << " Процесс потомок:Ошибка открытия файла." << endl;
            break;
        }
        fread(&value, sizeof(value), 1, file);
        
        cout << pid << " Процесс потомок: прочтено = " << value;
        if(value != 0)
        {
            cout << ", спит." << endl;
            sleep((rand() % 3) + 1);
        }
        else
        {
            if(counter != 0)
            {
                value = rand() % 1000 + 1;
            }
            else
            {
                value = -1;
            }
            cout << ", устанавливает значение = " << value << "." << endl;
            fseek(file, 0, SEEK_SET);
            fwrite(&value, sizeof(value), 1, file);
            fflush(file);
            counter--;
            
            if(value == -1)
            {
                cout << pid << " Процесс потомок:КОНЕЦ" <<endl;
                break;
            }
        }
        fclose(file);
    }
}

int main()
{
    FILE *file;
    int value = 0;
    file = fopen("fork", "wb");
    fwrite(&value, sizeof(value), 1, file);
    fclose(file);
    pid_t pid = fork(); //создаем процесс-потомок
    if(pid == -1)
    {
        if(errno == EAGAIN)
            cout << "fork не может выделить достаточно памяти для"
                    " копирования таблиц страниц родителя и для"
                    " выделения структуры описания процесса-потомка." << endl;
       
        if(errno == ENOMEM)
            cout << "fork не может выделить необходимые ресурсы ядра,"
                    " потому что памяти слишком мало." << endl;
    }
    else
    {
        if(pid == 0)
        {
            child_process_func();
        }
        else
        {
            parent_process_func();
        }
    }
    return 0;
}