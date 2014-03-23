using namespace std;
//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/utsname.h>
//#include <string>
//#include <sstream> 
//#include <signal.h>
//#include <dirent.h>
//#include <fstream>
#include "ps.h"

int MyPs(char **buf)
{
	DIR *proc_dir=opendir("/proc");
			
	struct dirent *f_dir;
	string file_name; 
	printf("%5s | %15s | %11s | %7s | %8s\n\r","PID","NAME","AVG_PER_CPU","THREADS","PRIORITY");
	if (proc_dir)
        while((f_dir=readdir(proc_dir))!=NULL) //считываем содержимое папки
        {
                if ((f_dir->d_name[0] <= '9')&&(f_dir->d_name[0] >= '0')) //для проверки того,что мы открываем
                {
                        file_name = "/proc/" + string(f_dir->d_name) + "/sched"; //файл процесса с текстовым именем файла
                        		                        
                        std::ifstream file(file_name.c_str());
                        std::string current_line,cpu_usage,thread_num,proc_name,priority;
					    std::getline(file, current_line);
					    float cpu_us;
					    
					    int space_pos = current_line.find(' ');
                        proc_name= current_line.substr(0,space_pos);
                        
                        space_pos=current_line.rfind(' ');		                        
                        thread_num = current_line.substr(space_pos,current_line.length()-1-space_pos);
                        					
                        while(getline(file, current_line))
                        { 
						    if (current_line.find("prio", 0) != string::npos)
						    {
								space_pos = current_line.rfind(' ');	                        
								priority = current_line.substr(space_pos,current_line.length()-space_pos);
						    }
						    if (current_line.find("avg_per_cpu ", 0) != string::npos)
						    {
								space_pos = current_line.rfind(' ');	                        
								cpu_usage = current_line.substr(space_pos,current_line.length()-space_pos);
								cpu_us = atof(cpu_usage.c_str());
						    }
						}

                        printf("%5s | %15s |  %8.2f   | %4s    | %5s\n\r", f_dir->d_name, proc_name.c_str(),cpu_us,thread_num.c_str(),priority.c_str());
                }
        }
	else
	{
		printf("Error opening /proc dirrectory../n/r");
		return 1; //can't open /proc
	}
	return 0;
}	
