#include <string>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;


int main ()
{
string inp;
while(1)
{
cout << ">";
getline(cin,inp);

switch(inp[0])
{
	case '/': 
		{
		system(inp.c_str());
		cout << "\n";
		break;
		}
    case 'l':       
		if(inp.compare(0,2,"ls")==0)
		{
			DIR *dir;
			struct dirent *ent;
			dir = opendir(".");
				if(!dir)
				{
					printf("Error opening current dirrectory..");
					closedir(dir);
					break;
				}
				while ((ent=readdir(dir)) != 0) 
				{
					printf("%s\n", ent->d_name);
				}
 
			closedir(dir);
			
			cout << "\n";
			break;
        }
    case 'p':
		if(inp.compare(0,3,"pwd")==0)
		{
		  printf("Current dirrectory: %s \n\r",get_current_dir_name());
			cout << "\n";
			break;
        }
		if(inp.compare(0,2,"ps")==0)
		{
			printf("ps");
			cout << "\n";
			break;
		}
    case 'k':       
		if(inp.compare(0,4,"kill")==0)
		{
			printf("kill");
			cout << "\n";
			break;
        }
    case 'e':       
		if(inp.compare(0,4,"exit")==0)
		{
			printf("Exit program.");
			cout << "\n";
			exit(0);
		}
	default: 
		{
		printf("There's no such cmd :(");
		cout << "\n";
		break;
		}
}
}
}
