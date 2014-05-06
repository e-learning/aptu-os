#include "inc.h"

#define file 0x1
#define dir 0x2
#define del 0x4

std::string open_dir(char * buffer, int size_block, int data_size, std::string put);
int inf_file(char * s_type, char * buffer, int buffer_length, std::string name_f, int * file_length, char * t_year, char * t_mon, char * t_mday, char * t_hour, char * t_min, char * t_sec);
int search_file_in_dir(int s_type, char * buffer, int buffer_length, std::string name_f, int * file_length, char mode = 0);
int add_file(int type, char * buffer, int buffer_length, std::string name_f, int  file_length);

int size_data(int size_block, int sector);
int search_free_sector();
int write_data(char * buffer2, int size_block, int sector, int data_size);
int load_data(char * buffer2, int size_block, int sector, int data_size);
std::string pars_dir(std::string in_zn, int mode);
void set_current_dir_sector(int sector);
int get_current_dir_sector();
void read_file_record(char * type, int * start_sector, int * size_in_bytes, std::string * str, char * t_year, char * t_mon, char * t_mday, char * t_hour, char * t_min, char * t_sec);
void read_file_record(char * type, int * start_sector, int * size_in_bytes, std::string * str);
void edit_file_record(char type,int start_sector, int size_in_bytes, std::string str);
void add_buffer(char *buff, char *arr, int offset, int offset2, int  size);
void clear_buffer(char * buff, int size);

int format(int count_block);
int create_root_dir();

int init_fs(int * size, int * count);

int read_block(char * buff, int size, int nom);
int write_block(char * buff,int size, int nom);

void set_root(std::string zn_root);
int init(int size_block, int count_block);
int create_block(int size, int nom);
