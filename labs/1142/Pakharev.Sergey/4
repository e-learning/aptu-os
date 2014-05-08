#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>

using namespace std;

int chr_to_int(char* buf,int size);
string IntToStr(int x);
string SetBlockName(int i,string path);

int main(int argc, char **argv)
{
	const int min_size = 1024;
	//const int bit_int_byte = 8;
	const int max_mem = 50*1024;
	if (argc!=2)
		{
			printf("Critical error001: Count of parameters!=2");
			return 1;
		}
	string cfg_name("/config");
	string path_to_cfg(argv[1]);
	char buf[10];
	int block_size;
	int block_count;
	FILE* configfile;
	configfile = fopen((path_to_cfg+cfg_name).c_str(),"r");
	//puts((path_to_cfg+cfg_name).c_str());
	if (configfile!=NULL)
	{
		if (fgets (buf , 10 , configfile) != NULL)
		{
			block_size = chr_to_int(buf,10);
			if (block_size<min_size)
				perror("Size of blocks <1024");
			//printf("%i\n\r",block_size);
			//cout<<buf;
		}
		if (fgets (buf , 10 , configfile) != NULL)
		{
			block_count = chr_to_int(buf,10);
			if (block_size*block_count>max_mem)
				perror("Memory from File system over 50M");
			//printf("%i\n\r",block_count);
			//cout<<buf;
		}
		fclose(configfile);
	}
	else
	{
		 perror("Error opening file");
		 return 1;
	}
	printf("1 step\n\r");
	for (int i=0;i<block_count;++i)
	{
		char one_block[block_size];
		
		for (int j=0;j<block_size;j=j+1)
		{
			one_block[j]=0xFF;
		}
		
		FILE* new_block = fopen(SetBlockName(i,path_to_cfg).c_str(),"wb");
		if (new_block==NULL)
		{
			perror("Error writing in block");
		}
		else
		{
			fwrite(one_block,sizeof(char),sizeof(one_block),new_block);
			fclose(new_block);
		}
	}
	printf("File system was successesully created!\n\r");
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

string SetBlockName(int i,string path)
{
	string block_name = path+"/"+IntToStr(i);
	printf("%s\n\r",block_name.c_str());
	return block_name;
}
