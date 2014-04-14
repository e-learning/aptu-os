#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <stdlib.h>

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg)
{
    FILE *file = NULL;
    int value;
    
    int argv_vlue = *(int *)arg;
    
    if(argv_vlue == 1)
    {
        pthread_mutex_lock(&mutex);
        cout << "Поток #1 запущен" <<endl;
        pthread_mutex_unlock(&mutex);
        while(1)
        {
            file = fopen("thread", "r+b");
            if(file == NULL)
            {
                pthread_mutex_lock(&mutex);
                cout << "Поток #1:Ошибка открытия файла." << endl;
                pthread_mutex_unlock(&mutex);
                break;
            }
            fread(&value, sizeof(value), 1, file);

            pthread_mutex_lock(&mutex);
            cout << "Поток #1: прочтено = " << value;

            if(value == -1)
            {
                cout << ", КОНЕЦ" <<endl;
                pthread_mutex_unlock(&mutex);
                break;
            }

            if(value == 0)
            {
                cout << ", спит." << endl;
                pthread_mutex_unlock(&mutex);
                sleep((rand() % 3) + 1);
            }
            else
            {       
                cout << ", обнуляет значение." << endl;
                pthread_mutex_unlock(&mutex);
                value = 0;
                fseek(file, 0, SEEK_SET);
                fwrite(&value, sizeof(value), 1, file);
                fflush(file);
            }
            fclose(file);
        }
    }
    else
    {
        int counter = 10;
        srand(time(NULL));
        pthread_mutex_lock(&mutex);
        cout << "Поток #2 запущен" <<endl;
        pthread_mutex_unlock(&mutex);
        while(1)
        {
            file = fopen("thread", "r+b");
            if(file == NULL)
            {
                pthread_mutex_lock(&mutex);
                cout << "Поток #2:Ошибка открытия файла." << endl;
                pthread_mutex_unlock(&mutex);
                break;
            }
            fread(&value, sizeof(value), 1, file);

            pthread_mutex_lock(&mutex);
            cout << "Поток #2: прочтено = " << value;
            if(value != 0)
            {
                cout << ", спит." << endl;
                pthread_mutex_unlock(&mutex);
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
                pthread_mutex_unlock(&mutex);
                fseek(file, 0, SEEK_SET);
                fwrite(&value, sizeof(value), 1, file);
                fflush(file);
                counter--;

                if(value == -1)
                {
                    pthread_mutex_lock(&mutex);
                    cout << "Поток #2:КОНЕЦ" <<endl;
                    pthread_mutex_unlock(&mutex);
                    break;
                }
            }
            fclose(file);
        }
    }
}


int main()
{
    pthread_t thread1, thread2;
    int arg1, arg2;
    FILE *file;
    int value = 0;
    file = fopen("thread", "wb");
    fwrite(&value, sizeof(value), 1, file);
    fclose(file);
    
    arg1 = 1;
    arg2 = 2;
    
    pthread_create( &thread1, NULL, thread_func, (void*) (&arg1));
    pthread_create( &thread2, NULL, thread_func, (void*) (&arg2));

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    
    return 0;
}