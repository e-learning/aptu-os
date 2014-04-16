#include <string>
#include <fstream>
#include "except.h"
#include "fsinfo.h"


using std::string;
using std::to_string;

using std::fstream;

FSInfo::FSInfo(const char* path):
	block_size(0),
	block_count(0),
	path(path),
	dirty(false),
	links_table()
{
	string root_path(path);
	fstream block((root_path+"/0").c_str(), std::ios::binary| std::ios::in);

	if (! block){
		throw BadRootPathException();
	}
	b_read(block, block_size);
	b_read(block, block_count);
	b_read(block, fst_free);
	b_read(block, root_entry);
	
	links_table.resize(block_count);
	size_t cur_block = 0;
	for (size_t read_block_links = 0; 
		 read_block_links!= block_count; 
		 ++read_block_links){
		if (block.tellg() > block_size - sizeof(Link)){
			block.close();
			block.open((root_path+"/"+to_string(++cur_block)).c_str());
			if (!block){
				throw BadRootPathException();
			}
		}
		
		b_read(block, links_table[read_block_links]);
	}
	
}



void FSInfo::close(){
	if (dirty){
		fstream block(root_path() + "/" + to_string(0),
					  std::ios::out|std::ios::in|std::ios::binary);
		if (!block){
			throw BadRootPathException();
		}
		b_write(block, block_size);
		b_write(block, block_count);
		b_write(block, fst_free);
		b_write(block, root_entry);
		size_t cur_block = 0;
		for (size_t i = 0; i < block_count; i++){
			if (block.tellp() > block_size - sizeof(Link)){
				block.close();
				block.open((root_path()+"/"+to_string(++cur_block)).c_str());
				if (!block){
					throw BadRootPathException();
				}
			}
			b_write(block, links_table[i]);
		}
		
		dirty = false;
	} 
}


std::pair<Link, Link> FSInfo::allocate(size_t num_of_blocks){
	if (num_of_blocks == 0){
		throw WorkflowException();
	}
	Link fst = fst_free;
	Link lst = fst_free;
	for(size_t i = 1 ; i < num_of_blocks; i++){
		if (lst == 0){
			throw NotEnoughMemoryException();
		}
		lst = getnext(lst);		
	}
	fst_free = getnext(lst);
	links_table[lst] = 0;
	dirty = true;
	return std::make_pair(fst, lst);
}

Link FSInfo::free(const Link& fst, const Link & lst){
	links_table[lst] = fst_free;
	fst_free = fst;
	dirty = true;
	return fst_free;
}

Link FSInfo::extend(const Link& lst){
	if (getnext(fst_free) == 0){
		throw NotEnoughMemoryException();
	}
	Link newlst = getnext(fst_free);
	fst_free = getnext(newlst);
	links_table[lst] = newlst;
	links_table[newlst] = 0;
	dirty = true;
	return newlst;
}
