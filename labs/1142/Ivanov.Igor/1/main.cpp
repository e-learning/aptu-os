#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <dirent.h>
using namespace std;

int main(void)
{
string command;
while (1)
{
cout<<">";
cin>>command;
if(command=="ls")
{
DIR *Dir = opendir(".");
if(!Dir) return 1;

struct dirent *entry;
while((entry=readdir(Dir))!=NULL)
printf("%s\n", entry->d_name);
closedir(Dir);
}
if(command=="exit") break;
}
}
