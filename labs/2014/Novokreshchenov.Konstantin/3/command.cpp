#include "Const.h"
#include "command.h"
#include "converter.h"
#include "structures.h"
#include <fstream>

size_t init_fileSystem(std::string const & rootpath)
{
    size_t blocksize = 0;
    size_t blockscount = 0;
    std::fstream configfs ((rootpath + "/" + CONFIG).c_str(),
                           std::fstream::in | std::fstream::out | std::fstream::binary);
    if (configfs.is_open()) {
        configfs >> blocksize;
        configfs >> blockscount;
    }
    else {
        return ERROR_READ_CONFIG;
    }
    std::fstream blockfs;
    for (size_t i = 0; i < blockscount; ++i) {
        std::string filename = rootpath + "/" + number_to_string(i);
        blockfs.open(filename.c_str(),
                     std::fstream::out | std::fstream::binary | std::fstream::trunc);
        //fill_block_by_char(blockfs, blocksize, '\0');
        blockfs.close();
    }

    return SUCCESS;
}

size_t format(std::string const & rootpath)
{
    size_t result = init_fileSystem(rootpath);
    if (result != SUCCESS) {
        return result;
    }
    size_t blocksize = 0;
    size_t blockscount = 0;
    std::fstream configfs ((rootpath + "/" + CONFIG).c_str(),
                           std::fstream::in | std::fstream::binary);
    if (configfs.is_open()) {
        configfs >> blocksize;
        configfs >> blockscount;
    }
    else {
        return ERROR_READ_CONFIG;
    }
    configfs.close();

    size_t bitmapCount = blockscount / blocksize + 1;

    Dir * rootDir = new Dir("/", 0, get_currenttime(), bitmapCount + 1);
    Root * root = new Root(rootpath,
                           blockscount,
                           blocksize,
                           bitmapCount,
                           blockscount,
                           rootDir);
    Bitmap * bitmap = new Bitmap(root);
    bitmap->initSelf(root);
	for (size_t i = 0; i < 1 + bitmapCount + 1; ++i) {
		bitmap->set_blockBusy(i);
	}
    bitmap->writeSelf(root);
    SaveFS(root, bitmap, rootDir);

    return SUCCESS;
}

void SaveFS(Root * root, Bitmap * bitmap, Dir * rootDir)
{
    rootDir->writeSelf(root, bitmap);
    bitmap->writeSelf(root);
    root->writeSelf();

    delete bitmap;
    delete root;
}

size_t create_dirpath (Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end)
{
    if (bitmap->get_freeBlocksCount() < (size_t)(end - current) + 1) {
        return ERROR_NOT_ENOUGH_SPACE;
    }
    if (current == end) {
        return SUCCESS;
    }
    std::string localname = *current;
    if (parentDir->find_file_by_name(localname) != NULL) {
        return ERROR_INCORRECT_PATH;
    }
    Dir* subdir = parentDir->find_dir_by_name(*current);
    if (subdir != NULL) {
        return create_dirpath(bitmap, subdir, current + 1, end);;
    }
    size_t subdirStartBlockNumber = bitmap->get_nextFreeBlock();
    bitmap->set_blockBusy(subdirStartBlockNumber);
    subdir = new Dir(*current, 0, get_currenttime(), subdirStartBlockNumber);
    parentDir->add_dirrecord(subdir);
    return create_dirpath(bitmap, subdir, current + 1, end);
}

size_t import (Root * root, Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end, std::string const & hostfile)
{
    if (current == end) {
        if (parentDir->find_dir_by_name(*current) != NULL) {
            return ERROR_INCORRECT_PATH;
        }
		File * file = parentDir->find_file_by_name(*current);
		if (file == NULL) {
			size_t startblockNumber = bitmap->get_nextFreeBlock();
			bitmap->set_blockBusy(startblockNumber);
			file = new File(*current, get_filesize_by_name(hostfile), get_currenttime(), startblockNumber);
			parentDir->add_filerecord(file);
		}
		else {
			file->readSelf(root);
			file->deleteSelf(root, bitmap);
		}
        file->read_hostfile(hostfile);
		file->writeSelf(root, bitmap);
        return SUCCESS;
    }
    Dir * subdir = parentDir->find_dir_by_name(*current);
    if (subdir == NULL) {
        return ERROR_INCORRECT_PATH;
    }
    return import(root, bitmap, subdir, current + 1, end, hostfile);
}

size_t exportFile (Root * root, Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end, std::string const & hostfile)
{
    if (current == end) {
        File* file = parentDir->find_file_by_name(*current);
        if (file == NULL) {
            return ERROR_INCORRECT_PATH;
        }
		file->readSelf(root);
        file->write_hostfile(hostfile);
        return SUCCESS;
    }
    Dir* subdir = parentDir->find_dir_by_name(*current);
    if (subdir == NULL) {
        return ERROR_INCORRECT_PATH;
    }
    return exportFile(root, bitmap, subdir, current + 1, end, hostfile);
}

size_t print_ls (Dir * parentDir, vecStrIt current, vecStrIt end)
{
    if (current == end) {
        parentDir->printInfo();
        return SUCCESS;
    }
    Dir* subdir = parentDir->find_dir_by_name(*current);
    if (subdir != NULL) {
        return print_ls(subdir, current + 1, end);
    }
    File* file = parentDir->find_file_by_name(*current);
    if ((file != NULL) && (current == (end - 1))) {
        file->printInfo();
        return SUCCESS;
    }
    return ERROR_INCORRECT_PATH;
}

size_t delete_rm (Root * root, Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end)
{
    if (current == end) {
        Dir* subdir = parentDir->find_dir_by_name(*current);
        if (subdir == NULL) {
            File* file = parentDir->find_file_by_name(*current);
            if (file == NULL) {
                return ERROR_INCORRECT_PATH;
            }
			file->readSelf(root);
            file->deleteSelf(root, bitmap);
            parentDir->remove_file(file->name_);
			delete file;
            return SUCCESS;
        }
        subdir->deleteSelf(root, bitmap);
        parentDir->remove_subdir(subdir->name_);
		delete subdir;
        return SUCCESS;
    }
    Dir* subdir = parentDir->find_dir_by_name(*current);
    if (subdir != NULL) {
        return delete_rm(root, bitmap, subdir, current + 1, end);
    }
    return ERROR_INCORRECT_PATH;
}


size_t move_dir_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcDirName, std::vector<std::string> & vpathDst)
{
	Dir * dstDir = rootDir;
	vecStrIt current = vpathDst.begin();

	while (current != vpathDst.end()) {
		File * file = dstDir->find_file_by_name(*current);
		if (file != NULL) {
			return ERROR_INCORRECT_PATH;
		}
		Dir * subdir = dstDir->find_dir_by_name(*current);
		if (subdir == NULL) {
			if (current != vpathDst.end() - 1) {
				return ERROR_INCORRECT_PATH;
			}
			size_t startDirBlock = bitmap->get_nextFreeBlock();
			bitmap->set_blockBusy(startDirBlock);
			subdir = new Dir(*current, 0, get_currenttime(), startDirBlock);
			dstDir->add_dirrecord(subdir);
		}
		current += 1;
		dstDir = subdir;
	}
	move_dir_to_dir(parentSrcDir, srcDirName, dstDir);
	return SUCCESS;
}

void move_dir_to_dir(Dir * parentSrcDir, std::string const & srcDirName, 
	Dir * dstDir)
{
	Dir * srcDir = parentSrcDir->find_dir_by_name(srcDirName);
	parentSrcDir->remove_subdir(srcDirName);
	dstDir->copy_records(srcDir);
	delete srcDir;
}


size_t move_file_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcFileName, std::vector<std::string> & vpathDst)
{
	Dir * parentDstDir = rootDir;
	vecStrIt current = vpathDst.begin();
	while (current != vpathDst.end()) {
		Dir * subdir = parentDstDir->find_dir_by_name(*current);
		if (subdir == NULL) {
			if (current == vpathDst.end() - 1) {
				File * file = parentDstDir->find_file_by_name(*current);
				if (file == NULL) {
					file = new File(*current, 0, get_currenttime(), bitmap->get_nextFreeBlock());
					parentDstDir->add_filerecord(file);
				}
				move_file_to_file(root, bitmap, parentSrcDir, srcFileName, parentDstDir, file->name_);
				return SUCCESS;
			}
			return ERROR_INCORRECT_PATH;
		}
		parentDstDir = subdir;
		current += 1;
	}
	if (parentDstDir->find_file_by_name(srcFileName) || parentDstDir->find_dir_by_name(srcFileName)) {
		return ERROR_INCORRECT_PATH;
	}
	parentDstDir->add_filerecord(new File (srcFileName, 0, get_currenttime(), bitmap->get_nextFreeBlock()));
	move_file_to_file(root, bitmap, parentSrcDir, srcFileName, parentDstDir, srcFileName);
	return SUCCESS;
}

void move_file_to_file(Root * root, Bitmap * bitmap, Dir * parentSrcDir, std::string const & srcFileName,
	Dir * parentDstDir, std::string const & dstFileName)
{
	File * srcfile = parentSrcDir->find_file_by_name(srcFileName);
	File * dstfile = parentDstDir->find_file_by_name(dstFileName);
	parentSrcDir->remove_file(srcFileName);
	parentDstDir->remove_file(dstFileName);
	dstfile->readSelf(root);
	dstfile->deleteSelf(root, bitmap);
	srcfile->name_ = dstfile->name_;
	parentDstDir->add_filerecord(srcfile);
	delete dstfile;
}



size_t copy_dir_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcDirName, std::vector<std::string> & vpathDst)
{
	Dir * dstDir = rootDir;
	vecStrIt current = vpathDst.begin();

	while (current != vpathDst.end()) {
		File * file = dstDir->find_file_by_name(*current);
		if (file != NULL) {
			return ERROR_INCORRECT_PATH;
		}
		Dir * subdir = dstDir->find_dir_by_name(*current);
		if (subdir == NULL) {
			if (current != vpathDst.end() - 1) {
				return ERROR_INCORRECT_PATH;
			}
			size_t startDirBlock = bitmap->get_nextFreeBlock();
			bitmap->set_blockBusy(startDirBlock);
			subdir = new Dir(*current, 0, get_currenttime(), startDirBlock);
			dstDir->add_dirrecord(subdir);
			copy_dir_to_dir(root, bitmap, parentSrcDir, srcDirName, subdir);
			return SUCCESS;
		}
		current += 1;
		dstDir = subdir;
	}
	size_t startDirBlock = bitmap->get_nextFreeBlock();
	bitmap->set_blockBusy(startDirBlock);
	Dir * subdir = new Dir(srcDirName, 0, get_currenttime(), startDirBlock);
	dstDir->add_dirrecord(subdir);
	copy_dir_to_dir(root, bitmap, parentSrcDir, srcDirName, subdir);
	return SUCCESS;
}

void copy_dir_to_dir(Root * root, Bitmap * bitmap, Dir * parentSrcDir, std::string const & srcDirName,
	Dir * dstDir)
{
	Dir * srcDir = parentSrcDir->find_dir_by_name(srcDirName);
	dstDir->copy_newrecords(root, bitmap, srcDir);
}

size_t copy_file_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcFileName, std::vector<std::string> & vpathDst)
{
	Dir * parentDstDir = rootDir;
	vecStrIt current = vpathDst.begin();
	while (current != vpathDst.end()) {
		Dir * subdir = parentDstDir->find_dir_by_name(*current);
		if (subdir == NULL) {
			if (current == vpathDst.end() - 1) {
				File * file = parentDstDir->find_file_by_name(*current);
				if (file == NULL) {
					file = new File(*current, 0, get_currenttime(), bitmap->get_nextFreeBlock());
					parentDstDir->add_filerecord(file);
				}
				copy_file_to_file(root, bitmap, parentSrcDir, srcFileName, parentDstDir, file->name_);
				return SUCCESS;
			}
			return ERROR_INCORRECT_PATH;
		}
		parentDstDir = subdir;
		current += 1;
	}
	if (parentDstDir->find_file_by_name(srcFileName) || parentDstDir->find_dir_by_name(srcFileName)) {
		return ERROR_INCORRECT_PATH;
	}
	parentDstDir->add_filerecord(new File(srcFileName, 0, get_currenttime(), bitmap->get_nextFreeBlock()));
	copy_file_to_file(root, bitmap, parentSrcDir, srcFileName, parentDstDir, srcFileName);
	return SUCCESS;
}

void copy_file_to_file(Root * root, Bitmap * bitmap, Dir * parentSrcDir, std::string const & srcFileName,
	Dir * parentDstDir, std::string const & dstFileName)
{
	File * srcfile = parentSrcDir->find_file_by_name(srcFileName);
	File * dstfile = parentDstDir->find_file_by_name(dstFileName);
	srcfile->readSelf(root);
	dstfile->read_fsfile(srcfile);
	dstfile->writeSelf(root, bitmap);
}

