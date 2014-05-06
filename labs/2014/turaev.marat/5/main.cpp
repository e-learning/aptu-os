#include <iostream>
#include <vector>

typedef std::vector<uint64_t> table;

table read_table() {
  table table;
  int n;
  std::cin >> std::dec >> n;
  uint64_t value;
  for (int i = 0; i < n; ++i) {
    std::cin >> std::hex >> value;
    table.push_back(value);
  }
  return table;
}

template <typename T>
bool exceed_size(size_t id, std::vector<T> vec) {
  return id >= vec.size();
}

void print_invalid() {
  std::cout << "INVALID" << std::endl;
}

uint64_t cut(uint64_t data, size_t from, size_t to) {
  return (data >> from) & ~((uint64_t) - 1 << (to - from + 1));
}

int main() {
  uint32_t logical_address;
  uint16_t selector;
  std::cin >> std::hex >> logical_address >> selector;
  table gdt = read_table();
  table ldt = read_table();
  table page_directory = read_table();
  table page_table = read_table();
  table &table = selector & (1 << 2) ? ldt : gdt;
  size_t segment_descriptor_index = selector >> 3;
  if ((table == gdt && segment_descriptor_index == 0) || exceed_size(segment_descriptor_index, table)) {
    print_invalid();
    return 0;
  }

  uint64_t segment_descriptor = table[segment_descriptor_index];
  if (!(segment_descriptor & ((uint64_t)1 << 47))) {
    print_invalid();
    return 0;
  }

  uint64_t base = cut(segment_descriptor, 16, 39) | (cut(segment_descriptor, 56, 63) << 24);
  uint64_t linear_address = base + logical_address;
  uint64_t directory_index = cut(linear_address, 22, 31);
  uint64_t table_intex = cut(linear_address, 12, 21);
  uint64_t offset = cut(linear_address, 0, 11);

  if (exceed_size(directory_index, page_directory) || exceed_size(table_intex, page_table)) {
    print_invalid();
    return 0;
  }

  uint64_t pd_entry = page_directory[directory_index];
  uint64_t pt_entry = page_table[table_intex];

  if (!(pd_entry & 1) || !(pt_entry & 1)) {
    print_invalid();
    return 0;
  }

  uint32_t page_frame = cut(pt_entry, 12, 31);
  uint32_t physical_address = (page_frame << 12) | offset;

  std::cout << std::hex << physical_address << std::endl;
  return 0;
}

