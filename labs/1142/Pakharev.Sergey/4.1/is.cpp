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
string path_to_fs;
string GetDirNameByDirS(DDirect str_dir);
const char* GetDirName(char* input,int lvl);
bool FileIsEmpty(int num);
string GetFileName(string input);
int GetDirInDir(string name,int num,int lvl);
int GetLastDir(string input);
bool PrintDirInfo(int num,int lvl);
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
	string empty = "";
	string name_of_file =  GetFileName(argv[2]); 
	if (name_of_file.c_str() !=empty.c_str())
	{
		printf(" file name %s\n\r",name_of_file.c_str());	
		tmpstr = tmpstr.substr(0,tmpstr.size()-name_of_file.size()-1);
	}
	else
	{
		tmpstr = tmpstr.substr(0,tmpstr.size()-1);
	}
	printf(" last dir %s\n\r",tmpstr.c_str());
	int tmp_block = GetLastDir(tmpstr); 
	tmp_block = (tmp_block==-1)?1:tmp_block;
	printf(" last dir %i\n\r",tmp_block);
	DInode file_inode;	
	ReadStruct(&file_inode,tmp_block); 
	printf(" name of inode file_inode: %s\n\r",file_inode.file_name);
	if (name_of_file.c_str() !=empty.c_str())
	{
		perror("not_file");
		DDirect src_dir;
		ReadStruct(&src_dir,file_inode.first_block_addr); 
		int file_id_block;
		ReadStruct(&file_inode,src_dir.ff);
		file_id_block = src_dir.ff;
		while (file_inode.file_name!=name_of_file)
		{
			printf(" .%s.  -  .%s.\n\r",file_inode.file_name,name_of_file.c_str());
			if (file_inode.nb!=NULL)
			{
				ReadStruct(&file_inode,file_inode.nb);
				file_id_block = file_inode.nb;
				perror("nope");
			}
			else
			{
				printf("End of files list in this directory\n\r");
				break;
			}
		}
		perror("suc");
		printf(" File name = %s\n\r",file_inode.file_name);
		printf(" File size = %i\n\r",file_inode.size);
		printf(" File create time = %s\n\r",ctime(&file_inode.cr_time));
		printf(" File last modification time = %s\n\r",ctime(&file_inode.lm_time));
	}
	else
	{
		DDirect tmp_dir;
		perror("started printing");
		printf(" File first block adr = %i\n\r",file_inode.first_block_addr);
		PrintDirInfo(file_inode.first_block_addr,0);
	}
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
	if (input[last_index] == '\0')
		return "";
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
	printf("count = %i,i=%i\n\r",count,i);
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
    printf("%s\n\r",GetDirNameByDirS(rt_path).c_str());
    if (rt_path.fch!=NULL)
    {
		PrintDirInfo(rt_path.fch,lvl+1);
	}
	if (rt_path.ff!=NULL)
    {
		DInode tmp_file;
		ReadStruct(&tmp_file,rt_path.ff);
		for (int i=0;i<lvl+1;++i)
			printf("-");
		printf("%s\n\r",tmp_file.file_name);
		while (true)
		{
			if (tmp_file.nb==NULL) break;
			int index = tmp_file.nb;
			ReadStruct(&tmp_file,tmp_file.nb);
			for (int i=0;i<lvl+1;++i)
				printf("-");
			printf("%s\n\r",tmp_file.file_name);
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
int GetDirInDir(string name,int num,int lvl)
{	
	printf(" .%s. lvl = %i",name.c_str(),lvl);
	getchar();
    DDirect rt_path;
    int result=-1;
    ifstream block_file(GetBlockName(num).c_str(),ios::binary|ios::in);
     if (!block_file.is_open())
		return -1;
    block_file.read((char*)&rt_path,sizeof(rt_path)); 
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
    perror("start");
    int num = GetDirInDir(path_name,fnode.first_block_addr,0);
    perror("end");
    iblock_file.close();
    return num;
}
