#include "pslib.h"

extern "C" {int GetPs(Ps* MassPs)
{
	DIR *Dir = opendir("/proc");
	if(!Dir) return 0;
	int Count;
	struct dirent *entry;

	char buf[1000];

	while((entry=readdir(Dir))!=NULL)
	{

		string InfName="/proc/"+string(entry->d_name)+"/comm";
		FILE *Inf=fopen(InfName.c_str(),"r");
		if(Inf==NULL) return 0;

		fgets(buf,50,Inf);
		MassPs[Count].Name=buf;
		MassPs[Count].Id=entry->d_name;
		Count++;
		fclose(Inf);
	}
	closedir(Dir);
	return Count;
}
}
