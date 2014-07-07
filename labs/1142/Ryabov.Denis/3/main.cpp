#include <iostream>
#include <pthread.h>
#include <ctime>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <cstdlib>
#include <unistd.h>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *potok(void *){
	int id = shmget(ftok("3laba", 0), 0, 0);
	if (id == -1) exit(1);
	int * adr =(int *) shmat(id, NULL, 0);
	if (adr==(int *)-1) exit(11);
	while (1) {
		if (*adr==0){
		*adr=rand()%100+1;
		pthread_mutex_lock(&mutex);
		cout<<"Записали:"<<*adr<<endl;
		pthread_mutex_unlock(&mutex);
		sleep(3);
		}
	}
}
int main(void){
	pthread_t pth;
	srand(time(NULL));
	int id = shmget(ftok("3laba", 0), 0, 0);
	if (id == -1) id = shmget(ftok("3laba", 0), 8, IPC_CREAT | IPC_EXCL | 0x100 | 0x80);
	if (id == -1) return 1;
	int * adr =(int *) shmat(id, NULL, 0);
	if (adr==(int *)-1) return 10;
	*adr=0;
	if (pthread_create(&pth, NULL, potok, NULL) != 0) {cout<<"Не удалось создать поток"<<endl;  return 1;}
	while (1){
		if (*adr!=0){
			pthread_mutex_lock(&mutex);
			cout<<"Найдено:"<<*adr<<endl;
			pthread_mutex_unlock(&mutex);
			*adr=0;
			sleep(1);
		}
	}
	pthread_join(pth, NULL);
	shmctl(id, IPC_RMID, NULL);
	return 0;
}
