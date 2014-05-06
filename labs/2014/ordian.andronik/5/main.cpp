#include <iostream>
#include <vector>
#include <string>

typedef std::vector<uint64_t> Table;

void fail_if(bool b)
{
  if (b)
    throw std::string("INVALID"); 
}

Table read_table()
{
  size_t size;
  Table table;
  table.reserve(size);
  std::cin >> std::dec >> size;
  uint64_t entry;
  for (size_t i = 0; i != size; ++i)
    {
      std::cin >> std::hex >> entry;
      table.push_back(entry);
    }
  return table;
}


uint64_t linear(uint64_t logical,
		uint64_t selector,
		Table const& gdt,
		Table const& ldt)
{
  fail_if((selector >> 2) == 0);

  Table const& dt = (selector & 0x4) ? ldt : gdt;
  size_t dt_idx = selector >> 3;

  fail_if(dt_idx >= dt.size());

  uint64_t descriptor = dt[dt_idx];

  fail_if(((descriptor >> (32 + 15)) & 0x1) == 0);

  uint64_t segment_limit = ((descriptor & 0xF000000000000) >> 32) |
    (descriptor & 0xFFFF);
  if (descriptor >> (32 + 23) != 0)
    segment_limit <<= 12;

  fail_if(logical > segment_limit);

  uint64_t base = ((descriptor >> 16) & 0xFFFFFF) |
    ((descriptor >> (32 + 24)) & 0xFF000000);

  return logical + base;
}


uint64_t physical(uint64_t linear,
		  Table const &pd,
		  Table const &pt)
{
  size_t pd_idx = linear >> 22;

  fail_if(pd_idx >= pd.size());
  fail_if((pd[pd_idx] & 1) == 0);

  size_t pt_idx = (linear >> 12) & 0x3FF;

  fail_if(pt_idx >= pt.size());

  uint64_t pt_entry = pt[pt_idx];

  fail_if((pt_entry & 1) == 0);
  
  return (linear & 0xFFF) | (pt_entry & ~0xFFF);
}


int main()
{
  uint64_t logical, selector;
  std::cin >> std::hex >> logical >> selector; 
  Table gdt(read_table()),
    ldt(read_table()),
    pd(read_table()),
    pt(read_table());
  
  try
    {
      uint64_t lin = linear(logical, selector, gdt, ldt);
      uint64_t phy = physical(lin, pd, pt);
      std::cout << std::hex << phy << std::endl;
    }
  catch (std::string const& msg)
    {
      std::cout << msg << std::endl;
    }
  
  return 0;
}
