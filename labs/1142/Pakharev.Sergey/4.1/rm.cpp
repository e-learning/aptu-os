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
	int s_isize;				
	int tfreeb;					
	int tinode;	
	time_t last_mod;			
	int block_size;				
	int block_count;			
};
struct DInode
{
	int size;				
	time_t cr_time;			
	int first_block_addr;	
	char file_name[max_name_length]; 
	time_t lm_time;			
	bool is_file;			
	int par;				
	int nb;  				
	int pb;					
};
struct DDirect
{
	int deskr_num;	
	int fch; 		
	int ff;  		
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
const char* GetDirName(char* input,int lvl);
bool FileIsEmpty(int num);
string GetFileName(string input);
int GetDirInDir(string name,int num,int lvl);
int GetLastDir(string input);
int main(int argc, char **argv)
{
	if (argc!=3)
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
	int file_id_block;
	ReadStruct(&file_inode,src_dir.ff);
	file_id_block = src_dir.ff;
	while (file_inode.file_name!=name_of_file)
	{
		if (file_inode.nb!=NULL)
		{
			file_id_block = file_inode.nb;
			ReadStruct(&file_inode,file_inode.nb);
		}
		else
		{
			printf("End of files list in this directory\n\r");
			break;
		}
	}
	if (file_inode.nb==NULL)
	{
		if  (file_inode.pb==NULL)
		{
			perror("1");
			DDirect fhrd;
			ReadStruct(&fhrd,file_inode.par);
			fhrd.ff = NULL;
			WriteStruct(&fhrd,file_inode.par);
		}
		else
		{			
			perror("2");
			DInode prev;
			ReadStruct(&prev,file_inode.pb);
			prev.nb = NULL;
			WriteStruct(&prev,file_inode.pb);
		}
	}
	else
	{
		if  (file_inode.pb==NULL)
		{
			DInode nextbr;
			ReadStruct(&nextbr,file_inode.nb);
			nextbr.pb = NULL;
			WriteStruct(&nextbr,file_inode.nb);			
			DDirect fhrd;
			ReadStruct(&fhrd,file_inode.par);
			fhrd.ff = file_inode.nb;
			WriteStruct(&fhrd,file_inode.par);
		}
		else
		{
			DInode prev;
			ReadStruct(&prev,file_inode.pb);			
			DInode nextbr;
			ReadStruct(&nextbr,file_inode.nb);
			prev.nb = file_inode.nb;
			nextbr.pb = file_inode.pb;
			WriteStruct(&prev,file_inode.pb);
			WriteStruct(&nextbr,file_inode.nb);
		}
	}	
	int count_of_blocks = file_inode.size; 
	int nbl = count_of_blocks/(spblock.block_size - sizeof(int) - sizeof(int)); 
	int nbtl = count_of_blocks%(spblock.block_size - sizeof(int) - sizeof(int)); 
	if (nbtl!=0) nbl++;
	spblock.tfreeb = spblock.tfreeb + nbl;
	spblock.tinode = spblock.tinode + 1;
	time(&spblock.last_mod);
	WriteStruct(&spblock,0);
	file_inode.first_block_addr = NULL;
	file_inode.size = 0;
	file_inode.par = NULL;
	WriteStruct(&file_inode,file_id_block);
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
int GetDirInDir(string name,int num,int lvl)
{	
    DDirect rt_path;
    int result=-1;
    ifstream block_file(GetBlockName(num).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return -1;
    block_file.read((char*)&rt_path,sizeof(rt_path)); 
    block_file.close();
    DInode rt_inode;
    ReadStruct(&rt_inode,rt_path.deskr_num);
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
	printf(" input str: %s",input.c_str());
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
