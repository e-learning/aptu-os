//init root - создаёт блоки в соответствии с config


#include "inc.h"
#include "fs_utils.h"

using namespace std;

int main(int count, char **arg){

	if (count==1){
		system("mkdir fs");
		set_root("fs");
	}else{
		string tmp="";
		tmp+=arg[1];
		set_root(tmp);
	}
	//чтение файла настроек

	FILE *config;
	char p[8];
	int size_block, count_block;	
	int err;

	config = fopen("config", "r");
	if (config == NULL) return -10;
	if  (feof(config)) return -20;
	fscanf(config, "%s", &p);
	cout<<p<<endl;
	size_block=atoi(p);
	if (feof(config)) return -30;

	fscanf(config, "%s", &p);
	cout<<p<<endl<<"---"<<endl;
	count_block=atoi(p);
	fclose(config);

	err=init(size_block, count_block);
	if (err) return err ;

	return 0;
}
