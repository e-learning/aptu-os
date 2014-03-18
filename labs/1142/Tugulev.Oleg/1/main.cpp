#include <string>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <signal.h>
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
		  DIR *dir; 
		  struct dirent *ent;	 
		  dir=opendir("/proc");
		  if(!dir)
		    {
		      printf("Error opening proc/!");
		      closedir(dir);
		      break;
		    }
		  while((ent=readdir(dir)) !=0)
		    {
		      string tmp = ent->d_name;
		      if ((tmp[0] >= '0')&&(tmp[0] <= '9'))
           {
             string path_to_proc = "/proc/"+tmp+"/comm";
             std::ifstream total(path_to_proc.c_str(),std::ifstream::in);
             string outp;
             total >> outp;
             total.close();
             cout<<tmp<<"\t"<<outp<<"\n";
           }
		    }
			cout << "\n";
			break;
       
		}
    case 'k':       
		if(inp.compare(0,4,"kill")==0)
		{
		  size_t pid_pos_st=inp.find(" ");
			size_t pid_pos_en=inp.find(" ",pid_pos_st+1);
      string pid=inp.substr(pid_pos_st+1,pid_pos_en-pid_pos_st-1);
			size_t sig_pos=inp.find("-");
			string sig=inp.substr(sig_pos+1,inp.length()-sig_pos);
			//printf("%s\n",pid.c_str());
			//printf("%s\n",sig.c_str());
			if (!kill(atoi(sig.c_str()),atoi(pid.c_str())))
            {
            printf("Can't kill this!\n");
            break;
            }
		   else
            {
            printf("Killed successfully\n");
            break;
            }
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
