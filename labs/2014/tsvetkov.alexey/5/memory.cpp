#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <map>

typedef unsigned long long ull;

struct descriptor {
  ull base, limit;
  int dpl;
};

ull bits(ull n, size_t from, size_t to) {
  size_t sz = to - from + 1;
  return (n >> from) & (((1ULL << sz) - 1ULL));
}

std::vector<descriptor> read_descriptor_table() {
  std::vector<descriptor> result;
  int n;
  std::cin >> std::dec >> n;
  for (size_t i = 0; i < n; ++i) {
    ull record;
    std::cin >> std::hex >> record;

    ull base = bits(record, 16, 39) | (bits(record, 32+24, 32+31) << 24);
    ull limit = bits(record, 0, 15) | (bits(record, 32+16, 32+19) << 16);

    bool gran = bits(record, 32+23, 32+23) > 0;

    if (gran) {
      limit <<= 12;
    }

    int dpl = bits(record, 45, 46);

    result.push_back({base, limit, dpl});
  }

  return result;
}

std::vector<ull> read_page_table() {
  std::vector<ull> result;

  int n;
  std::cin >> std::dec >> n;
  for (size_t i = 0; i < n; ++i) {
    ull next;
    std::cin >> std::hex >> next;
    result.push_back(next);
  }

  return result;
}

void assert_true(bool v) {
  if (!v) {
    throw std::exception();
  }
}

void solve() {
  ull ss, offset;
  std::cin >> std::hex >> offset >> ss;

  std::vector<descriptor> gdt = read_descriptor_table();
  std::vector<descriptor> ldt = read_descriptor_table();

  int rpl = bits(ss, 0, 1);
  int is_ldt = bits(ss, 2, 2);
  std::vector<descriptor>& dt = is_ldt ? ldt : gdt;

  size_t idx = bits(ss, 3, 15);
  assert_true(is_ldt || idx > 0);

  descriptor& descr = dt.at(idx);

  assert_true(rpl >= descr.dpl);
  assert_true(offset < descr.limit);

  ull linear_address = descr.base + offset;

  std::vector<ull> pd = read_page_table();
  std::vector<ull> pt = read_page_table();

  size_t pde_idx = bits(linear_address, 22, 31);
  assert_true(pd.at(pde_idx) & 1);

  size_t pte_idx = bits(linear_address, 12, 21);
  ull pte = pt.at(pte_idx);

  assert_true(pte & 1);

  ull page_base = bits(pte, 12, 31) << 12;
  ull page_offset = bits(linear_address, 0, 11);
  ull result = page_base + page_offset;

  assert_true(result < (1ULL << 32));

  std::cout<< std::hex << result;
}

int main() {
  try {
    solve();
  } catch (std::exception& e) {
    std::cout << "INVALID";
  }

  return 0;
}
