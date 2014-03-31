#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include <fstream>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <ctime>

using std::ifstream;
using std::ofstream;
using std::string;

namespace utils {
std::string pathAppend(string const &path, string const &folder = "");

void write_to_block(ifstream &inputStream, string const &blockFile, size_t size);

void write_to_block(string const &blockFileIn, string const &blockFileOut, size_t size);

void read_from_block(ofstream &outputStream, string const &blockFile, size_t size);

string tts(time_t);
}

#endif /* end of include guard: UTILITIES_HPP */
