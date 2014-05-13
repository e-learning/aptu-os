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
int GetFirstDiskript();

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id);

template <class TStruct>
bool WriteStruct(TStruct* input,int block_id);

string path_to_fs;
string GetDirNameByDirS(DDirect str_dir);

const char* GetDirName(char* input,int lvl);

bool FileIsEmpty(int num);
int GetFirstDataBlock(int cur);

string GetFileName(string input);
int GetBlockOfLastDir(string input);

void ConvertName(string str_arr_input,char* str);

int GetBlockOfLastBr(int cur_bl);

int main(int argc, char **argv)
{
	if (argc!=3)
	{
		printf("Critical error001: Count of parameters!=3\n\r");
		return 1;
	}
	
	path_to_fs = argv[1];
	Superblock spblock;
	if (ReadStruct(&spblock,0))
	{
		perror("ooops");
	}
	
	int first_free_diskript = GetFirstDiskript();
	int first_free_block = GetFirstDataBlock(-1);
	
	int tmp_block = GetBlockOfLastDir(argv[2]); 
	tmp_block = (tmp_block==0)?spblock.s_isize:tmp_block;
	DDirect src_dir;
	ReadStruct(&src_dir,tmp_block);	
	DInode file_inode;
	ReadStruct(&file_inode,src_dir.deskr_num);
	
	DDirect new_direct;
	DInode new_inode;
	
	if (src_dir.fch!=NULL)
	{
		DDirect br_dir;
		ReadStruct(&br_dir,src_dir.fch);
		
		DInode brother;
		int adr_lb=GetBlockOfLastBr(br_dir.deskr_num);
		ReadStruct(&brother,adr_lb);
		brother.nb = first_free_diskript;
		WriteStruct(&brother,adr_lb);
		new_inode.pb = adr_lb;
	}
	else
	{
		new_inode.pb=NULL;
		src_dir.fch = first_free_block;
		WriteStruct(&src_dir,tmp_block);
	}	
	
	new_inode.is_file = false;
	new_inode.size = 0;
	new_inode.first_block_addr = first_free_block;
	new_inode.par = src_dir.deskr_num;
	string fn(GetFileName(argv[2]));
	ConvertName(fn,new_inode.file_name);	
	
	new_inode.nb = NULL;
	time(&new_inode.cr_time);
	new_inode.lm_time = new_inode.cr_time;
	
	new_direct.deskr_num = first_free_diskript;
	new_direct.fch = NULL;
	new_direct.ff = NULL;
	
	spblock.tfreeb = spblock.tfreeb - 1;
	spblock.tinode = spblock.tinode - 1;
	time(&spblock.last_mod);
	WriteStruct(&spblock,0);
	WriteStruct(&new_inode,first_free_diskript);
	WriteStruct(&new_direct,first_free_block);
	return 0;
}

string GetFileName(string input)
{
	int i=0;
	int last_index=0;
	while (input[i]!='\0')
	{
		char tmp = input[i];
		if (tmp == '/')
			last_index=i;
		++i;
	}
	string ret_name("");
	i=last_index+1;
	while (input[i]!='\0')
	{
		ret_name = ret_name + input[i];
		++i;
	}
	
	return ret_name;
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

int GetFirstDiskript()
{
	string supbl_path = GetBlockName(0);
	Superblock new_sblock;
    ifstream in(supbl_path.c_str(),ios::binary|ios::in);
     if (!in.is_open())
		return -1;
    in.read((char*)&new_sblock,sizeof(new_sblock));
    in.close();
    int end_of_diskr_block = new_sblock.s_isize;
    for (int i=2;i<end_of_diskr_block;++i)
    {
		DInode tmp;
		ReadStruct(&tmp,i);
		if (tmp.first_block_addr==NULL)
		 return i;		
	}
	return -1;
}

int GetFirstDataBlock(int cur)
{
	string supbl_path = GetBlockName(0);
	Superblock new_sblock;
    ifstream in(supbl_path.c_str(),ios::binary|ios::in);
     if (!in.is_open())
		return -1;
    in.read((char*)&new_sblock,sizeof(new_sblock));
    in.close();
    
    int end_of_data_field = new_sblock.block_count;
    
    for (int i=new_sblock.s_isize;i<end_of_data_field;++i)
    {
		if (FileIsEmpty(i)&&(i!=cur))
			return i;
	}
	return -1;
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

void ConvertName(string str_input,char* arr_str)
{
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

int GetBlockOfLastBr(int cur_bl)
{
	DInode cur;
	ReadStruct(&cur,cur_bl);
	int ret_val = cur_bl;
	while(cur.nb!=NULL)
	{
		if ((ret_val = cur.nb)==NULL) break;
		ReadStruct(&cur,ret_val);
		getchar();
	}
	return ret_val;
}
