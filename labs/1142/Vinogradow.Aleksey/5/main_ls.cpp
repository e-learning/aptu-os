


#include "inc.h"
#include "fs_utils.h"

using namespace std;

int main(int count, char **arg){

	if (count<2) return -4;
	{
		string tmp="";
		tmp+=arg[1];
		set_root(tmp);
	}

	string fs_file="/";

	if (count == 3){
		fs_file=arg[2];
	}

	int size_block=0, count_block=0;
	int err=0;
	set_current_dir_sector(1);

	err=init_fs(&size_block, &count_block);
	if (err) return err;


	cout<<"Размер блока:"<<size_block<<endl
		<<"Количество блоков:"<<count_block<<endl
		<<"fs:"<<fs_file<<endl;

////////////////////////////////////////////////////////

	int data_size = size_data(size_block,1);

	cout<<"data_size"<<data_size<<endl;

	char *buffer = new char [data_size];
	if (buffer == NULL) return -1;

	clear_buffer(buffer, data_size);

	fs_file = open_dir(buffer, size_block, data_size, fs_file);

	if (fs_file == "/nof/"){ cout<<"файл не найден"<<endl; return -1;}

	if (fs_file == "/err/"){ cout<<"err"<<endl; return -2;}

	load_data(buffer, size_block, get_current_dir_sector(), data_size);

/// добавление файла

	char year, mon, day, hour, min, sec;

	int file_size;
	char type;
	int file_sector = inf_file(&type, buffer, data_size, fs_file, &file_size, &year, &mon, &day, &hour, &min, &sec);

	if (file_sector < 0){ 
		cout<<"root/"<<endl; 
		file_sector = 1;
	}else{

	cout<<"дата:20"<<(int)(unsigned char)year-100
		<<"."<<(int)(unsigned char)mon
		<<"."<<(int)(unsigned char)day<<endl
		<<"время:"<<(int)(unsigned char)hour
		<<":"<<(int)(unsigned char)min
		<<":"<<(int)(unsigned char)sec<<endl;
		
	cout<<"type:"<<(int)type<<endl;
	//выделяем память и загружаем содержимое
	}
		
	if (type == dir){
		cout<<"-----"<<endl;
		load_data(buffer, size_block, file_sector, data_size);
		search_file_in_dir(file, buffer, data_size, fs_file, &file_size, 1);
		cout<<"-----"<<endl;
	}
	

//	load_data(file_buffer, size_block, file_sector, size_data(size_block, file_sector));

/////////////////////////////////////////////////////////

	delete[] buffer;
	cout<<"---"<<endl;

	return 0;
}
