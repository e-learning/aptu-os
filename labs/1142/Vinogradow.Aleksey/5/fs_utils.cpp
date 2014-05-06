//Vin92.26.04.2014

#include "fs_utils.h"
#include <stdlib.h>

#define file 0x1
#define dir 0x2
#define del 0x4

char super_block[21];
char file_record[28];
std::string root;
std::string current_dir;
int current_dir_sector;



//////////////////////////////////////////////////////
//                                                  //
//               Работа с каталогами                //
//                                                  //
//////////////////////////////////////////////////////

std::string open_dir(char * buffer, int size_block, int data_size, std::string put){

	int dir_sector=1;

	std::string str1, str2;

	str2=pars_dir(put, 1);
	str1=pars_dir(put, 0);

	while (str2!=""){

//		cout<<"каталог "<<str1<<" потом ";

		if (load_data(buffer, size_block, dir_sector, data_size)) return "/err/";

		int tmp;

		dir_sector = search_file_in_dir(dir, buffer, data_size, str1, &tmp);

		if (dir_sector == -1){/* cout<<"файл не найден"<<endl;*/ return "/nof/";}

		str1=pars_dir(str2, 0);
		str2=pars_dir(str2, 1);

	}
//	cout<<"файл "<<str1<<endl;
//	cout<<"str2="<<str2<<" str1="<<str1<<endl;


	current_dir_sector = dir_sector;
	return str1;
}



int inf_file(char * s_type, char * buffer, int buffer_length, std::string name_f, int * file_length, char * t_year, char * t_mon, char * t_mday, char * t_hour, char * t_min, char * t_sec){

	int i=0;

	std::string nf;
	int start_sector=0;

	int j = name_f.length();

	for ( ;j < 15; j++){
		name_f += " ";
	}

	//while (name_f.length() < 15) name_f +=' ';
	
	while (buffer[i*28] != 0){
		if (i*28 > buffer_length) return -2;
		add_buffer(file_record, buffer, 0, i*28, 28);	
		read_file_record(s_type,  &start_sector, file_length, &nf, t_year, t_mon, t_mday, t_hour, t_min, t_sec);
		if ((nf == name_f)) {
			return start_sector;
		}
		i++;
	}
	return -1;//файл не найден
}



/*void print_dir(char * buffer, int buffer_length){

	int i=0;

	std::string nf;
	
	char type=0;
	
	while (buffer[i*28] != 0){
		if (i*28 > buffer_length) return -2;
		add_buffer(file_record, buffer, 0, i*28, 28);	
		read_file_record(&type,  &start_sector, file_length, &nf);
		if ((type == s_type) && (nf == name_f)) {
			return start_sector;
		}
		i++;
	}
	

}*/



int search_file_in_dir(int s_type, char * buffer, int buffer_length, std::string name_f, int * file_length, char mode){

	int i=0;

	std::string nf;
	char type=0;
	int start_sector=0;

	int j = name_f.length();

	for ( ;j < 15; j++){
		name_f += " ";
	}

	//while (name_f.length() < 15) name_f +=' ';
	
	while (buffer[i*28] != 0){
		if (i*28 > buffer_length) return -2;
		add_buffer(file_record, buffer, 0, i*28, 28);	
		read_file_record(&type,  &start_sector, file_length, &nf);
		if (mode){
			std::cout<<nf;
			if (type == dir) std::cout<<"dir";
			std::cout<<std::endl;
		}else{
			if ((type == s_type) && (nf == name_f)) {
				return start_sector;
			}
		}
		i++;
	}
	return -1;//файл не найден
}



int add_file(int type, char * buffer, int buffer_length, std::string name_f, int  file_length){

	int i=0;

	while ((buffer[i*28] != 0) && (buffer[i*28] != del)){
		if (i*28 > buffer_length - 28) return -2;
		i++;
		std::cout<<"buffer[i*28]="<<buffer[i*28]<<" i="<<i<<std::endl;
	}

	std::cout<<"ok - buffer[i*28]="<<buffer[i*28]<<" i="<<i<<std::endl;

	int file_start_sector = search_free_sector(); 

	if (file_start_sector == -1) return -1;

	edit_file_record(type, file_start_sector, file_length, name_f);

	add_buffer(buffer, file_record, i*28, 0, 28);

	return file_start_sector;
}



//////////////////////////////////////////////////////
//                                                  //
//    Инструменты для работы с файловой системой    //
//                                                  //
//////////////////////////////////////////////////////

int size_data(int size_block, int sector){

	int data_size =0;
	char sys_buff[3];

	while (sector!=0xFFFF){
		read_block(sys_buff, 3, sector);
		data_size+=size_block - 3;
		sector = (unsigned char)sys_buff[1];
		sector <<=8;
		sector |= (unsigned char)sys_buff[2];
	}
	return data_size;
}



int search_free_sector(){
	char sys_buff[3];
	int err=0;
	int sector=1;

	while(!err){
		err = read_block(sys_buff, 3, sector);
		if (err) return -1;

		if (sys_buff[0] == 0){
			sys_buff[0] = 0xFF;
			sys_buff[1] = 0xFF;
			sys_buff[2] = 0xFF;
			err = write_block(sys_buff, 3, sector);		
			if (err) return -1;
			return sector;
		}
		sector++;
	}	

	return -1;
}



int write_data(char * buffer2, int size_block, int sector, int data_size){
	char * buffer1 = new char [size_block];
	int offset = 0;
	int next_sector;

	std::cout<<std::endl<<"size_block:"<<size_block<<" data_size:"<<data_size<<std::endl;

	while (data_size>0){
		read_block(buffer1, size_block, sector);
		next_sector = (unsigned char)buffer1[1];
		next_sector <<= 8;
		next_sector |= (unsigned char)buffer1[2];
		buffer1[0] = 0xFF;
		if (data_size > size_block){
		if (next_sector == 0xFFFF){
			next_sector = search_free_sector();
			if (next_sector == 0xFFFF) return -1;
			buffer1[1]=(next_sector >> 8) & 0x00ff;
			buffer1[2]=next_sector & 0x00ff;
			std::cout<<"add sector:"<<next_sector<<std::endl;
		}	}
		if (data_size<(size_block - 3)){
			add_buffer(buffer1, buffer2, 3, offset, data_size + 3);//хз почему +3
			write_block(buffer1, data_size + 3, sector);
			std::cout<<"end "<<data_size<<std::endl;
		}else{
			add_buffer(buffer1, buffer2, 3, offset, size_block - 3);
			write_block(buffer1, size_block, sector);
		}
		sector = next_sector;
		offset += size_block - 3;
		data_size -= (size_block - 3);
		std::cout<<"datasize:"<<data_size<<" offset:"<<offset<<std::endl;
	}
	return 0;
}



int load_data(char * buffer2, int size_block, int sector, int data_size){

	char *buffer1 = new char [size_block];
	int offset = 0;

	while (sector!=0xFFFF){
		read_block(buffer1, size_block, sector);
		sector = (unsigned char)buffer1[1];
		sector <<=8;
		sector |= (unsigned char)buffer1[2];
		add_buffer(buffer2, buffer1, offset, 3, size_block - 3);
		offset += size_block - 3;
	}

	delete[] buffer1;
	return 0;
}



std::string pars_dir(std::string in_zn, int mode){
	std::string rez0, rez1;
	char flag=0, flag2=0;
	for (int i=0; i<strlen(in_zn.c_str()); i++){
		if (!flag2){
			 if (in_zn[i] != '/')
			 	flag2=1;
		}
		if (flag2){
			if (flag){
				rez1+=in_zn[i];
			}else{
				if (in_zn[i] == '/'){ 
					flag = 1;
				}else{
					rez0+=in_zn[i];
				}		
			}
		}
	}
	if (mode) return rez1;
	return rez0;
} 



void set_current_dir_sector(int sector){
	current_dir_sector = sector;
}



int get_current_dir_sector(){
	return current_dir_sector;
}



void read_file_record(char * type, int * start_sector, int * size_in_bytes, std::string * str, char * t_year, char * t_mon, char * t_mday, char * t_hour, char * t_min, char * t_sec){
	
	*type = file_record[0]; 
	*str = "";
	for(char i=0; i<15; i++){
		*str+=file_record[i+1];
	}

	*start_sector = 0;
	*start_sector = (unsigned char)file_record[16];
	*start_sector <<=8;
	*start_sector |= (unsigned char)file_record[17];

	*size_in_bytes = 0;
	*size_in_bytes = (unsigned char)file_record[18];
	*size_in_bytes <<=8;
	*size_in_bytes |= (unsigned char)file_record[19];
	*size_in_bytes <<=8;
	*size_in_bytes |= (unsigned char)file_record[20];
	*size_in_bytes <<=8;
	*size_in_bytes |= (unsigned char)file_record[21];


	*t_year = (unsigned char)file_record[22];
	*t_mon = (unsigned char)file_record[23];
	*t_mday = (unsigned char)file_record[24];
	*t_hour = (unsigned char)file_record[25];
	*t_min = (unsigned char)file_record[26];
	*t_sec = (unsigned char)file_record[27];
}



void read_file_record(char * type, int * start_sector, int * size_in_bytes, std::string * str){
	
	*type = file_record[0]; 
	*str = "";
	for(char i=0; i<15; i++){
		*str+=file_record[i+1];
	}

	*start_sector = 0;
	*start_sector = (unsigned char)file_record[16];
	*start_sector <<=8;
	*start_sector |= (unsigned char)file_record[17];

	*size_in_bytes = 0;
	*size_in_bytes = (unsigned char)file_record[18];
	*size_in_bytes <<=8;
	*size_in_bytes |= (unsigned char)file_record[19];
	*size_in_bytes <<=8;
	*size_in_bytes |= (unsigned char)file_record[20];
	*size_in_bytes <<=8;
	*size_in_bytes |= (unsigned char)file_record[21];
}



void edit_file_record(char type, int start_sector, int size_in_bytes, std::string str){
	file_record[0] = type;
	char i;
	for (i=0; (i<str.length()) & (i < 15); i++){
		file_record[i+1]=str[i];
	}
	for ( ; i<15; i++){
		file_record[i+1] = ' ';
	}

	file_record[16]=(start_sector >> 8) & 0x00ff;
	file_record[17]=start_sector & 0x00ff;

	file_record[18]=(size_in_bytes >> 24) & 0x00ff;
	file_record[19]=(size_in_bytes >> 16) & 0x00ff;
	file_record[20]=(size_in_bytes >> 8) & 0x00ff;
	file_record[21]=size_in_bytes & 0x00ff;

	time_t sec=time(NULL);
	tm* time_info = localtime(&sec);


	file_record[22]=time_info->tm_year & 0x00ff;
	file_record[23]=(time_info->tm_mon + 1) & 0x00ff;
	file_record[24]=time_info->tm_mday & 0x00ff;
	file_record[25]=time_info->tm_hour & 0x00ff;
	file_record[26]=time_info->tm_min & 0x00ff;
	file_record[27]=time_info->tm_sec & 0x00ff;
}



void add_buffer(char *buff, char *arr, int offset, int offset2, int  size){
	for(int i=0; i<size; i++){
		buff[i+offset]=arr[i+offset2];
	}
}



void clear_buffer(char * buff, int size){
	for (int i=0; i<size; i++) buff[i] = 0;
}



//////////////////////////////////////////////////////
//                                                  //
//          Отчистка файловой системы               //
//                                                  //
//////////////////////////////////////////////////////

int format(int count_block){

	char buff[3];

	buff[0]=0x00;

	buff[1]=0x00;
	buff[2]=0x00;

	for (int i=1; i < count_block; i++){
		write_block(buff, 3, i); 
	}
}



int create_root_dir(){

	char buff[3];

	buff[0]=0xFF;

	buff[1]=0xFF;
	buff[2]=0xFF;

	return write_block(buff, 3, 1); 
}



//////////////////////////////////////////////////////
//                                                  //
//          Инициализация файловой системы          //
//                                                  //
//////////////////////////////////////////////////////

int init_fs(int * size, int * count){

	int err = read_block(super_block, 21, 0);
	if (err) return err;

	std::cout<<super_block[0]<<
				super_block[1]<<
				super_block[2]<<
				super_block[3]<<
				super_block[4]<<std::endl;

	if (super_block[5]!=1) return -101;
	if ((super_block[20]!=0x55)) return -100;

	*size=(unsigned char)super_block[16];
	*size=*size<<8;
	*size+=(unsigned char)super_block[17];

//	std::cout<<std::endl<<" [18] "<<(int)super_block[18];
	*count=(unsigned char)super_block[18];
	*count=*count<<8;
//	std::cout<<" <<8 "<<*count;
	*count|=(unsigned char)super_block[19];
//	std::cout<<" [19] "<<(unsigned char)super_block[19]<<" rez "<<*count;
	return 0;
}



//////////////////////////////////////////////////////
//                                                  //
//              Чтение и запись блока               //
//                                                  //
//////////////////////////////////////////////////////

int read_block(char * buff, int size, int nom){
	FILE *block;

	char name[8];
	sprintf(name,"%d", nom);

	std::string full_name=root + '/' + name;
	block = fopen(full_name.c_str(), "rb");
	if (block == NULL) return -45;
	const char zero=0;
	for (int i=0; i<size; i++){
		fread(&buff[i], 1, 1, block);
	}
	fclose(block);
	return 0;
}



int write_block(char * buff,int size, int nom){
	FILE *block;

	char name[8];
	sprintf(name,"%d", nom);

	std::string full_name=root + '/' + name;
	block = fopen(full_name.c_str(), "rb+");
	if (block == NULL) return -40;
	const char zero=0;
	for (int i=0; i<size; i++){
		fwrite(&buff[i], 1, 1, block);
	}
	fclose(block);
	return 0;
}



/////////////////////////////////////////////

void set_root(std::string zn_root){
	root=zn_root;
}



/////////////////////////////////////////////
//                                         //
//     Создаём пустую файловую систему     //
//                                         //
/////////////////////////////////////////////

int init(int size_block, int count_block){
	int err;
	for (int i=0; i<count_block; i++){
		err=create_block(size_block, i);
		if (err) return err;
	}
	super_block[0]='V';	super_block[1]='i';	super_block[2]='n';	super_block[3]='9';	super_block[4]='2';

	super_block[5]=1;

	super_block[6]='D';	super_block[7]='i';	super_block[8]='s';	super_block[9]='k';	super_block[10]=' ';	super_block[11]=' ';	super_block[12]='0';	super_block[13]='0';	super_block[14]='1';	super_block[15]=' ';

	super_block[16]=(size_block >> 8) & 0x00ff;
	super_block[17]=size_block & 0x00ff;

	super_block[18]=(count_block >> 8) & 0x00ff;
	super_block[19]=count_block & 0x00ff;

	super_block[20]=0x55;

	err=write_block(super_block, 21, 0);
	if(err) return err;

	return 0;
}



int create_block(int size, int nom){

	FILE *block;

	char name[8];
	sprintf(name,"%d", nom);

	std::string full_name=root + '/' + name;
	block = fopen(full_name.c_str(), "wb");
	if (block == NULL) return -40;
	const char zero=0;
	for (int i=0; i<size; i++){
		fwrite(&zero, 1, 1, block);
	}
	fclose(block);
	return 0;
}


/////////////////////////////////////////////////////////////////////
         //////////////////////////////////////////////////
                   //////////////////////////////
                             //////////

