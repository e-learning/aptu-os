#include "pslib.h"

extern "C" {Ps* GetPs()
{
	DIR *Dir = opendir("/proc");
	if(!Dir) return NULL;
	Ps* MassPs=new Ps[1000];
	int Count;
	struct dirent *entry;
	while((entry=readdir(Dir))!=NULL)
	{
		string InfName="/proc/"+string(entry->d_name)+"/comm";
		FILE *Inf=fopen(InfName.c_str(),"r");
		if(Inf==NULL) return NULL;

		fgets(MassPs[Count].Name,50,Inf);
		MassPs[Count].Id=entry->d_name;
		Count++;
		fclose(Inf);
	}
	closedir(Dir);
	return MassPs;
}
}
