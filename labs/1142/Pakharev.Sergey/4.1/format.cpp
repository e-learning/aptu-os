#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <ctime>

using namespace std;
static const int max_name_length = 10;
struct Superblock
{
	int s_isize;				//адрес первого адреса данных
	int tfreeb;					//всего свободных блоков данных
	int tinode;					//всего дескрипторов
	time_t last_mod;			//время последнего изменения
	int block_size;				//размер одного блока
	int block_count;			//количество блоков
};

struct DInode
{
	int size;				//размер в байтах,если файл
	time_t cr_time;			//дата создания
	int first_block_addr;	//адрес первого блока
	char file_name[max_name_length]; //имя файла или папки
	time_t lm_time;			//время последней модификации
	bool is_file;			//файл или папка представлена по адресу
	int par;				//номер блока - родителя
							//в случае папки - родительский каталог
							//в случае файла - каталог хранения
	int nb;  				//следующий брат-каталог(файл), ссылка на айнод
	int pb;					//предыдущий брат-каталог(файл)
};

struct DDirect
{
	int deskr_num;	//номер индексного дескриптора
	int fch; 		//номер блока с именем первого дочернего каталога
	int ff;			//номер блока с первым файлом этой папки
};

string GetBlockName(int i);
int GetFirstDiskript(string path);

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id);

template <class TStruct>
bool WriteStruct(TStruct* input,int block_id);

int chr_to_int(char* buf,int size);
string IntToStr(int x);

string path_to_fs;
string GetDirNameByDirS(DDirect str_dir);

bool CreateEmptyDiskr(DInode* input_node);
void ConvertName(string str_input,char arr_str[]);

int main(int argc, char **argv)
{
	const int max_files = 25;
	const int min_size = 1024;
	const int max_mem = 50000*1024;
	if (argc!=2)
	{
		printf("Critical error001: Count of parameters!=2");
		return 1;
	}
	string cfg_name("/config");
	path_to_fs = argv[1];
	char buf[10];
	int block_size;
	int block_count;
	FILE* configfile;
	configfile = fopen((path_to_fs+cfg_name).c_str(),"r");
	if (configfile!=NULL)
	{
		if (fgets (buf , 10 , configfile) != NULL)
		{
			block_size = chr_to_int(buf,10);
			if (block_size<min_size)
				perror("Size of blocks <1024");
		}
		if (fgets (buf , 10 , configfile) != NULL)
		{
			block_count = chr_to_int(buf,10);
			if (block_size*block_count>max_mem)
				perror("Memory from File system over 50M");
		}
		fclose(configfile);
	}
	else
	{
		 perror("Error opening file");
		 return 1;
	}
	
	DInode root_inode;
	root_inode.is_file = false;
	root_inode.first_block_addr = max_files+2;
	root_inode.par = 0;
	ConvertName("/",root_inode.file_name);	
	
	root_inode.nb = NULL;
	root_inode.pb = NULL;
	time(&root_inode.cr_time);
	root_inode.lm_time = root_inode.cr_time;
	
	DDirect root_dir;
	root_dir.deskr_num = 1;
	root_dir.fch = NULL;
	root_dir.ff = NULL;
	
	Superblock new_sblock;
	new_sblock.block_count = block_count;
	new_sblock.block_size = block_size;
	new_sblock.s_isize = max_files+2;
	new_sblock.tfreeb = block_count - max_files - 2;
	new_sblock.tinode = max_files;
	time(&new_sblock.last_mod);
	
	for (int i=2;i<new_sblock.s_isize;++i)
	{
		DInode tmp;
		CreateEmptyDiskr(&tmp);
		WriteStruct(&tmp,i);
	}
	
	WriteStruct(&new_sblock,0);
	WriteStruct(&root_inode,1);
	WriteStruct(&root_dir,max_files+2);
	DDirect tmpdir;
	ReadStruct(&tmpdir,max_files+2);
	return 0;
}


int chr_to_int(char* buf,int size)
{
	int num = 0;
	for (int i=0;i<size;++i)
	{
		if (buf[i]=='\n')
			return num;
		if ((buf[i]>='0')&&(buf[i]<='9'))
		{
			num=num*10+(buf[i]-'0');
		}
		else
			perror("Error of reading file");
	}
	return num;
}

string IntToStr(int x)
{
    char buf[20];
    sprintf(buf,"%d",x);
    return buf;
}

string GetBlockName(int i)
{
	string block_name = path_to_fs+"/"+IntToStr(i);
	return block_name;
}

int GetFirstDiskript(string path)
{
	string supbl_path = path;
	Superblock new_sblock;
    ifstream in(path.c_str(),ios::binary|ios::in);
     if (!in.is_open())
		return -1;
    in.read((char*)&new_sblock,sizeof(new_sblock));
    in.close();
    int end_of_diskr_block = new_sblock.s_isize;
    for (int i=2;i<end_of_diskr_block;++i)
    {
		DInode tmp;
		ReadStruct(&tmp,i);
		if (tmp.first_block_addr>0)
		 return i;		
	}
	return -1;
}

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id)
{
    ifstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return 1;
    block_file.read((char*)input,sizeof(*input));
    block_file.close();
	return 0;
}

template <class TStruct>
bool WriteStruct(TStruct* input,int block_id)
{
    ofstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::out);
     if (!block_file.is_open())
		return 1;
    block_file.write((char*)input,sizeof(*input));
    block_file.close();
	return 0;
}

string GetDirNameByDirS(DDirect str_dir)
{
	int num_diskr = str_dir.deskr_num;
	DInode tmp_inode;
	ReadStruct(&tmp_inode,num_diskr);
	string tmp_str(tmp_inode.file_name);
	
	printf("Name inputGet: ..%s..\n\r",tmp_inode.file_name);
	return tmp_str;	
}

bool CreateEmptyDiskr(DInode* input_node)
{
	(*input_node).size = 0;
	(*input_node).cr_time = 0;
	(*input_node).first_block_addr = NULL;
	
	(*input_node).is_file = true;
	(*input_node).par = NULL;	
	return 0;
}


void ConvertName(string str_input,char arr_str[])
{
	char converted_name[max_name_length];
	if (str_input.size()>max_name_length)
		{
			printf("Name is too long\n\r");
			exit(1);
		}
	int i=0;
	for (i=0;i<str_input.size();++i)
	{
		arr_str[i] = str_input[i];
	}
	for(i=str_input.size();i<max_name_length;++i)
	{
		arr_str[i]='\0';
	}
}

