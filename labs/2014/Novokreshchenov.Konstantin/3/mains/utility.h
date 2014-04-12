#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


size_t get_filerest (std::fstream & fs);
size_t get_filesize_by_name (std::string const & filename);
size_t get_path(std::string const & filepath, std::vector<std::string> & vpath);
size_t get_currenttime();

bool src_isPartOf_dst (std::vector<std::string> const & vpathSrc, std::vector<std::string> const & vpathDst);
void print_error (size_t errorCode);

#endif // UTILITY_H
