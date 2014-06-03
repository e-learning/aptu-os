


#include "inc.h"
#include "fs_utils.h"

using namespace std;

int main(int count, char **arg){

	if (count!=4) return -4;
	{
		string tmp="";
		tmp+=arg[1];
		set_root(tmp);
	}

	string host_file=arg[2];
	string fs_file=arg[3];

	int size_block=0, count_block=0;
	int err=0;
	set_current_dir_sector(1);

	err=init_fs(&size_block, &count_block);
	if (err) return err;


	cout<<"Размер блока:"<<size_block<<endl
		<<"Количество блоков:"<<count_block<<endl
		<<"host:"<<host_file<<endl
		<<"fs:"<<fs_file<<endl;

	int data_size = size_data(size_block,1);//28 - число байт для добавления файловой записи

	cout<<"data_size"<<data_size<<endl;

	char *buffer = new char [data_size];
	if (buffer == NULL) return -1;

	clear_buffer(buffer, data_size);


	fs_file = open_dir(buffer, size_block, data_size, fs_file);

	if (fs_file == "/nof/"){ cout<<"файл не найден"<<endl; return -1;}

	if (fs_file == "/err/"){ cout<<"err"<<endl; return -2;}


////////////////////////////////////////////////////////


	load_data(buffer, size_block, get_current_dir_sector(), data_size);
//	load_data(buffer, size_block, 1, data_size);


/// добавление файла

	ifstream File;
	File.open(host_file.c_str(), ios::binary);

	//опеделение рамера файла
	File.seekg(0, ios::end);
	int file_size = File.tellg();
	File.seekg(0, ios::beg);

	//выделяем память и загружаем содержимое
	char * file_buffer = new char [file_size];
	if (file_buffer == NULL) {cout<<"Нет памяти!"<<endl; return -3;}
		
	File.read(file_buffer, file_size);

	File.close();

	cout<<"добавление файла"<<endl;

	int file_sector = add_file(file ,buffer, data_size, fs_file, file_size);

	cout<<"файл добавлен"<<endl;

	if (file_sector < 0){
		cout<<"некуда записать данные"<<endl;
		return -3;//некуда записать данные	
	}

	write_data(file_buffer, size_block, file_sector, file_size);


	write_data(buffer, size_block, get_current_dir_sector(), data_size);


/////////////////////////////////////////////////////////

	delete[] buffer;
	delete[] file_buffer;
	cout<<"---"<<endl;

	return 0;
}
