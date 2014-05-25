#include <iostream>
#include <vector>
#include <fstream>

struct Descriptor {
  unsigned long long value;
  bool segmentPresent() {
    return (value >> 47) & 1;
  }

  bool checkOffset(unsigned int offset) {
    unsigned int limit = (value & 65535) | ((value >> 32) & (15 << 16));
    unsigned int G = (value >> 55) & 1;
    if (G == 1) {
        limit *= 4096;
      }
    return offset < limit;

  }

  unsigned int getBaseAddress() {
    unsigned int shift1 = 16;
    unsigned int ba_0_15 = (value >> shift1) & 0xFFFFFF;
    unsigned int shift2 = 56;
    unsigned int ba_24_31 = value >> shift2;
    return (ba_24_31 << 24) + ba_0_15;
  }

  Descriptor(unsigned long long v) : value(v)
  {}
};

struct PageEntry {
  unsigned int value;
  PageEntry(unsigned long long v) {
    value = (unsigned int)v;
  }

  bool present() {
    return value & 1;
  }

  unsigned int getBase() {
    return value >> 12;
  }
};

template <typename T>
class Table {

public:
  void parseTable(std::istream& is) {
    unsigned int size;
    is >> std::dec >> size;
    unsigned long long v;
    for (unsigned int i = 0; i < size; i++) {
      is >> std::hex >> v;
      m_table.push_back (T(v));
    }
  }

protected:
  std::vector<T> m_table;

};

class DescriptorTable : public Table<Descriptor> {

public:
  Descriptor* getDescriptor(unsigned int index) {
    if (index < m_table.size ()) {
        return &m_table[index];
      }
    return 0;
  }
};

class DirTable : public Table<PageEntry> {

public:
  bool entryPresent(unsigned int addr) {
    unsigned int dir = addr >> 22;
    if (dir < m_table.size ()) {
        return m_table[dir].present();
      }

    return false;
  }

};


class PageTable : public Table<PageEntry> {

public:
  bool entryPresent(unsigned int addr) {
    unsigned int page = (addr << 10) >> 22;
    if (page < m_table.size ()) {
        return m_table[page].present();
      }
    return false;
  }
  unsigned int getBasePhysicalAddress (unsigned int addr) {
    unsigned int page = (addr << 10) >> 22;
    return m_table[page].getBase();
  }

};

void invalid() {
  std::cout << "INVALID" << std::endl;
}

int main()
{

  unsigned int logicalAddress;
  std::cin >> std::hex >> logicalAddress;

  unsigned short segmentRegistr;
  std::cin >> std::hex >> segmentRegistr;

  DescriptorTable gdt;
  gdt.parseTable (std::cin);

  DescriptorTable ldt;
  ldt.parseTable (std::cin);

  DirTable pageDirecory;
  pageDirecory.parseTable (std::cin);

  PageTable pageTable;
  pageTable.parseTable (std::cin);

  unsigned int dIndex = segmentRegistr >> 3;
  int descriptorTable = segmentRegistr & 4;
  DescriptorTable* currentDescriptorTable = (descriptorTable) ? (&ldt) : (&gdt);
  Descriptor* descriptor = currentDescriptorTable->getDescriptor (dIndex);
  if ((currentDescriptorTable == &gdt && dIndex == 0) || !descriptor) {
      invalid ();
      return 1;
    }
  if (!descriptor->segmentPresent () || !descriptor->checkOffset(logicalAddress)) {
      invalid ();
      return 1;
    }

  unsigned int linearAddress = descriptor->getBaseAddress () + logicalAddress;
  if (! pageDirecory.entryPresent (linearAddress) || ! pageTable.entryPresent (linearAddress)) {
      invalid ();
      return 1;
    }

  unsigned int phyAddress = (pageTable.getBasePhysicalAddress(linearAddress) << 12) + (linearAddress & 4095);
  std::cout << std::hex << phyAddress << std::endl;

}
