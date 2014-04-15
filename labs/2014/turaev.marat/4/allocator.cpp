#include <iostream>
#include <string>
#include <cstdlib>
#include <map>

struct header {
  int size;
  int prev;
  int next;
};

class allocator {
public:

  allocator(int size) : mem_size(size), ptr(new char[size]) {
    set_header(0, size);
  }

  ~allocator() {
    delete[] ptr;
  }

  void print_map() {
    int b = start;
    int p = start;
    while (b != -1) {
      while (p < b) {
        header *h = get_header(p);
        print_block(h, p, 'u');
      }
      header *h = get_header(b);
      print_block(h, p, 'f');
      b = h->next;
    }
    while (p < mem_size) {
      header *h = get_header(p);
      print_block(h, p, 'u');
    }
    std::cout << std::endl;
  }

  void print_info() {
    int maxBlock = 0;
    // int allocated = mem_size - blockCount * HEADER_SIZE;
    int allocat = 0;

    for (auto i = allocated.begin(); i != allocated.end(); ++i) {
      allocat += i->second;      
    }

    int b = start;
    while (b != -1) {
      int s = get_header(b)->size;
      // allocated -= s;
      maxBlock = std::max(maxBlock, s - HEADER_SIZE - (b == start) * HEADER_SIZE);
      b = get_header(b)->next;
    }
    std::cout << blockCount << ' ' << allocat << ' ' << maxBlock << std::endl;
  }

  bool free(int p) {
    int p1 = p;
    bool f = false;
    for (auto i = allocated.begin(); i != allocated.end(); ++i) {
      if (i->first == p) {
        f = true;
        break;
      }      
    }

    if (f == false) {
      return false;
    }

    p -= HEADER_SIZE;
    if (p < 0 || p >= mem_size) {
      return false;
    }

    int r = start;
    while (r != -1) {
      if (is_intersect(p, r)) {
        return false;
      }
      header *h = get_header(r);
      if (p < r && p > h->prev) {
        break;
      }
      r = h->next;
    }

    int l;
    if (r == -1) {
      l = start;
      while (get_header(l)->next != -1) {
        l = get_header(l)->next;
      }
    } else {
      l = get_header(r)->prev;
    }

    set_prev(p, l);
    set_next(p, r);

    set_next(l, p);
    set_prev(r, p);

    --blockCount;

    merge(l, p);
    merge(p, r);

    allocated.erase(p1);
    return true;
  }

  int alloc(int const s) {
    int block = start;
    int ss = s + HEADER_SIZE;
    while (block != -1 && get_header(block)->size < ss + (block == 0) * HEADER_SIZE) {
      block = get_header(block)->next;
    }
    if (block == -1) {
      return -1;
    }

    ++blockCount;

    header *header = get_header(block);

    if (header->size < ss + (int) sizeof(int)) {
      set_prev(header->next, header->prev);
      set_next(header->prev, header->next);
      allocated[block + HEADER_SIZE] = s;
      return block + HEADER_SIZE;
    }

    int bb = block + header->size - ss;
    set_header(bb, ss);
    set_size(block, header->size - ss);
    allocated[bb + HEADER_SIZE] = s;
    return bb + HEADER_SIZE;
  }

private:
  static const int HEADER_SIZE = sizeof(header);
  std::map<int, int> allocated;
  int mem_size = 0;
  char *ptr = 0;
  int start = 0;
  int blockCount = 0;

  header *get_header(int p) {
    if (p < 0 || p >= mem_size) {
      return 0;
    }
    return (header *)(ptr + p);
  }


  void set_header(int p, int size, int prev = -1, int next = -1) {
    if (p == -1) {
      return;
    }
    *get_header(p) = header {size, prev, next};
  }

  void set_size(int block, int size) {
    if (block == -1) {
      return;
    }
    get_header(block)->size = size;
  }

  void set_prev(int block, int prev) {
    if (block == -1) {
      return;
    }
    get_header(block)->prev = prev;
  }

  void set_next(int block, int next) {
    if (block == -1) {
      return;
    }
    get_header(block)->next = next;
  }

  bool is_intersect(int b1, int b2) {
    if (b1 == -1 || b2 == -1) {
      return false;
    }
    if (b1 > b2) {
      std::swap(b1, b2);
    }
    return b1 + get_header(b1)->size > b2;
  }

  void merge(int &l, int &r) {
    if (l == -1 || r == -1) {
      return;
    }
    header *hl = get_header(l);
    header *hr = get_header(r);
    if (l + hl->size != r) {
      return;
    }
    set_next(l, hr->next);
    set_prev(hr->next, l);
    set_size(l, hl->size + hr->size);
    r = l;
    // --blockCount;
  }

  void print_block(header const *h, int &p, char symbol) {
    for (int i = 0; i < HEADER_SIZE; ++i) {
      std::cout << 'm';
      ++p;
    }
    for (int i = HEADER_SIZE; i < h->size; ++i) {
      std::cout << symbol;
      ++p;
    }
  }
};