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

template <class TStruct>
bool ReadStruct(TStruct* input,int block_id);

template <class TStruct>
bool WriteStruct(TStruct* input,int block_id);

string path_to_fs;
string GetDirNameByDirS(DDirect str_dir);

std::ifstream::pos_type filesize(const char* filename);
const char* GetDirName(char* input,int lvl);

bool FileIsEmpty(int num);
string GetFileName(string input);

void ReadData(DFile* input,int block_id,char data[]);
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
	if (ReadStruct(&spblock,0))
	{
		perror("ooops");
	}
	string tmpstr(argv[2]);
	string name_of_file =  GetFileName(argv[2]);
	tmpstr = tmpstr.substr(0,tmpstr.size()-name_of_file.size()-1);
	int tmp_block = GetLastDir(tmpstr);
	tmp_block = (tmp_block==-1)?1:tmp_block;
		
	DInode file_inode; 
	ReadStruct(&file_inode,tmp_block);
	DDirect src_dir;
	ReadStruct(&src_dir,file_inode.first_block_addr);
	
	ReadStruct(&file_inode,src_dir.ff);
	while (file_inode.file_name!=name_of_file.c_str())
	{
		if ((file_inode.nb==NULL)&&(file_inode.file_name!=name_of_file.c_str()))
		{
			int res = (file_inode.file_name!=name_of_file);
			printf("End of files list in this directory\n\r");
			break;
		}
		ReadStruct(&file_inode,file_inode.nb);
	}
	string outputFileName(argv[3]);
	outputFileName+=file_inode.file_name;
	ofstream output_file(outputFileName.c_str(),ios::binary|ios::out|ios::trunc);
    if (!output_file.is_open())
		{
			perror("file not open");
			return 1;
		}
	DFile tmp_file;
	ReadStruct(&tmp_file,file_inode.first_block_addr);
	
	int index = file_inode.first_block_addr;
	int k=0;
	do
	{		
		char tmp_arr[tmp_file.size_of_block];
		ReadData(&tmp_file,index,tmp_arr);
		output_file.write(tmp_arr,tmp_file.size_of_block);
		index=tmp_file.next_block_part;
		if (index==NULL) break;
		ReadStruct(&tmp_file,index);
		++k;
	}while (tmp_file.next_block_part!=NULL);
	printf("written blocks = %i",k);
    output_file.close();
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
	//printf("%s\n\r",block_name.c_str());
	return block_name;
}

bool FileIsEmpty(int num)
{
	FILE* src_file;
	//bool flag;
	//int count =0 ;
	src_file = fopen(GetBlockName(num).c_str(),"rb");
	//char* buf;
	if (src_file!=NULL)
	{
		char c;
		while ((c = fgetc(src_file)) != EOF)
	    {
	        if ((char)c != 0xff)
	        {//printf("%x",c);
	         fclose(src_file); return false;}
	    }
		//do {
			//c = (char)fgetc (src_file);
			//if (c != 0xFF) {printf("%c ",c); fclose(src_file); return false;}
			//count++;
			//} while (c != EOF);	
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

bool PrintDirInfo(int num,int lvl)
{	
    DDirect rt_path;
    //printf("block_addr = %i",num);
    ifstream block_file(GetBlockName(num).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return 1;
    block_file.read((char*)&rt_path,sizeof(rt_path)); //Читаем структуру целиком сразу
    block_file.close();
    DInode rt_inode;
    ReadStruct(&rt_inode,rt_path.deskr_num);
    
    for (int i=0;i<lvl;++i)
		printf("-");
    //printf("%s\n\r",GetDirNameByDirS(rt_path).c_str());
    if (rt_path.fch>0)
    {
		PrintDirInfo(rt_path.fch,lvl+1);
	}
	if (rt_inode.nb>0)
    {
		for (int i=0;i<lvl;++i)
			printf("-");
		PrintDirInfo(rt_inode.nb,lvl+1);
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

void ReadData(DFile* input,int block_id,char data[])
{
    ifstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::in);
    if (!block_file.is_open())
		return;
    block_file.read((char*)input,sizeof(*input)); //Читаем структуру целиком сразу
    
    block_file.read(data,(*input).size_of_block);
    block_file.close();	
}

int GetDirInDir(string name,int num,int lvl)
{	
	printf(" .%s. lvl = %i",name.c_str(),lvl);
	getchar();
    DDirect rt_path;
    int result=-1;
    ifstream block_file(GetBlockName(num).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return -1;
    block_file.read((char*)&rt_path,sizeof(rt_path)); //Читаем структуру целиком сразу
    block_file.close();
    DInode rt_inode;
    ReadStruct(&rt_inode,rt_path.deskr_num);
    
    printf(" cur_dir: .%s. ,flag = %i, name: .%s.\n\r",rt_inode.file_name,rt_inode.file_name==name,name.c_str());
    
    if (rt_inode.file_name==name)
		{
			result = rt_path.deskr_num;
			perror("result found");
			return result;
		}
		
    if (rt_path.fch!=NULL)
    {
		perror("child found");
		result = GetDirInDir(name,rt_path.fch,lvl+1);
		if (result>0) return result;
	}
	
	if (rt_inode.nb!=NULL)
    {
		DInode tmp;
		perror("brother found");
		ReadStruct(&tmp,rt_inode.nb);	
		result = GetDirInDir(name,tmp.first_block_addr,lvl);
		if (result>0) return result;
	}
	return result;
}

int GetLastDir(string input)
{	
	if (input[1]=='\0')
	{		
		perror("exit_GetBlock");
		return -1;
	}
	
	//открываем файл для чтения
	Superblock sb;
    ifstream sblock_file(GetBlockName(0).c_str(),ios::binary|ios::in);
    if (!sblock_file.is_open())
		return -1;
    sblock_file.read((char*)&sb,sizeof(sb)); //Читаем структуру целиком сразу
    sblock_file.close();
    
    int num_of_rf;
    int num_of_next_inode;
    
    DInode fnode;
    ifstream iblock_file(GetBlockName(1).c_str(),ios::binary|ios::in);
    if (!iblock_file.is_open())
		return -1;
    iblock_file.read((char*)&fnode,sizeof(fnode)); //Читаем структуру целиком сразу    
    
    string path_name = GetFileName(input);
    perror("start");
    int num = GetDirInDir(path_name,fnode.first_block_addr,0);
    perror("end");
    iblock_file.close();
    return num;
}
