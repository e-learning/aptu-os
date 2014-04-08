#include "util.h"
void assertFile(bool file){
	if (! file){
		std::cerr << "Can't open file" << std::endl;
		exit(1); 
	}
}


size_t calc_busy_list_size(size_t block_size, size_t block_count) {
	return block_count*sizeof(Link);
}



size_t busy_list_offset(){
	return sizeof(size_t) + //size
 		sizeof(size_t) +  //count
		sizeof(Link) +  // fst free
		sizeof(dir_entry); //root entry
}

namespace std{
	std::ostream & operator<<(std::ostream& out, const dir_entry& entry){
		char buf[MAXNAMESIZE + 1];
		buf[MAXNAMESIZE] = 0;
		for (size_t i = 0; i != MAXNAMESIZE; i++ ){
			buf[i] = entry.name[i];
		}
		out << buf << " ";
		out << entry.size << " ";
		out << ctime(&entry.mod_time) ;
		return out;
	}
}


