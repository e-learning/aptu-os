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
	int ff;			//номер блока с айнодом первого файла этой папки
};

struct DFile
{
	int next_block_part;
	int size_of_block;
};

string GetBlockName(int i);
int GetFirstDiskript();

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id,int ololo);

template <class TStruct>
bool WriteStruct(TStruct* input,int block_id);

string IntToStr(int x);
string path_to_fs;

std::ifstream::pos_type filesize(const char* filename);
const char* GetDirName(char* input,int lvl);

bool FileIsEmpty(int num);
int GetFirstDataBlock(int cur);

string GetFileName(string input);

void ConvertName(string str_input,char arr_str[]);
int GetBlockOfLastBr(int cur_bl);

bool WriteData(DFile* input,int block_id,char data[]);
int GetDirInDir(string name,int num,int lvl);

int GetLastDir(string input);

int main(int argc, char **argv)
{
	if (argc!=4)
	{
		printf("Critical error001: Count of parameters!=4\n\r");
		return 1;
	}
	
	path_to_fs = argv[1];
	Superblock spblock;
	if (ReadStruct(&spblock,0,0))
	{
		perror("ooops");
	}
	
	FILE* src_file;
	src_file = fopen(argv[2],"rb");
	if (src_file==NULL)
	{
		printf("Critical error004: File not exist!\n\r");
		return 1;
	}
	
	
	int count_of_blocks = filesize(argv[2]);
	int nbl = count_of_blocks/(spblock.block_size-sizeof(int) - sizeof(int));
	int nbtl = count_of_blocks%(spblock.block_size-sizeof(int) - sizeof(int));
	if (nbtl!=0) nbl++;
	
	char main_buf[count_of_blocks];
	fread(main_buf,sizeof(char),count_of_blocks,src_file);
	if (nbl>spblock.tfreeb)
	{
		printf("error003: File is too big for File system.\n\r");
		return 1;
	}
	
	if (argv[3][0]!='/')
	{
		printf("error002: Destination adress isn't correct.\n\r");
		return 1;
	}
	
	DInode new_file_node;
	int first_free_diskript = GetFirstDiskript();
	int first_free_block = GetFirstDataBlock(-1);
	ReadStruct(&new_file_node,first_free_diskript,0);
	
	new_file_node.size = count_of_blocks;
	time(&new_file_node.cr_time);
	time(&new_file_node.lm_time);
	new_file_node.is_file = true;
	new_file_node.first_block_addr = first_free_block;
	new_file_node.nb = NULL;
	string fn(GetFileName(argv[2]));
	ConvertName(fn,new_file_node.file_name);

	int num_of_block = GetLastDir(argv[3]); 
	num_of_block = (num_of_block==-1)?1:num_of_block;
	
	DInode par_dir; 
	ReadStruct(&par_dir,num_of_block,0);
	
	DDirect par_dir_dir;
	ReadStruct(&par_dir_dir,par_dir.first_block_addr,0);
	new_file_node.par = par_dir.first_block_addr;

	int lf;
	if (par_dir_dir.ff==NULL)
	{
		lf=par_dir.first_block_addr;
		par_dir_dir.ff = first_free_diskript;
		WriteStruct(&par_dir_dir,par_dir.first_block_addr);
		new_file_node.pb = NULL;
	}
	else
	{
		lf = GetBlockOfLastBr(par_dir_dir.ff);
		ReadStruct(&par_dir,lf,0);
		par_dir.nb = first_free_diskript;
		WriteStruct(&par_dir,lf);
		new_file_node.pb = lf;
	}
	
	int max_block_size = spblock.block_size-sizeof(int) - sizeof(int);
	for (int i=0;i<nbl-1;++i)
	{		
		DFile new_file_block;
		new_file_block.size_of_block = max_block_size;
		char tmp_arr[max_block_size];	
		memcpy(&tmp_arr,(char*)main_buf+i*(max_block_size),max_block_size);
		int next_p= GetFirstDataBlock(first_free_block);
		new_file_block.next_block_part = (nbtl>0)?next_p:NULL;
		WriteData(&new_file_block,first_free_block,tmp_arr);
		first_free_block = next_p;	
	}
	if (nbtl>0)
	{
		DFile new_file_block;
		new_file_block.size_of_block = nbtl;
		char tmp_arr[max_block_size];		
		memcpy(&tmp_arr,(char*)main_buf+(nbl-1)*(max_block_size),max_block_size);
		new_file_block.next_block_part = NULL;
		WriteData(&new_file_block,first_free_block,tmp_arr);
	}
	
	spblock.tfreeb = spblock.tfreeb - nbl;
	spblock.tinode = spblock.tinode - 1;
	time(&spblock.last_mod);
	WriteStruct(&spblock,0);
	WriteStruct(&new_file_node,first_free_diskript);
	printf("deskr block of new file = %i\n\r",first_free_diskript);
	printf("name of new file = %s\n\r",new_file_node.file_name);
	printf("first data block = %i\n\r",new_file_node.first_block_addr);	
	return 0;
}

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::in | std::ifstream::binary);
    if (!in.is_open())
		return -1;
    in.seekg(0, std::ifstream::end);
    return in.tellg(); 
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
		if (count==lvl+1)
			break;
		++i;
	}
	--i;
	while ((i>0)&&(input[i]!='/'))
	{
		ret=input[i]+ret;
		--i;
	}
	return ret.c_str();
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
		ReadStruct(&tmp,i,0);
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
		{
			return i;
		}
	}
	return -1;
}

bool FileIsEmpty(int num)
{
	FILE* src_file;
	bool flag=true;
	src_file = fopen(GetBlockName(num).c_str(),"rb");
	if (src_file!=NULL)
	{
		char c;
		while ((c = fgetc(src_file)) != EOF)
	    {
	        if ((char)c != 0xff)
	        {
	          flag = false; break;}
	    }
		fclose(src_file);			
	}
	else
	{
		perror("File can't be opened!");
	}
	
	return flag;
}

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id,int ololo)
{
	if (ololo)
		{getchar();}
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

int GetDirInDir(string name,int num,int lvl)
{	
	getchar();
    DDirect rt_path;
    int result=-1;
    ifstream block_file(GetBlockName(num).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return -1;
    block_file.read((char*)&rt_path,sizeof(rt_path));
    block_file.close();
    DInode rt_inode;
    ReadStruct(&rt_inode,rt_path.deskr_num,0);
    
    if (rt_inode.file_name==name)
		{
			result = rt_path.deskr_num;
			return result;
		}
		
    if (rt_path.fch!=NULL)
    {
		result = GetDirInDir(name,rt_path.fch,lvl+1);
		if (result>0) return result;
	}
	
	if (rt_inode.nb!=NULL)
    {
		DInode tmp;
		ReadStruct(&tmp,rt_inode.nb,0);	
		result = GetDirInDir(name,tmp.first_block_addr,lvl);
		if (result>0) return result;
	}
	return result;
}

int GetLastDir(string input)
{	
	if (input[1]=='\0')
	{		
		return -1;
	}
	
	Superblock sb;
    ifstream sblock_file(GetBlockName(0).c_str(),ios::binary|ios::in);
    if (!sblock_file.is_open())
		return -1;
    sblock_file.read((char*)&sb,sizeof(sb));
    sblock_file.close();
    
    int num_of_rf;
    int num_of_next_inode;
    
    DInode fnode;
    ifstream iblock_file(GetBlockName(1).c_str(),ios::binary|ios::in);
    if (!iblock_file.is_open())
		return -1;
    iblock_file.read((char*)&fnode,sizeof(fnode));
    
    string path_name = GetFileName(input);
    int num = GetDirInDir(path_name,fnode.first_block_addr,0);
    iblock_file.close();
    return num;
}

void ConvertName(string str_input,char arr_str[])
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
	ReadStruct(&cur,cur_bl,0);
	int ret_val = cur_bl;
	while(cur.nb!=NULL)
	{
		ret_val = cur.nb;
		ReadStruct(&cur,ret_val,0);
	}
	return ret_val;
}

bool WriteData(DFile* input,int block_id,char data[])
{
    ofstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::out|ios::trunc);
     if (!block_file.is_open())
		return false;
    block_file.write((char*)input,sizeof(*input));
    block_file.write(data,(*input).size_of_block);
    block_file.close();
    return true;	
}