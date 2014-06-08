#ifndef MEM_H
#define MEM_H

#include <iostream>
#include <vector>
#include <fstream>

struct Descriptor {
    Descriptor(unsigned long long v) : value(v) {}

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
        unsigned int baseAddr0to15 = (value >> shift1) & 0xFFFFFF;
        unsigned int shift2 = 56;
        unsigned int baseAddr24to31 = value >> shift2;
        return (baseAddr24to31 << 24) + baseAddr0to15;
    }

    unsigned long long value;
};


struct PageEntry {
    PageEntry(unsigned long long v) {
        value = (unsigned int)v;
    }

    bool present() {
        return value & 1;
    }

    unsigned int getBase() {
        return value >> 12;
    }

    unsigned int value;
};

template <typename T> struct Table {
    Table(std::istream &input) {
        unsigned int size;
        input >> std::dec >> size;
        unsigned long long v;
        for (unsigned int i = 0; i < size; i++) {
            input >> std::hex >> v;
            memTable.push_back (T(v));
        }
    }
protected:
    std::vector<T> memTable;
};


struct DescriptorTable : Table<Descriptor> {
    DescriptorTable(std::istream &input) :Table<Descriptor>(input) {}

    Descriptor* getDescriptor(unsigned int index) {
        if (index < memTable.size ()) {
            return &memTable[index];
        }
        return 0;
    }
};

struct DirTable : Table<PageEntry> {
    DirTable(std::istream &input) : Table<PageEntry>(input) {}

    bool entryPresent(unsigned int addr) {
        unsigned int dir = addr >> 22;
        if (dir < memTable.size ()) {
            return memTable[dir].present();
        }
        return 0;
    }
};


struct PageTable : Table<PageEntry> {
    PageTable(std::istream &input) : Table<PageEntry>(input) {}

    bool entryPresent(unsigned int addr) {
        unsigned int page = (addr << 10) >> 22;
        if (page < memTable.size ()) {
            return memTable[page].present();
        }
        return 0;
    }

    unsigned int getBasePhysicalAddress (unsigned int addr) {
        unsigned int page = (addr << 10) >> 22;
        return memTable[page].getBase();
    }
};

#endif