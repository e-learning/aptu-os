


#include "inc.h"
#include "fs_utils.h"

using namespace std;

int main(int count, char **arg){

	if (count!=3) return -3;
	{
		string tmp="";
		tmp+=arg[1];
		set_root(tmp);
	}

	string dir_name=arg[2];

	int size_block=0, count_block=0;
	int err=0;
	set_current_dir_sector(1);

	err=init_fs(&size_block, &count_block);
	if (err) return err;


	cout<<"Размер блока:"<<size_block<<endl
		<<"Количество блоков:"<<count_block<<endl
		<<"host:"<<dir_name<<endl;

////////////////////////////////////////////////////////

	int data_size = size_data(size_block,1);//28 - число байт для добавления файловой записи

	cout<<"data_size"<<data_size<<endl;

	char *buffer = new char [data_size];
	if (buffer == NULL) return -1;

	clear_buffer(buffer, data_size);

	dir_name = open_dir(buffer, size_block, data_size, dir_name);

	if (dir_name == "/nof/"){ cout<<"файл не найден"<<endl; return -1;}

	if (dir_name == "/err/"){ cout<<"err"<<endl; return -2;}


	load_data(buffer, size_block, get_current_dir_sector(), data_size);

/// добавление директории

	int file_sector = add_file(dir ,buffer, data_size, dir_name, 0);

	cout<<"папка создана. Скектор:"<<file_sector<<endl;

	if (file_sector < 0){
		cout<<"некуда записать данные"<<endl;
		return -3;//некуда записать данные	
	}

	write_data(buffer, size_block, get_current_dir_sector(), data_size);


/////////////////////////////////////////////////////////

	delete[] buffer;
	cout<<"---"<<endl;

	return 0;
}
