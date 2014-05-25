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
	char file_name[max_name_length]; //имя файла или папки
	time_t lm_time;			//время последней модификации
	bool is_file;			//файл или папка представлена по адресу
	int par;				//номер блока - родителя
							//в случае папки - родительский каталог
							//в случае файла - каталог хранения
	int nb;  				//следующий брат-каталог(файл)
	int pb;					//предыдущий брат-каталог(файл)
};

struct DDirect
{
	int deskr_num;	//номер индексного дескриптора
	int fch; 		//номер блока с именем первого дочернего каталога
	int ff;  		//номер блока с первым файлом этой папки
};

struct DFile
{
	int next_block_part;
	int size_of_block;
};

string GetBlockName(int i);

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id);

string path_to_fs;
string GetDirNameByDirS(DDirect str_dir);

const char* GetDirName(char* input,int lvl);
int GetBlockOfLastDir(string input);

bool PrintInform();
bool PrintDirInfo(int num,int lvl);

int main(int argc, char **argv)
{
	path_to_fs = argv[1];
	
	PrintInform();
	
	return 0;
}

const char* GetDirName(char* input,int lvl)
{
	int i=0;
	int count = 0;
	string ret("");
	while (input[i]!='\0')
	{
		char tmp = input[i];
		if (tmp == '/')
			count++;
		if (count==lvl)
			break;
		++i;
	}
	if (count==1)
	{
		return "/";
	}
	while ((input[count]!='\0')&&(input[count]!='/'))
	{
		ret=ret+input[count];
		++count;
	}
	return ret.c_str();
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

bool FileIsEmpty(int num)
{
	FILE* src_file;
	src_file = fopen(GetBlockName(num).c_str(),"rb");
	if (src_file!=NULL)
	{
		char c;
		while ((c = fgetc(src_file)) != EOF)
	    {
	        if ((char)c != 0xff)
	        {
	         fclose(src_file); return false;}
	    }
		fclose(src_file);			
	}
	else
	{
		perror("File can't be opened!");
	}
	
	return true;
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

bool PrintDirInfo(int num,int lvl)
{	
    DDirect rt_path;
    ifstream block_file(GetBlockName(num).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return 1;
    block_file.read((char*)&rt_path,sizeof(rt_path));
    block_file.close();
    DInode rt_inode;
    ReadStruct(&rt_inode,rt_path.deskr_num);
    for (int i=0;i<lvl;++i)
		printf("-");

    if (rt_path.fch!=NULL)
    {
		PrintDirInfo(rt_path.fch,lvl+1);
	}
	if (rt_path.ff!=NULL)
    {
		DInode tmp_file;
		ReadStruct(&tmp_file,rt_path.ff);
		for (int i=0;i<lvl;++i)
			printf("--");
		printf("%s\n\r",tmp_file.file_name);

		while (true)
		{
			for (int i=0;i<lvl;++i)
				printf("-");
			
			if (tmp_file.nb==NULL) break;
			int index = tmp_file.nb;
			ReadStruct(&tmp_file,tmp_file.nb);
		}
	}
	if (rt_inode.nb!=NULL)
    {
		DInode tmp;
		ReadStruct(&tmp,rt_inode.nb);	
		PrintDirInfo(tmp.first_block_addr,lvl);
	}
	return 0;
}

bool PrintInform()
{	
	Superblock sb;
    ifstream sblock_file(GetBlockName(0).c_str(),ios::binary|ios::in);
    if (!sblock_file.is_open())
		return 1;
    sblock_file.read((char*)&sb,sizeof(sb));
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
    iblock_file.read((char*)&fnode,sizeof(fnode));
    
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

int GetBlockOfLastDir(string input)
{
	string empty = "";
	if (GetDirName((char*)input.c_str(),1)=="/")
	{		
		return 0;
	}
	DInode tmpInode;
	int block_num = 1;
	ReadStruct(&tmpInode,1);	
	int i=1;
	DDirect tmpDir;
	block_num = tmpInode.first_block_addr;
	ReadStruct(&tmpDir,block_num);
	string tmpS(tmpInode.file_name);
	
	block_num = tmpDir.fch;

	ReadStruct(&tmpDir,block_num);
	ReadStruct(&tmpInode,tmpDir.deskr_num);
	
	while (true)
	{
		string tmp (GetDirName((char*)input.c_str(),i));
		if (tmp==empty)
			break;
		while ((tmpInode.file_name!=tmp.c_str())&&(tmpInode.nb!=NULL))
		{
			int bl_num;
			bl_num = tmpInode.nb;
			ReadStruct(&tmpDir,bl_num);
			ReadStruct(&tmpInode,tmpDir.deskr_num);
		}
		if ((tmpInode.nb==NULL)&&(tmpInode.file_name!=tmp.c_str()))
		{
			printf("..%s..\n\r",tmp.c_str());
			perror("catalog not found!");
			exit(1);
		}
		block_num = tmpInode.first_block_addr;
		ReadStruct(&tmpDir,tmpDir.fch);
		ReadStruct(&tmpInode,tmpDir.deskr_num);
		++i;		
	}
	return block_num;
}
