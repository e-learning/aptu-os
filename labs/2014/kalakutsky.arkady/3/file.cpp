#include <fstream>
#include <string>
#include <list>
#include <vector>

#include "except.h"
#include "util.h"
#include "file.h"

using std::fstream;

using std::string;
using std::to_string;

using std::list;
using std::vector;



void parse_path(std::list<string>& path_names, const char* path){
	int state = 0;
	char buf[MAXNAMESIZE+1] ;
	for (size_t i = 0 ; i < MAXNAMESIZE + 1; ++i ){
		buf[i] = 0;
	}

	buf[MAXNAMESIZE] = 0;

	size_t buf_pos = 0;
	for(size_t i = 0; path[i] != 0; i++){
		if (state == 0){
			if (path[i] == '/'){
				state = 1;
			} else {
				throw RelativePathException();
			}
		}else if (state == 1){
			if (path[i]!='/'){
				i -= 1;
				state = 2;
			}
		}else{
			if(path[i] != '/'){
				if (buf_pos == MAXNAMESIZE){
					throw TooLongFileNameException();
				}else{
					buf[buf_pos++] = path[i];
				}
			}else{
				state = 1;
				path_names.push_back(string(buf));
				for (size_t j = 0; j < MAXNAMESIZE; ++j){
					buf[j] = 0;
				}
			}
		}
	}
	if (buf_pos > 0){
		path_names.push_back(string(buf));
	}
}

void open_path(std::vector<File*> & path_stack, FSInfo & fsinfo, const char * path){
	list<string> path_names;
	parse_path(path_names, path);
	while(!path_names.empty()){
		path_stack.push_back(new File(fsinfo, path_stack.back(), 
									  path_names.front().c_str())
			);
		path_names.pop_front();		
	}
	
}


File::File(FSInfo& fsinfo):
	file_data(0),
	entry(fsinfo.root_entry),
	parent(0),
	opened(false),	
	dirty(false),
	fsinfo(fsinfo)
{
	
}

File::File(FSInfo& fsinfo, File *parent, const char* name):
	file_data(0),
	parent(parent),
	opened(false),
	dirty(false),
	fsinfo(fsinfo)
{
	if (!parent->exists() || parent->type() != filetype::DIR){
		throw FileNotFoundException();
	}
	
	entry = parent->getfile(name);
	
}
File::File(FSInfo& fsinfo, File *parent, dir_entry entry):
	file_data(0),
	entry(entry),
	parent(parent),
	opened(false),
	dirty(false),
	fsinfo(fsinfo)
{}



dir_entry File::getfile(const char *name){
	if (type() != filetype::DIR){
		throw NotDirectoryException();
	}
	open();
	const dir_entry* dir_data = reinterpret_cast<dir_entry*>(file_data);
	for (size_t i = 0; i < entry.size / sizeof(dir_entry); ++i){
		if (dir_data[i].isname(name)){
			return dir_data[i];
		}
	}
	return dir_entry(name, filetype::UNDEF, false);
}

void File::rem(dir_entry& entry){
	if (type()!= filetype::DIR){
		throw NotDirectoryException();
	}
	open();
	dir_entry* dir_data = reinterpret_cast<dir_entry*> (file_data);
	for (size_t i = 0; i < entry.size / sizeof(dir_entry); ++i){
		if (dir_data[i].busy && dir_data[i].isname(entry.name)){
			dir_data[i].busy = false;
			dirty = true;
			break;
		}
	}
}

void File::copy_to(File* dst){
	copy_entry_to(entry, dst);

	open();
	dst->file_data = new char[size()];
	for (size_t i = 0; i < size(); i++){
		dst->file_data[i] = file_data[i];
	}
	
	dst->create(entry.type);
	dst->dirty = true;

	dst->write();
	if (type() == filetype::DIR){
		dir_entry* dir_data = reinterpret_cast<dir_entry*> (file_data);
		dir_entry* dst_dir_data =reinterpret_cast<dir_entry*> (dst->file_data);
		for (size_t i = 0; i < entry.size / sizeof(dir_entry); ++i){
			if (dir_data[i].busy){
				File f(fsinfo, this, dir_data[i]);
				dst_dir_data[i].busy = false;
				File d(fsinfo, dst, dir_data[i].name);
				f.copy_to(&d);
				d.write();
			}
		}
		dst->write();
	}
	
}

void File::copy_entry_to(dir_entry& entry, File* dst){
	dst->entry.size = entry.size;
	dst->entry.fst_block = entry.fst_block;
	dst->entry.lst_block = entry.lst_block;
	dst->entry.type = entry.type;
}

void File::move_to(File* dst){
	dst->file_data = file_data;
	file_data = 0;
	copy_entry_to(entry, dst);
	parent->rem(entry);
	dst->parent->add(dst->entry);
}

void File::del(){
	if (type() == filetype::DIR){
		open();
		dir_entry* dir_data = reinterpret_cast<dir_entry*> (file_data);
		for (size_t i = 0; i < entry.size / sizeof(dir_entry); ++i){
			if (dir_data[i].busy){
				File f(fsinfo, this, dir_data[i]);
				f.del();
			}
		}	
	}
	dirty = false;
	fsinfo.free(entry.fst_block, entry.lst_block);
	parent->rem(entry);
}


void File::open(){
	if (! opened){
		opened = true;
		Link block_num = entry.fst_block;
		size_t size_to_read = entry.size;
		file_data = new char[size_to_read];
		for (;block_num != 0 && size_to_read > 0; block_num = fsinfo.getnext(block_num)){
			fstream block((fsinfo.root_path()+"/"+to_string(block_num)).c_str(),
						  std::ios::binary|std::ios::out|std::ios::in);
			if (!block){
				throw BadRootPathException();
			}
			size_to_read -= block.readsome(
				file_data + entry.size - size_to_read,
				size_to_read);
		}
	}
}

void File::create(filetype::filetype t ){
	if (exists()) return;
	if (!file_data){ 
		throw WorkflowException();
	}
	std::pair<Link, Link> bufs = fsinfo.allocate(size() / fsinfo.block_size + 1);
	entry.busy = true;
	entry.fst_block = bufs.first;
	entry.lst_block = bufs.second;
	entry.type = t;
	dirty = true;
	parent->add(entry);
}

void  File::add(dir_entry & f){
	if (type() != filetype::DIR){
		throw NotDirectoryException();
	}

	open();
	dir_entry* dir_data = reinterpret_cast<dir_entry*>(file_data);

	size_t where = static_cast<size_t>(-1);
	for (size_t i = 0; i < entry.size / sizeof(dir_entry); ++i){
		if (dir_data[i].busy && dir_data[i].isname(f.name)){
			throw AlreadyExistsException();
		} else if (! dir_data[i].busy && where == static_cast<size_t>(-1)){
			where = i;
		}
	}

	if (where != static_cast<size_t>(-1)){
		dir_data[where] = f;
		dirty = true;
	}else {
		//todo directory extension
		throw 1234;
	}
}


void File::write(){
	if (dirty){
		Link block_num = entry.fst_block;
		size_t written_size = 0;
		while(block_num != 0){
			fstream block(fsinfo.root_path() + "/" + to_string(block_num),
						  std::ios::in| std::ios::out|std::ios::binary);
			if (!block){
				throw BadRootPathException();
			}
			block.write(file_data + written_size, fsinfo.block_size);
			written_size += fsinfo.block_size;
			block_num = fsinfo.getnext(block_num);
		}
		dirty = false;
	}
}












