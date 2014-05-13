#include <iostream>
#include <string>
#include <algorithm>

#include <cstddef>

typedef short msize_t;

char *MEMORY;
msize_t N;
msize_t COUNT = 0;
msize_t USER_SIZE = 0;
const msize_t EMPTY = -1;

struct BlockInfo {
  bool is_free;
  msize_t size; 
  msize_t neib;

  bool is_end() {
    return neib == EMPTY;
  }
};

const msize_t BI_SIZE = sizeof(BlockInfo);

void read_and_exec();
void alloc_m(msize_t s);
void free_m(msize_t p);
void info();
void map();

void write_info(msize_t offset, BlockInfo info);
void write_next(msize_t offset, BlockInfo info);
void write_prev(msize_t offset, BlockInfo info);

BlockInfo read_info(msize_t offset);
BlockInfo read_next(msize_t offset);
BlockInfo read_prev(msize_t offset);

msize_t user_size(msize_t block_size);

int main() {
  std::cin >> N;
  MEMORY = new char[N];
  for (msize_t i = 0; i < N; ++i) {
    MEMORY[i] = 0;
  }

  write_info(0, {true, user_size(N), EMPTY});
  write_info(sizeof(BlockInfo)+user_size(N), {true, user_size(N), EMPTY});

  while (std::cin.good()) {
    read_and_exec();
    std::cout << std::endl;
  }
  return 0;
}

void read_and_exec() {
  std::string cmd;
  msize_t s, p;
  std::cin >> cmd;
  if (cmd == "ALLOC") {
    std::cin >> s;
    alloc_m(s);
  } else if (cmd == "FREE") {
    std::cin >> p;
    free_m(p);
  } else if (cmd == "INFO") {
    info();
  } else if (cmd == "MAP") {
    map();
  } else {
    std::cerr << "unknown command: " << cmd << std::endl; 
  }
}

bool find_good(msize_t s, msize_t& offset, BlockInfo& bi) {
  offset = 0;
  bi = read_info(offset);

  while (true) {
    if (bi.is_free && bi.size >= s + 2 * BI_SIZE) {
      return true;
    }

    bi = read_info(offset + BI_SIZE + bi.size);
    if (bi.is_end()) 
      return false;

    offset = bi.neib;
    bi = read_info(offset);
  }
}

void alloc_m(msize_t s) {
  msize_t begin_offset;
  BlockInfo begin1;

  if(!find_good(s, begin_offset, begin1)) {
    std::cout << '-';
    return;
  }

  msize_t end_offset = begin_offset + BI_SIZE + begin1.size; 
  msize_t bs1 = begin1.size - s;

  write_info(begin_offset, 
             {true, user_size(bs1), begin1.neib});
  
  msize_t end1_offset = begin_offset + BI_SIZE + user_size(bs1);
  msize_t next = end1_offset + BI_SIZE;
  write_info(end1_offset, {true, user_size(bs1), next});
  
  write_info(next, {false, s, end1_offset});
  
  BlockInfo end1 = read_info(end_offset);
  end1.is_free = false;
  end1.size = s;
  write_info(end_offset, end1); 

  std::cout << '+' << next + BI_SIZE;
  
  COUNT++;
  USER_SIZE+=s;
}

void unite(msize_t o1, msize_t o2) {
  BlockInfo begin1 = read_info(o1);
  BlockInfo begin2 = read_info(o2);
  msize_t new_size = begin1.size + begin2.size + 2*BI_SIZE;

  msize_t end_offset = o2 + BI_SIZE + begin2.size;
  BlockInfo end2 = read_info(end_offset);

  write_info(o1, {true, new_size, begin1.neib});
  write_info(end_offset, {true, new_size, end2.neib});
}

void free_m(msize_t p) {
  msize_t offset = 0;
  BlockInfo bi = read_info(offset);

  while (true) {
    if (offset + BI_SIZE == p && !bi.is_free) {
      break;
    }

    msize_t end_offset = offset + BI_SIZE + bi.size;
    bi = read_info(end_offset);

    if (bi.is_end()) {
      std::cout << '-';
      return;
    }

    offset = bi.neib;
    bi = read_info(offset);
  }

  msize_t begin_offset = p - BI_SIZE;
  BlockInfo begin1 = read_info(begin_offset);
  
  msize_t end_offset = p + begin1.size;
  BlockInfo end1 = read_info(end_offset);

  if (!begin1.is_free) {
    COUNT--;
    USER_SIZE-=begin1.size;
    std::cout << '+';
  }

  begin1.is_free = true;
  end1.is_free = true;
  write_info(begin_offset, begin1);
  write_info(end_offset, end1);
  
  if (!begin1.is_end()) {
    BlockInfo end0 = read_info(begin1.neib);
    if (end0.is_free) {
      msize_t o0 = begin1.neib - end0.size - BI_SIZE;
      unite(o0, begin_offset);
      begin_offset = o0;
    }
  }

  if (!end1.is_end()) {
    BlockInfo begin2 = read_info(end1.neib);
    if (begin2.is_free) {
      unite(begin_offset, end1.neib);
    }
  }
}

void info() {
  msize_t offset = 0;
  BlockInfo bi = read_info(offset);

  msize_t max_free_size = 0;
  while (true) {
    if (bi.is_free && bi.size >= 2*BI_SIZE) {
      msize_t tt = bi.size - 2*BI_SIZE;
      max_free_size = std::max(max_free_size, tt);
    }

    msize_t end_offset = offset + BI_SIZE + bi.size;
    bi = read_info(end_offset);

    if (bi.is_end())
      break;

    offset = bi.neib;
    bi = read_info(offset);
  }
  std::cout << COUNT << " " << USER_SIZE << " " << max_free_size;
}


void print_sys_mem() {
  for (msize_t i = 0; i < (msize_t) sizeof(BlockInfo); ++i) 
    std::cout << 'm';
}

void map() {
  msize_t offset = 0;
  BlockInfo bi = read_info(offset);

  while (true) {
    print_sys_mem();
    
    for (msize_t i = 0; i < bi.size; ++i)
      std::cout << (bi.is_free?'f':'u');
    
    print_sys_mem();

    msize_t end_offset = offset + BI_SIZE + bi.size;
    bi = read_info(end_offset);

    if (bi.is_end())
      break;

    for (msize_t i = end_offset + BI_SIZE; i < bi.neib; ++i)
      std::cout << (bi.is_free?'f':'u');

    offset = bi.neib;
    bi = read_info(offset);
  }
}

void write_info(msize_t offset, BlockInfo info) {
  *(reinterpret_cast<BlockInfo*>(MEMORY+offset)) = info;
}

BlockInfo read_info(msize_t offset) {
  return *(reinterpret_cast<BlockInfo*>(MEMORY+offset));
}

msize_t user_size(msize_t block_size) {
  if (block_size < 2 * (msize_t) sizeof(BlockInfo)) {
    return 0;
  }

  return block_size - 2*sizeof(BlockInfo);
}
