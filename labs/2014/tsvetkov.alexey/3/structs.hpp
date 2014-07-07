#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <time.h>

struct FileDescriptor {
  int id;
  bool directory;
  int first_child;
  int parent_file;
  int next_file;
  int prev_file;
  int first_block;
  size_t number_of_blocks;
  char name[10];
  time_t time;
};

struct BlockDescriptor {
  int id;
  int next;
  size_t len;
};

struct Config {
  int block_size;
  int block_number;
};

template<typename T>
std::ostream & operator<<(std::ostream & out_, const std::vector<T> & v) {
  out_ << "total " << v.size() << " file(s)" << std::endl;
  for (auto it = v.begin(); it!=v.end(); ++it) {
    out_ << *it;
  }
  return out_;
}

inline std::ostream & operator <<(std::ostream & out_, const FileDescriptor & fd) {
  out_ << (fd.directory ? 'd' : 'f') << "\t '" << fd.name << "'\t";

  if  (fd.directory) {
    out_ << std::endl;
  } else {
    out_ << fd.number_of_blocks << " blocks";
    out_ << '\t' << ctime(&fd.time);
  }
  return out_;
}

#endif
