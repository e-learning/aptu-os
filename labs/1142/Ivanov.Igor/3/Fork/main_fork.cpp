Игорь
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
FILE *File;

void First(){
cout « "Дочерний процесс начал работать" « endl;
while(1)
{
if(File = fopen("File","r+b"))
{
cout « "Ошибка!" « endl;
break;
}
fseek(File,0,SEEK_SET);
int number;
while(fread(&number,sizeof(number),1,File)==0);
cout « "Чтение дочерним процессом " « number « endl;
if(i == 5)
{
cout « "Выполнено!\n" « endl;
break;
}
if(i != 0){
sleep(1);
number=0;
cout « "Запись дочерним процессом " « number « endl;
fseek(File,0,SEEK_SET);
fwrite(&number,sizeof(i),1,File);
fflush(File);
}
fclose(File);
sleep(1);
}
}
void Second()
{
cout « "Родительский процесс начал работать" « endl;
srandom(time(NULL));
while(1){
File = fopen("File","r+b");
{
cout « "Ошибка!" « endl;
break;
}
fseek(File,0,SEEK_SET);
int number;
while(fread(&i,sizeof(i),1,f)==0);
cout « "Чтение родительским процессом " « number « endl;
if(i != 0)
{
sleep(1);
}
else
{
number = random()%10+1;
cout « "Запись родительским процессом " « number « endl;
fseek(f,0,SEEK_SET);
fwrite(&number,sizeof(number),1,File);
fflush(File);
}
fclose(File);
if(i == 5){
cout « "Выполнено!\n" « endl;
break;
}
sleep(1);
}
}
int main(){
int number=0;
cout « "Создаем процессы" « endl;
File = fopen("file","wb");
fwrite(&number,sizeof(number),1,File);
fclose(File);
if(fork()){
cout « "Я дочерний процесс, мой PID — " « getpid() « endl;
First();
}else{
cout « "Я родительский процесс, мой PID — " « getpid() « endl;
Second();
}
return 0;
}
