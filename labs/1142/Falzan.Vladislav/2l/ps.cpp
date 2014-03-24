#include "ps.h"

extern "C" 
{
int Ps(string *resultPs)
{
	DIR *proc_dir = opendir("/proc");
			
	struct dirent *entry;
	string file_name;
 
	*resultPs = "PID\tPROC_NAME\n";
	if (proc_dir)
        while((entry = readdir(proc_dir)) != NULL)
        {
                if ((entry->d_name[0] <= '9')&&(entry->d_name[0] >= '0'))
                {
                        file_name = "/proc/" + string(entry->d_name) + "/comm";
                        		                        
                        ifstream file(file_name.c_str(),ifstream::in);

                        string proc_name;
			getline(file, proc_name);
			string temp = entry->d_name;
                        		                        
                      	*resultPs += temp + "\t" + proc_name + "\n";
                }
        }
	else
	{
		printf("Error: Can't open ""/proc"" directory. (ps)\n");
		return 1;
	}
	return 0;
}
}

