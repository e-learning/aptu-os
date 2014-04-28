#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

struct Superblock
{
	int s_isize;				//адрес первого адреса данных
	int tfreeb;					//всего свободных блоков данных
	int tinode;					//всего дескрипторов
	//int freeb[];				//список свободных блоков данных
	//int inode[];				//список свободных дескрипторов
	//int first_free_deskript;	//номер первого свободного дескриптора 
	//int first_free_block;		//номер первого свободного блока
	time_t last_mod;			//время последнего изменения
	int block_size;				//размер одного блока
	int block_count;			//количество блоков
};

struct DInode
{
	int size;				//размер в байтах,если файл
	time_t cr_time;			//дата создания
	int first_block_addr;	//адрес первого блока
	char* file_name;		//имя файла или папки
	time_t lm_time;			//время последней модификации
	bool is_file;			//файл или папка представлена по адресу
	int par;				//номер блока - родителя
							//в случае папки - родительский каталог
							//в случае файла - каталог хранения
};

struct DDirect
{
	//char* dir_name;
	int deskr_num;	//номер индексного дескриптора
	int fch; 		//номер блока с именем первого дочернего каталога
	int nb;  		//следующий брат-каталог
	//int pb;
};

string GetBlockName(int i);
int GetFirstDiskript(string path);

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id);
template <class TStruct>
bool WriteStruct(TStruct* input,int block_id);

//bool ReadInode(DInode* input,int block_id);
//bool WriteInode(DInode* input,int block_id);

//bool ReadDir(DDirect* input,int block_id);
//bool WriteDir(DDirect* input,int block_id);

int chr_to_int(char* buf,int size);
string IntToStr(int x);

bool PrintInform();
bool PrintDirInfo(int num,int lvl);

string path_to_fs;
string GetDirNameByDirS(DDirect str_dir);

bool CreateEmptyDiskr(DInode* input_node);
int main(int argc, char **argv)
{
	const int max_files = 25;
	const int min_size = 1024;
	const int max_mem = 50*1024;
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
	root_inode.first_block_addr = max_files+1;
	root_inode.par = 0;
	root_inode.file_name = "/";
	time(&root_inode.cr_time);
	root_inode.lm_time = root_inode.cr_time;
	
	DDirect root_dir;
	root_dir.deskr_num = 1;
	root_dir.fch = -1;
	root_dir.nb = -1;
	//root_dir.pb = -1;
	
	Superblock new_sblock;
	new_sblock.block_count = block_count;
	new_sblock.block_size = block_size;
	new_sblock.s_isize = max_files+2;
	new_sblock.tfreeb = block_count - max_files - 2;
	new_sblock.tinode = max_files;
	//new_sblock.first_free_deskript = 2;
	//new_sblock.first_free_block = max_files+2;
	time(&new_sblock.last_mod);
	
	for (int i=2;i<new_sblock.tinode+2;++i)
	{
		DInode tmp;
		CreateEmptyDiskr(&tmp);
		WriteStruct(&tmp,i);
	}
	
	WriteStruct(&new_sblock,0);
	WriteStruct(&root_inode,1);
	WriteStruct(&root_dir,max_files+1);
	Superblock tmpbl;
	ReadStruct(&tmpbl,0);
	perror("main_end");
	PrintInform();
	
	//printf("name = %s\n\r",(tmpbl.root_name));
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
	//printf("%s\n\r",block_name.c_str());
	return block_name;
}

int GetFirstDiskript(string path)
{
	string supbl_path = path;
	Superblock new_sblock;
	//открываем файл для чтения
    ifstream in(path.c_str(),ios::binary|ios::in);
     if (!in.is_open())
		return -1;
    in.read((char*)&new_sblock,sizeof(new_sblock)); //Читаем структуру целиком сразу
    in.close();
    int end_of_diskr_block = new_sblock.s_isize;
    for (int i=2;i<end_of_diskr_block;++i)
    {
		DInode tmp;
		ReadStruct(&tmp,i);
		if (tmp.first_block_addr>0)
		 return i;		
	}
	//int tmp=new_sblock.first_free_deskript;
	return -1;
}

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id)
{
	//открываем файл для чтения
    ifstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return 1;
    block_file.read((char*)input,sizeof(*input)); //Читаем структуру целиком сразу
    block_file.close();
	return 0;
}

template <class TStruct>
bool WriteStruct(TStruct* input,int block_id)
{
	// открываем файл для записи
    ofstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::out);
     if (!block_file.is_open())
		return 1;
    block_file.write((char*)input,sizeof(*input)); //Записали всю структуру целиком
    block_file.close();
	return 0;
}

//bool ReadInode(DInode* input,int block_id)
//{
	////открываем файл для чтения
    //ifstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::in);
     //if (!block_file.is_open())
		//return 1;
    //block_file.read((char*)input,sizeof(*input)); //Читаем структуру целиком сразу
    //block_file.close();
	//return 0;
//}

//bool WriteInode(DInode* input,int block_id)
//{
	//// открываем файл для записи
    //ofstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::out);
     //if (!block_file.is_open())
		//return 1;
       //block_file.write((char*)input,sizeof(*input)); //Записали всю структуру целиком
    //block_file.close();
	//return 0;
//}

//bool ReadDir(DDirect* input,int block_id)
//{
	////открываем файл для чтения
    //ifstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::in);
     //if (!block_file.is_open())
		//return 1;
    //block_file.read((char*)input,sizeof(*input)); //Читаем структуру целиком сразу
    //block_file.close();
	//return 0;
//}

//bool WriteDir(DDirect* input,int block_id)
//{
	//// открываем файл для записи
    //ofstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::out);
    //if (!block_file.is_open())
		//return 1;
       //block_file.write((char*)input,sizeof(*input)); //Записали всю структуру целиком
    //block_file.close();
	//return 0;
//}

bool PrintDirInfo(int num,int lvl)
{	
    DDirect rt_path;
    //printf("block_addr = %i",num);
    ifstream block_file(GetBlockName(num).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return 1;
    block_file.read((char*)&rt_path,sizeof(rt_path)); //Читаем структуру целиком сразу
    block_file.close();
    
    for (int i=0;i<lvl;++i)
		printf("-");
    printf("%s\n\r",GetDirNameByDirS(rt_path).c_str());
    if (rt_path.fch>0)
    {
		PrintDirInfo(rt_path.fch,lvl+1);
	}
	if (rt_path.nb>0)
    {
		for (int i=0;i<lvl;++i)
			printf("-");
		PrintDirInfo(rt_path.nb,lvl+1);
	}
	return 0;
}

bool PrintInform()
{	
	//открываем файл для чтения
	Superblock sb;
    ifstream sblock_file(GetBlockName(0).c_str(),ios::binary|ios::in);
    if (!sblock_file.is_open())
		return 1;
    sblock_file.read((char*)&sb,sizeof(sb)); //Читаем структуру целиком сразу
    printf(" Всего блоков: %i\n\r",sb.block_count);
    printf(" Всего свободно блоков: %i\n\r",sb.tfreeb);
    printf(" Время последней модицикации: %s\n\r",ctime(&sb.last_mod));
    sblock_file.close();
    
    int num_of_rf;
    int num_of_next_inode;
    
    DInode fnode;
    ifstream iblock_file(GetBlockName(1).c_str(),ios::binary|ios::in);
    if (!iblock_file.is_open())
		return 1;
    iblock_file.read((char*)&fnode,sizeof(fnode)); //Читаем структуру целиком сразу    
    PrintDirInfo(fnode.first_block_addr,0);    
    iblock_file.close();
    return 0;
}

string GetDirNameByDirS(DDirect str_dir)
{
	int num_diskr = str_dir.deskr_num;
	DInode tmp_inode;
	ReadStruct(&tmp_inode,num_diskr);
	string tmp_str = tmp_inode.file_name;
	return tmp_str;	
}

bool CreateEmptyDiskr(DInode* input_node)
{
	(*input_node).size = 0;
	(*input_node).cr_time = 0;
	(*input_node).first_block_addr = -1;
	(*input_node).file_name = NULL;
	(*input_node).is_file = true;
	(*input_node).par = -1;	
	return 0;
}
