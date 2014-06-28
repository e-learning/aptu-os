#include "filesystem.h"
#include "directory_iterator.h"
Directory_iterator::Directory_iterator(File_system*  fs, Block*  b, bool skipEmpty) {
    current_block = b;
    directory_header = (Directory_header*)b->get_data();
    current_file_entry = (File_entry*)(b->get_data()+sizeof(Directory_header));
    position = 0;
    skip = skipEmpty;
    file_system = fs;
}

bool Directory_iterator::hasNext() const {
    return position < directory_header->files_in_dir;
}

File_entry*  Directory_iterator::current() {
    return current_file_entry;
}

//return pointer to next existing file entry
File_entry*  Directory_iterator::next() {
    while(1) {
        ++current_file_entry;
        if((char*)current_file_entry+sizeof(File_entry) >
                current_block->get_data()+current_block->get_size()) {
            current_block = file_system->get_block(current_block->get_header()->next_block);
            current_file_entry = (File_entry*)current_block->get_data();
        }
        if(current_file_entry->attr &  EXIST_FLAG) {
            position++;
            break;
        } else if(!skip) break;
    }
    return current_file_entry;
}

//the file entry is the last in dir
bool Directory_iterator::atBorder() const {
    return ((char*)current_file_entry+2*sizeof(File_entry) >
            current_block->get_data()+current_block->get_size());
}

//current block of iterator
Block*  Directory_iterator::currentBlock() {
    return current_block;
}

Directory_header*  Directory_iterator::dirHeader() {
    return directory_header;
}


