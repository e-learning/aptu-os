#include "ps.h"

extern "C" {int fPS(std::string * zn0){
	DIR *d=opendir("/proc");
	if (!d) return 1;

	struct dirent *f_dir;
	FILE *in_f;
	std::string f_name;
	char p_name[20];
	char s_tmp[40];
//	printf("  PID  NAME\n\r");
	*zn0="  PID  NAME\n\r";
	while((f_dir=readdir(d))!=NULL){
		if ((f_dir->d_name[0] <= '9')&&(f_dir->d_name[0] >= '0')){
			f_name = "/proc/" + std::string(f_dir->d_name) + "/comm";
			in_f = fopen(f_name.c_str(), "r");
			if (in_f == NULL) return 2;
			fscanf(in_f, "%s", &p_name);
			//printf("%5s  %s\n\r", f_dir->d_name, p_name);
			snprintf(s_tmp,40,"%5s  %s\n\r", f_dir->d_name, p_name);
			*zn0 += s_tmp;
			fclose(in_f);
		}
	}
	return 0;
}
}
