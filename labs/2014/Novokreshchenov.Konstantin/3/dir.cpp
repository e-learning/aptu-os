#include "structures.h"
#include "converter.h"

Dir::Dir ():
    name_(""),
    size_(0),
    time_(0),
    startblock_(LAST_BLOCK)
{}

Dir::Dir(std::string const & name, size_t size, size_t time, size_t startblock):
    name_(name),
    size_(size),
    time_(time),
    startblock_(startblock)
{
    blocks_.push_back(startblock_);
}

Dir::Dir(Dir const & dir):
    name_(dir.name_),
    size_(dir.size_),
    time_(dir.time_),
    startblock_(dir.startblock_),
    blocks_(dir.blocks_),
    filerecords_(dir.filerecords_), dirrecords_(dir.dirrecords_)
{}

Dir & Dir::operator= (Dir const & dir)
{
    name_ = dir.name_;
    size_ = dir.size_;
    time_ = dir.time_;
    startblock_ = dir.startblock_;
    blocks_ = dir.blocks_;
    filerecords_ = dir.filerecords_;
    dirrecords_ = dir.dirrecords_;

    return *this;
}

Dir::~Dir()
{
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        delete filerecords_[i];
    }
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        delete dirrecords_[i];
    }
}

size_t Dir::get_recordsCount() const
{ return dirrecords_.size() + filerecords_.size(); }

size_t Dir::get_sizeInBlocks()
{ return blocks_.size(); }

size_t Dir::get_size() {
    size_t size = 0;
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        size += filerecords_[i]->get_size();
    }
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        size += dirrecords_[i]->get_size();
    }
    return size + BYTES_FOR_RECORD * (filerecords_.size() + dirrecords_.size());
}

void Dir::readAboutSelf(std::fstream &fs) { //used after non-parameter constructor
    name_ = read_string_from_bytes(fs, BYTES_FOR_NAME);
    size_ = read_number_from_bytes(fs, BYTES_FOR_FILESIZE);
    time_ = read_number_from_bytes(fs, BYTES_FOR_TIME);
    startblock_ = read_number_from_bytes(fs, BYTES_FOR_BLOCKNUMBER);

    blocks_.clear();
    blocks_.push_back(startblock_);
}

void Dir::readSelf(Root * root)
{
    size_t currentBlockNumber = startblock_;
    while (true) {
        std::string currentBlockName = root->get_rootpath() + "/" + number_to_string(currentBlockNumber);
        std::fstream blockfs(currentBlockName.c_str(),
                             std::fstream::in | std::fstream:: out | std::fstream::binary);
        size_t recordsCount = read_number_from_bytes(blockfs, BYTES_FOR_RECORDS_COUNT);
        for (size_t i = 0; i < recordsCount; ++i) {
            bool isDir = read_number_from_bytes(blockfs, BYTES_FOR_ISDIR);
            if (isDir) {
                Dir* dir = new Dir();
                dir->readAboutSelf(blockfs);
                dirrecords_.push_back(dir);
            }
            else {
                File* file = new File();
                file->readAboutSelf(blockfs);
                filerecords_.push_back(file);
            }
        }
        currentBlockNumber = read_number_from_bytes(blockfs, BYTES_FOR_BLOCKNUMBER);
        if (currentBlockNumber == LAST_BLOCK) {
            break;
        }
        blocks_.push_back(currentBlockNumber);
    }

	for (size_t i = 0; i < dirrecords_.size(); ++i) {
		dirrecords_[i]->readSelf(root);
	}
	for (size_t i = 0; i < filerecords_.size(); ++i) {
		//filerecords_[i]->readSelf(root);
	}
}

void Dir::writeAboutSelf(std::fstream & fs) //with byte for isdir
{
    write_number_to_bytes(fs, BYTES_FOR_ISDIR, 1);
    write_string_to_bytes(fs, BYTES_FOR_NAME, name_);
    write_number_to_bytes(fs, BYTES_FOR_BLOCKSIZE, size_);
    write_number_to_bytes(fs, BYTES_FOR_TIME, time_);
    write_number_to_bytes(fs, BYTES_FOR_BLOCKNUMBER, startblock_);
}

void Dir::writeSelf(Root * root, Bitmap * bitmap)
{
    size_t blockSize = root->get_blockSize();
    size_t recordsInBlock = (blockSize - BYTES_FOR_BLOCKNUMBER - BYTES_FOR_RECORDS_COUNT) / BYTES_FOR_RECORD;
    size_t allrecordsCount = filerecords_.size() + dirrecords_.size();
    size_t writedrecords = 0;
    size_t allwritedrecords = 0;

    BlockIt blockIt = blocks_.begin();
    size_t currentBlockNumber = get_nextBlockForWrite(blockIt, bitmap);
    std::string currentBlockName = root->get_rootpath() + "/" + number_to_string(currentBlockNumber);
    std::fstream blockfs(currentBlockName.c_str(),
                         std::fstream::out | std::fstream::binary | std::fstream::trunc);
    write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT,
                          allrecordsCount - allwritedrecords > recordsInBlock ? recordsInBlock : allrecordsCount - allwritedrecords);
    for (size_t d = 0; d < dirrecords_.size(); ++d) {
        if (writedrecords == recordsInBlock) {
            currentBlockNumber = get_nextBlockForWrite(blockIt, bitmap);
            write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, currentBlockNumber);
            blockfs.close();
            blockfs.open(currentBlockName.c_str(),
                         std::fstream::out | std::fstream::binary | std::fstream::trunc);
            writedrecords = 0;
            write_number_to_bytes(blockfs, BYTES_FOR_RECORDS_COUNT,
                                  allrecordsCount - allwritedrecords > recordsInBlock
                                  ? recordsInBlock
                                  : allrecordsCount - allwritedrecords);
        }
        dirrecords_[d]->writeAboutSelf(blockfs);
        ++writedrecords;
        ++allwritedrecords;
    }
    for (size_t f = 0; f < filerecords_.size(); ++f) {
        if (writedrecords == recordsInBlock) {
            currentBlockNumber = get_nextBlockForWrite(blockIt, bitmap);
            write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, currentBlockNumber);
            blockfs.close();
            blockfs.open(currentBlockName.c_str(),
                         std::fstream::out | std::fstream::binary | std::fstream::trunc);
            writedrecords = 0;
            write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER,
                                  allrecordsCount - allwritedrecords > recordsInBlock
                                  ? recordsInBlock
                                  : allrecordsCount - allwritedrecords);
        }
        filerecords_[f]->writeAboutSelf(blockfs);
        ++writedrecords;
        ++allwritedrecords;
    }
    write_number_to_bytes(blockfs, BYTES_FOR_BLOCKNUMBER, LAST_BLOCK);
    blockfs.close();

    if (blockIt != blocks_.end()) {
        for (std::vector<size_t>::iterator it = blockIt; it != blocks_.end(); ++it) {
            bitmap->set_blockFree(*it);
        }
    }

	for (size_t i = 0; i < dirrecords_.size(); ++i) {
		dirrecords_[i]->writeSelf(root, bitmap);
	}
	for (size_t i = 0; i < filerecords_.size(); ++i) {
		//filerecords_[i]->writeSelf(root, bitmap);
	}
}

size_t Dir::get_nextBlockForWrite(BlockIt & it, Bitmap * bitmap)
{
    size_t blockNumber = LAST_BLOCK;
    if (it != blocks_.end()) {
        blockNumber = *it;
        ++it;
    } else {
        blockNumber = bitmap->get_nextFreeBlock();
    }
	bitmap->set_blockBusy(blockNumber);
    return blockNumber;
}

void Dir::deleteSelf(Root * root, Bitmap * bitmap)
{
    for (size_t i = 0; i < filerecords_.size(); ++i) {
		filerecords_[i]->readSelf(root);
        filerecords_[i]->deleteSelf(root, bitmap);
    }
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        dirrecords_[i]->deleteSelf(root, bitmap);
    }

    for (BlockIt it = blocks_.begin(); it != blocks_.end(); ++it) {
        std::string truncedBlockName = root->get_rootpath() + "/" + number_to_string(*it);
        std::fstream blockfs (truncedBlockName.c_str(),
                              std::fstream::out | std::fstream::binary | std::fstream::trunc);
        bitmap->set_blockFree(*it);
        blockfs.close();
    }

}

void Dir::add_filerecord (File * file)
{
    filerecords_.push_back(file);
}

void Dir::add_dirrecord (Dir * dir)
{
    dirrecords_.push_back(dir);
}

void Dir::copy_filerecord (Bitmap * bitmap, File * file)
{
    size_t startFileBlock = bitmap->get_nextFreeBlock();
    bitmap->set_blockBusy(startFileBlock);
    File * newfile = new File(file->name_, file->size_, get_currenttime(), startFileBlock);
    newfile->read_fsfile(file);
    filerecords_.push_back(newfile);
}

void Dir::copy_dirrecord (Bitmap * bitmap, Dir * dir)
{
    size_t startDirBlock = bitmap->get_nextFreeBlock();
    bitmap->set_blockBusy(startDirBlock);
    Dir * newdir = new Dir(dir->name_, dir->size_, get_currenttime(), startDirBlock);
    newdir->filerecords_ = dir->filerecords_;
    newdir->dirrecords_ = dir->dirrecords_;
    dirrecords_.push_back(dir);
}

void Dir::remove_file(std::string const & filename)
{
    size_t i = 0;
    for (; i < filerecords_.size(); ++i) {
        if (filerecords_[i]->name_ == filename) {
			filerecords_.erase(filerecords_.begin() + i);
            break;
        }
    }
    
}

void Dir::remove_subdir(std::string const & dirname)
{
    size_t i = 0;
    for (; i < dirrecords_.size(); ++i) {
        if (dirrecords_[i]->name_ == dirname) {
			dirrecords_.erase(dirrecords_.begin() + i);
            break;
        }
    }
    
}

File* Dir::find_file_by_name (std::string const & filename)
{
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        if (filerecords_[i]->name_ == filename) {
            return filerecords_[i];
        }
    }
    return NULL;
}

Dir* Dir::find_dir_by_name (std::string const & dirname)
{
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        if (dirrecords_[i]->name_ == dirname) {
            return dirrecords_[i];
        }
    }
    return NULL;
}

void Dir::printInfo()
{
    std::cout << "Type: Dir" << std::endl <<
                 "Name: " << name_ << std::endl <<
                 "Size: " << get_size() << " B" << std::endl <<
                 "Date: " << timeinfo_to_string(time_) << std::endl;
    for (size_t i = 0; i < dirrecords_.size(); ++i) {
        std::cout << "   --> Dir: " << dirrecords_[i]->name_ << std::endl;
    }
    for (size_t i = 0; i < filerecords_.size(); ++i) {
        std::cout << "   --> File: " << filerecords_[i]->name_ <<std::endl;
    }
}

void Dir::copy_records(Dir * srcDir)
{
	for (size_t i = 0; i < srcDir->dirrecords_.size(); ++i) {
		dirrecords_.push_back(new Dir(*srcDir->dirrecords_[i]));
	}
	for (size_t i = 0; i < srcDir->filerecords_.size(); ++i) {
		filerecords_.push_back(new File(*srcDir->filerecords_[i]));
	}
}

void Dir::copy_newrecords(Root * root, Bitmap * bitmap, Dir * srcDir)
{
	for (size_t i = 0; i < srcDir->filerecords_.size(); ++i)
	{
		File * copiedfile = srcDir->filerecords_[i];
		size_t startFileBlock = bitmap->get_nextFreeBlock();
		bitmap->set_blockBusy(startFileBlock);
		File * newfile = new File(copiedfile->name_, copiedfile->size_, copiedfile->time_, startFileBlock);
		copiedfile->readSelf(root);
		newfile->read_fsfile(copiedfile);
		newfile->writeSelf(root, bitmap);
		(this->filerecords_).push_back(newfile);
	}

	for (size_t i = 0; i < srcDir->dirrecords_.size(); ++i)
	{
		Dir * copieddir = srcDir->dirrecords_[i];
		size_t startDirBlock = bitmap->get_nextFreeBlock();
		bitmap->set_blockBusy(startDirBlock);
		Dir * newdir = new Dir(copieddir->name_, 0, copieddir->time_, startDirBlock);
		newdir->copy_newrecords(root, bitmap, copieddir);
		this->dirrecords_.push_back(newdir);
	}
}