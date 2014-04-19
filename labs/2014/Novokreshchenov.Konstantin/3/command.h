#ifndef COMMAND_H
#define COMMAND_H

#include "structures.h"
#include <string>
#include <vector>

typedef std::vector<std::string>::iterator vecStrIt;

void LoadFS(std::string const & rootpath);
void SaveFS(Root * root, Bitmap * bitmap, Dir * rootDir);

size_t init_fileSystem (std::string const & rootpath); //checked
size_t format (std::string const & rootpath); //scanned

size_t create_dirpath (Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end);
size_t import (Root * root, Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end, std::string const & hostfile);
size_t exportFile (Root * root, Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end, std::string const & hostfile);
size_t print_ls (Dir * parentDir, vecStrIt current, vecStrIt end);
size_t delete_rm (Root * root, Bitmap * bitmap, Dir * parentDir, vecStrIt current, vecStrIt end);

size_t move_dir_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcDirName, std::vector<std::string> & vpathDst);
void move_dir_to_dir(Dir * parentSrcDir, std::string const & srcDirName,
	Dir * dstDir);
size_t move_file_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcFileName, std::vector<std::string> & vpathDst);
void move_file_to_file(Root * root, Bitmap * bitmap, Dir * parentSrcDir, std::string const & srcFileName,
	Dir * parentDstDir, std::string const & dstFileName);

size_t copy_dir_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcDirName, std::vector<std::string> & vpathDst);
void copy_dir_to_dir(Root * root, Bitmap * bitmap, Dir * parentSrcDir, std::string const & srcDirName,
	Dir * dstDir);
size_t copy_file_to_dest(Root * root, Bitmap * bitmap, Dir * rootDir,
	Dir * parentSrcDir, std::string const & srcFileName, std::vector<std::string> & vpathDst);
void copy_file_to_file(Root * root, Bitmap * bitmap, Dir * parentSrcDir, std::string const & srcFileName,
	Dir * parentDstDir, std::string const & dstFileName);



#endif // COMMAND_H
