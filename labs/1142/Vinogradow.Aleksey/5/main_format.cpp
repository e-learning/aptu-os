//init root - создаёт блоки в соответствии с config


#include "inc.h"
#include "fs_utils.h"

using namespace std;

int main(int count, char **arg){

	if (count==1){
		set_root("fs");
	}else{
		string tmp="";
		tmp+=arg[1];
		set_root(tmp);
	}

	int size_block=0, count_block=0;
	int err=0;

	err=init_fs(&size_block, &count_block);
	if (err) return err;

	err=format(count_block);

	err = create_root_dir();
	if (err) return err;

	cout<<"Размер блока:"<<size_block<<endl
		<<"Количество блоков:"<<count_block<<endl;

	cout<<"---"<<endl;

	return 0;
}
