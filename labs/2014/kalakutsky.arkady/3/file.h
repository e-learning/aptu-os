#ifndef FILE
#define FILE

#include "fsinfo.h"
#include "util.h"

class File{
  public:
	File(FSInfo& fsinfo);
	File(FSInfo& fsinfo, File* parent, dir_entry entry);
	File(FSInfo& fsinfo, File* parent, const char* name);
	~File(){
		if (dirty) throw WorkflowException();
		delete [] file_data;
	}
	
	bool exists() const{
		return entry.busy;
	}
	dir_entry getfile(const char * name) ;
	void open();

	filetype::filetype type() const{
		return entry.type;
	}

	void del();

	size_t size() const{
		return entry.size;
	}
	void size(size_t s) {
		entry.size = s;
	}
	size_t block_size() const{
		return size() / fsinfo.block_size +1;
	}
	std::string name() const{
		char buf[MAXNAMESIZE + 1];
		buf[MAXNAMESIZE] = 0;
		for (size_t i = 0; i != MAXNAMESIZE; i++ ){
			buf[i] = entry.name[i];
		}
		return std::string(buf);
	}
	const char * time() const{
		return ctime(&entry.mod_time);
	}
	std::string fstlst() const{
		return std::to_string(entry.fst_block)+ " "+std::to_string(entry.lst_block);
	}
	
	void create( filetype::filetype t = filetype::BIN);
	void add(dir_entry & f);
	void rem(dir_entry & f);
	void write();
	
	void copy_to(File* dst);
	void move_to(File* dst);

	char * file_data;
  private:
	dir_entry entry;
	File * parent;
	bool opened;
	bool dirty;
	FSInfo & fsinfo;
	
	void copy_entry_to(dir_entry& entry, File* dst);
};

void open_path(std::vector<File*> & path_stack, FSInfo & fsinfo, 
			   const char * path);

#endif
