#include "ps.h"

extern "C" {
	void Ps(std::string * out_string){
		DIR *directory=opendir("/proc");
		if (!directory) return;

		struct dirent *f_directory;
		FILE *in_file;
		std::string f_name;
		char process_name[20];
		char tmp[40];
		while((f_directory=readdir(directory))!=NULL){
			if ((f_directory->d_name[0] <= '9')&&(f_directory->d_name[0] >= '0')){
				f_name = "/proc/" + std::string(f_directory->d_name) + "/comm";
				in_file = fopen(f_name.c_str(), "r");
				if (in_file == NULL) return;
				fscanf(in_file, "%s", &process_name);
				fclose(in_file);
				snprintf(tmp,40,"%8s  %s\n\r", f_directory->d_name, process_name);
				*out_string += tmp;
			}
		}
		return;
	}
}
