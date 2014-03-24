#include "ps.h"

void handler_function (int parameter)
{
  printf("Ctrl-C pressed\n");
}
int main ()
{
string inp;
string out_ps;
cout <<"use 'help' for further information about com's "<<endl;
signal(SIGINT,handler_function);
while(1)
{
cout << ">";
getline(cin,inp);

switch(inp[0])
{
        case 'h':
	  {
	    if (inp.compare(0,4,"help")==0)
		     {
		       cout <<"ls - list of files in current folder" << endl;
		       cout<<"ps - list of running processes"<<endl;
		       cout<<"/<path> to run an app"<<endl;
		       cout<<"pwd - full path to current folder"<<endl;
		       cout<<"kill <PID> <-SIGNAL> - send a SIGNAL to process with this PID "<<endl;
		       cout<<"exit - stop this app"<<"help - list of com's "<<endl;
		       break;
		     }
	  }
	case '/': 
		{
		  system(inp.c_str());
		  cout << "\n";
		  break;
		}
    case 'l':     
      {  
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
      }
    case 'p':
      {
		if(inp.compare(0,3,"pwd")==0)
		{
		  printf("Current dirrectory: %s \n\r",get_current_dir_name());
			cout << "\n";
			break;
		}
		if(inp.compare(0,2,"ps")==0)
		  {
		    if (MyPs(&out_ps)!=0)
		      {
			printf("Error while opening proc/ !  \n");
			break;
		      }
		    else
		      {
			cout<<out_ps;
			break;
		      }
		  }
      }
    case 'k':
      {       
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
      }
    case 'e':
      {       
		if(inp.compare(0,4,"exit")==0)
		{
			printf("Exit program.");
			cout << "\n";
			exit(0);
		}
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



