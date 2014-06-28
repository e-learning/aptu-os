#ifndef DIRECTORY_ITERATOR_H
#define DIRECTORY_ITERATOR_H


class Directory_iterator {
public:
    Directory_iterator(File_system* fs, Block*  b, bool skipEmpty = true);
    File_entry* current();
    File_entry* next();
    Block* currentBlock();
    Directory_header* dirHeader();
    bool atBorder() const;
    bool hasNext() const;

private:
    Block* current_block;
    File_entry* current_file_entry;
    Directory_header* directory_header;
    File_system* file_system;
    bool skip;
    size_t position;
};

#endif // DIRECTORY_ITERATOR_H
