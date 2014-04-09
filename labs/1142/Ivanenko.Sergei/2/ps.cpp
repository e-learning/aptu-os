#include "ps.h"

extern "C" 
{
int Ps(string *finalyPs)
{
	DIR *UseDirection = opendir("/proc");		
	struct dirent *start;
	string Name_File;
	string NameProcess;
	string intermRes;
	int time = 0;
 
	if (UseDirection)
        while((start = readdir(UseDirection)) != NULL)
        {
		if (time == 0)
		{
			*resultPs = "PID\tPROC_NAME\n";
			time = 1;
		}
                if ((start->d_name[0] <= '9')&&(start->d_name[0] >= '0'))
                {
                        Name_File = "/proc/" + string(start->d_name) + "/comm";
                        		                        
                        ifstream file(Name_File.c_str(),ifstream::in);

                        
			getline(file, NameProcess);
			intermRes = start->d_name;
                        		                        
                      	*finalyPs += intermRes + "\t" + NameProcess + "\n";
                }
        }
	else
	{
		cout << "Error: Can't open ""/proc"" directory. (ps)\n");
		*finalyPs = "n";
	}
}
}
