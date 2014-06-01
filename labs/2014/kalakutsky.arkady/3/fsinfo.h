#ifndef FSINFO
#define FSINFO
 

#include <vector>
#include <string>
#include "util.h"
#include "except.h"

class FSInfo{
  public:
	FSInfo(const char * path);
	~FSInfo(){
		if (dirty) throw WorkflowException();
	}
	size_t block_size;
	size_t block_count;
	dir_entry root_entry;

	std::pair<Link, Link> allocate(size_t num_of_blocks);
	Link free(const Link & fst, const Link & lst);
	Link extend(const Link & lst);

	Link getnext(Link cur)const {
		return links_table[cur];
	}
	void close();

	const std::string & root_path(){
		return path;
	}
	
	size_t free_space(){
		size_t cnt = 0;
		Link cur = fst_free;
		while(cur != 0){
			++cnt;
			cur = getnext(cur);
		}
		return cnt;
	}
  private:
	Link fst_free;
	std::string path;
	bool dirty;
	std::vector<Link> links_table;
};


#endif
