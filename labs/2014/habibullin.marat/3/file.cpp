#include "file.h"

#include <fstream>

using std::ifstream;
using std::ofstream;

File::File(size_t block_size, const std::string &ext_path): blocks_(block_size) {
    blocks_.head.SetName(Utils::GetPathTail(ext_path));
    blocks_.head.SetLastModTime(Utils::FileModTime(ext_path));

    ifstream ifs(ext_path);
    while(!ifs.eof()) {
        FileBodyBlock fbb(block_size);
        fbb.SetData(ifs);
        blocks_.Add(fbb);
    }
    blocks_.head.SetSize(blocks_.Size());
}

File::File(size_t block_size, File& other): blocks_(block_size) {
    blocks_.head.SetName(other.Name());
    blocks_.head.SetLastModTime(Utils::CurrentDatetime());
    blocks_.head.SetSize(other.SizeInBlocks());

    typename BlockList<FileHeadBlock, FileBodyBlock>::Iterator iter;
    typename BlockList<FileHeadBlock, FileBodyBlock>::Iterator end = other.blocks_.EndIterator();
    for(iter = other.blocks_.ElemBegIterator(); iter != end; ++iter) {
        char* data = new char[block_size];
        size_t size_bytes = (*iter).GetData(data);
        FileBodyBlock fbb(block_size);
        fbb.SetData(data, size_bytes);
        delete[] data;
        blocks_.Add(fbb);
    }
}

void File::Export(const string &path) {
    ofstream ofs(path);
    typename BlockList<FileHeadBlock, FileBodyBlock>::Iterator iter;
    typename BlockList<FileHeadBlock, FileBodyBlock>::Iterator end = blocks_.EndIterator();
    for(iter = blocks_.ElemBegIterator(); iter != end; ++iter) {
        (*iter).GetData(ofs);
    }
}
