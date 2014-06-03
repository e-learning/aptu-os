#include "ps.h"

extern "C" 
{
int MyPs(string *buf)
	{
		  DIR *dir; 
		  struct dirent *ent;
		  *buf="PID\tPROC_NAME\n";
		  dir=opendir("/proc");
		  if(!dir)
		    {
		      closedir(dir);
		      return 1;
		    }
		  while((ent=readdir(dir)) !=0)
		    {
		      string tmp = ent->d_name;
		      if ((tmp[0] >= '0')&&(tmp[0] <= '9'))
			{
			  string path_to_proc = "/proc/"+tmp+"/comm";
			  ifstream total(path_to_proc.c_str(),ifstream::in);
			  string outp;
			  total >> outp;
			  total.close();
			  *buf+=tmp+"\t"+outp+"\n";
			}
		    }
		  return 0;
	}
}
