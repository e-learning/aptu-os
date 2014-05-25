#ifndef UTILITY_H
#define UTILITY_H

#include "reader.h"
#include "writer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Dir;
typedef std::vector<std::string>::iterator vecStrIt;

bool isFileExist(std::string const & filename);
bool checkRecordExist(Dir * rootDir, vecStrIt current, vecStrIt end);
size_t get_filerest (std::fstream & fs);
size_t get_filesize_by_name (std::string const & filename);
size_t get_path(std::string const & filepath, std::vector<std::string> & vpath);
size_t get_currenttime();

bool src_isPartOf_dst (std::vector<std::string> const & vpathSrc, std::vector<std::string> const & vpathDst);
void print_error (size_t errorCode);

#endif // UTILITY_H
