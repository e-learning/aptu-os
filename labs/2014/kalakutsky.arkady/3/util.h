#ifndef UTIL_H 
#define UTIL_H

#include <fstream>
#include <iostream>
#include <ctime>


/***************************************
 **        CONSTS                     **
 **************************************/
static const size_t MAXNAMESIZE = 10;

/***************************************
 **       ASSERTS                     **
 **************************************/
void assertFile(bool file);

/***************************************
 **     DATA TYPES                    **
 **************************************/
typedef size_t Link;


namespace filetype{
	enum filetype{
		UNDEF, BIN, DIR
	};
}

struct dir_entry{
    dir_entry():busy(false),
		name(),
		type(filetype::UNDEF),
		mod_time(0),
		size(0),
		fst_block(0),
		lst_block(0)
		{}

   dir_entry(const char* name, filetype::filetype ft, 
		  bool bs = true  ):busy(bs),
		name(),
		type(ft),
		mod_time(time(0)),
		size(0),
		fst_block(0),
		lst_block(0)
		{
			for(size_t i = 0; i < MAXNAMESIZE; ++i){
				if (!name[i]){
					break;
				}else{
					this->name[i] = name[i];
				}
			}
		}

	bool isname(const char * name) const{
		size_t i = 0;
		for (;i < MAXNAMESIZE; ++i){
			if (! name[i]|| ! this->name[i]){
				break;
			}
			if (name[i] != this->name[i]){
				return false;
			}
		}
		if (name[i] != this->name[i]){
			return false;
		}
		return true;
	}

	bool busy;
	char name[MAXNAMESIZE];
	filetype::filetype type;
	time_t mod_time;
	size_t size;
	size_t fst_block;
	size_t lst_block;
};
namespace std{
	ostream & operator<<(ostream & out, const dir_entry & de);
}
/***************************************
 **      FUNCTIONS                    **
 **************************************/
size_t calc_busy_list_size(size_t block_size, size_t block_count);

size_t busy_list_offset();


template<class T>
void b_write(std::ofstream & file, T & data){
	file.write(reinterpret_cast<char*>(&data), sizeof(T));
}
template<class T>
void b_read(std::ifstream & file, T & data){
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
}
template<class T>
void b_write(std::fstream & file, T & data){
	file.write(reinterpret_cast<char*>(&data), sizeof(T));
}
template<class T>
void b_read(std::fstream & file, T & data){
	file.read(reinterpret_cast<char*>(&data), sizeof(T));
}
#endif
