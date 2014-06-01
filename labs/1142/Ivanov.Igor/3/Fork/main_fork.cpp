#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

void First()
{
	FILE* File;
	cout << "Дочерний процесс начал работать" << endl;
	while(1)
	{
		if((File = fopen("File","r+b"))==NULL)
		{
			cout << "Ошибка!" << endl;
			break;
		}
		fseek(File,0,SEEK_SET);
		int number;
		while(fread(&number,sizeof(number),1,File)==0){};
		cout << "Чтение дочерним процессом " << number << endl;
		if(number == 5)
		{
			cout << "Выполнено!\n" << endl;
			break;
		}
		if(number != 0)
		{
			sleep(1);
			number=0;
			cout << "Запись дочерним процессом " << number << endl;
			fseek(File,0,SEEK_SET);
			fwrite(&number,sizeof(number),1,File);
			fflush(File);
		}
		fclose(File);
		sleep(3);
	}
}
void Second()
{
	FILE* File;
	cout << "Родительский процесс начал работать" << endl;
	srandom(time(NULL));
	while(1)
	{
		if((File = fopen("File","r+b")) == NULL)
		{
			cout << "Ошибка!" << endl;
			break;
		}
		fseek(File,0,SEEK_SET);
		int number;
		while(fread(&number,sizeof(number),1,File)==0){};
		cout << "Чтение родительским процессом " << number << endl;
		if(number != 0)
		{
			sleep(1);
		}
		else
		{
			number = random()%10+1;
			cout << "Запись родительским процессом " << number << endl;
			fseek(File,0,SEEK_SET);
			fwrite(&number,sizeof(number),1,File);
			fflush(File);
		}
		fclose(File);
		if(number == 5)
		{
			cout << "Выполнено!" << endl;
			break;
		}
		sleep(3);
	}
}
int main()
{
	FILE* File;
	int number=0;
	cout << "Создаем процессы" << endl;
	File = fopen("File","wb");
	fwrite(&number,sizeof(number),1,File);
	fclose(File);
	if(fork())
	{
		cout << "Я дочерний процесс, мой PID — " << getpid() << endl;
		First();
	}
	else
	{
		cout << "Я родительский процесс, мой PID — " << getpid() << endl;
		Second();
	}
	return 0;
}
