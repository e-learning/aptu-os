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
int GetFirstDiskript();
template <class TStruct>
bool ReadStruct(TStruct* input,int block_id);
template <class TStruct>
bool WriteStruct(TStruct* input,int block_id);
string path_to_fs;
bool FileIsEmpty(int num);
int GetFirstDataBlock(int cur);
string GetFileName(string input);
void ConvertName(string str_arr_input,char* str);
bool WriteData(DFile* input,int block_id,char data[]);
void ReadData(DFile* input,int block_id,char data[]);
int GetDirInDir(string name,int num,int lvl);
int GetLastDir(string input);
int CopyFile(DInode src_inode,string dst_path);
int CopyDir(DInode src_inode,DInode* dst_inode,bool flag);
int GetBlockOfLastBr(int cur_bl);
int MakeDirect(DInode src_inode,DInode dst_inode);
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
	string empty = "";
	string name_of_file =  GetFileName(argv[2]);
	if (name_of_file.c_str() !=empty.c_str())
	{
		tmpstr = tmpstr.substr(0,tmpstr.size()-name_of_file.size()-1);
	}
	else
	{
		tmpstr = tmpstr.substr(0,tmpstr.size()-1);
	}
	int tmp_block = GetLastDir(tmpstr);
	tmp_block = (tmp_block==-1)?1:tmp_block;
	DInode file_inode;	
	ReadStruct(&file_inode,tmp_block);
	if (name_of_file.c_str() !=empty.c_str())
	{
		DDirect src_dir;
		ReadStruct(&src_dir,file_inode.first_block_addr); 
		int file_id_block;
		ReadStruct(&file_inode,src_dir.ff);
		file_id_block = src_dir.ff;
		while (file_inode.file_name!=name_of_file)
		{
			if (file_inode.nb!=NULL)
			{
				ReadStruct(&file_inode,file_inode.nb);
				file_id_block = file_inode.nb;
			}
			else
			{
				printf("End of files list in this directory\n\r");
				break;
			}
		}
		CopyFile(file_inode,argv[3]);
	}
	else
	{
		string tmpstr(argv[3]);
		string name_of_file =  GetFileName(argv[3]);
		tmpstr = tmpstr.substr(0,tmpstr.size()-name_of_file.size()-1);
		int tmp_block = GetLastDir(tmpstr);
		tmp_block = (tmp_block==-1)?1:tmp_block;	
		DInode dst_inode;
		ReadStruct(&dst_inode,tmp_block);
		int pind = MakeDirect(file_inode,dst_inode);
		DInode dst_f;
		ReadStruct(&dst_f,pind);
		CopyDir(file_inode,&dst_f,0);
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
void ReadData(DFile* input,int block_id,char data[])
{
    ifstream block_file(GetBlockName(block_id).c_str(),ios::binary|ios::in);
    if (!block_file.is_open())
		return;
    block_file.read((char*)input,sizeof(*input)); 
    block_file.read(data,(*input).size_of_block);
    block_file.close();	
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
int CopyFile(DInode src_inode,string dst_path)
{
	Superblock spblock;
	if (ReadStruct(&spblock,0))
	{
		perror("ooops");
	}	
	int count_of_blocks = src_inode.size; 
	int nbl = count_of_blocks/(spblock.block_size - sizeof(int) - sizeof(int)); 
	int nbtl = count_of_blocks%(spblock.block_size - sizeof(int) - sizeof(int)); 
	if (nbtl!=0) nbl++;
	if (nbl>spblock.tfreeb)
	{
		printf("error003: File is too big for File system.\n\r");
		return 1;
	}
	if (dst_path[0]!='/')
	{
		printf("error002: Destination adress isn't correct.\n\r");
		return 1;
	}
	DInode new_file_node;
	int first_free_diskript = GetFirstDiskript();
	int first_free_block = GetFirstDataBlock(-1);
	ReadStruct(&new_file_node,first_free_diskript);
	new_file_node.size = src_inode.size;
	time(&new_file_node.cr_time);
	time(&new_file_node.lm_time);
	new_file_node.is_file = true;
	new_file_node.first_block_addr = first_free_block;
	new_file_node.nb = NULL;
	string new_name(src_inode.file_name);
	ConvertName(new_name,new_file_node.file_name);	
	int num_of_block = GetLastDir(dst_path); 
	num_of_block = (num_of_block==-1)?1:num_of_block;
	DInode par_dir; 
	ReadStruct(&par_dir,num_of_block);
	DDirect par_dir_dir;
	ReadStruct(&par_dir_dir,par_dir.first_block_addr); 
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
		ReadStruct(&par_dir,lf);
		par_dir.nb = first_free_diskript;
		WriteStruct(&par_dir,lf);
		new_file_node.pb = lf;
	}
	int max_block_size = spblock.block_size - sizeof(int) - sizeof(int);
	int inf_block_adr = src_inode.first_block_addr;
	for (int i=0;i<nbl-1;++i)
	{		
		DFile next_inf_part;
		DFile new_file_block;
		new_file_block.size_of_block = max_block_size;
		char tmp_arr[max_block_size];		
		ReadData(&new_file_block,inf_block_adr,tmp_arr);
		int next_p= GetFirstDataBlock(first_free_block);		
		new_file_block.next_block_part = (nbtl>0)?next_p:NULL;		
		WriteData(&new_file_block,first_free_block,tmp_arr);
		ReadStruct(&next_inf_part,inf_block_adr);
		first_free_block = next_p;	
		inf_block_adr = next_inf_part.next_block_part;
	}
	if (nbtl>0)
	{
		DFile new_file_block;
		new_file_block.size_of_block = nbtl;
		char tmp_arr[max_block_size];		
		ReadData(&new_file_block,inf_block_adr,tmp_arr);
		new_file_block.next_block_part = NULL;
		if (!WriteData(&new_file_block,first_free_block,tmp_arr))
		{
			perror("ololo");
		}
	}
	spblock.tfreeb = spblock.tfreeb - nbl;
	spblock.tinode = spblock.tinode - 1;
	time(&spblock.last_mod);
	WriteStruct(&spblock,0);
	WriteStruct(&new_file_node,first_free_diskript);
	return 0;
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
int GetBlockOfLastBr(int cur_bl)
{
	DInode cur;
	ReadStruct(&cur,cur_bl);
	int ret_val = cur_bl;
	while(cur.nb!=NULL)
	{
		ret_val = cur.nb;
		ReadStruct(&cur,ret_val);
	}
	return ret_val;
}
int MakeDirect(DInode src_inode,DInode dst_inode)
{
	Superblock spblock;
	if (ReadStruct(&spblock,0))
	{
		perror("ooops");
	}
	int first_free_diskript = GetFirstDiskript();
	int first_free_block = GetFirstDataBlock(-1);
	DDirect dst_dir;
	ReadStruct(&dst_dir,dst_inode.first_block_addr);	
	DDirect new_direct;
	DInode new_inode;
	if (dst_dir.fch!=NULL)
	{
		DDirect br_dir;
		ReadStruct(&br_dir,dst_dir.fch);
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
		dst_dir.fch = first_free_block;
		WriteStruct(&dst_dir,dst_inode.first_block_addr);
	}	
	new_inode.is_file = false;
	new_inode.size = 0;
	new_inode.first_block_addr = first_free_block;
	new_inode.par = dst_dir.deskr_num;
	string fn(src_inode.file_name);
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
	return first_free_diskript;
}
int CopyDir(DInode src_inode,DInode* dst_inode,bool flag)
{
	DDirect src_dir;
	ReadStruct(&src_dir,src_inode.first_block_addr);
	if (src_dir.fch!=NULL)
    {
		DInode src_ch_inode;
		DDirect src_ch_dir;
		ReadStruct(&src_ch_dir,src_dir.fch);
		ReadStruct(&src_ch_inode,src_ch_dir.deskr_num);
		int pind = MakeDirect(src_ch_inode,*dst_inode);
		DInode dst_f;
		ReadStruct(&dst_f,pind);
		CopyDir(src_ch_inode,&dst_f,1);
	}
	if ((src_inode.nb!=NULL)&&(flag))
    {
		DInode src_nb_inode;
		ReadStruct(&src_nb_inode,src_inode.nb);
		DInode par_dir_inode;
		ReadStruct(&par_dir_inode,(*dst_inode).par);
		int pind = MakeDirect(src_nb_inode,par_dir_inode);
		DInode dst_f;
		ReadStruct(&dst_f,pind);
		CopyDir(src_nb_inode,&dst_f,1);
	}
	return 0; 
}