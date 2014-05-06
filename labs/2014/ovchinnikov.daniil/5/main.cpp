#include <iostream>
#include <vector>

typedef std::vector<uint64_t> DT;           // Descriptor table

std::istream & operator >> (std::istream & in, DT & dt) {
    int n;
    in >> std::dec >> n;
    dt.resize(n);
    for (auto & v : dt) {
        in >> std::hex >> v;
    }
    return in;
}

int invalid() {
    std::cout << "INVALID" << std::endl;
    return 0;
}

uint64_t read_bits(const uint64_t & data, size_t start, size_t end) {
    return (data >> start) & ~((uint64_t)-1 << (end - start + 1));
}

int main() {
    uint32_t logical_address;
    uint16_t selector;

    DT gdt, ldt, pd, pt;

    std::cin >> std::hex
            >> logical_address
            >> selector
            >> gdt
            >> ldt
            >> pd
            >> pt;

    DT & dt = selector & (1 << 2) ? ldt : gdt;      // choose descriptor table
    size_t sdi = selector >> 3;                     // segment descriptor index
    if ((dt == gdt && sdi == 0) || sdi >= dt.size()) {
        return invalid();
    }

    uint64_t sd = dt[sdi];                          // segment descriptor
    if (!(sd & ((uint64_t)1 << 47))) {              // test segment present (32 + 15) bit
        return invalid();
    }

    uint64_t base = (read_bits(sd, 16, 32+7)) | (read_bits(sd, 32+24, 32+31) << 24);

    uint64_t linear_address = base + logical_address;
    uint64_t di     = read_bits(linear_address, 22, 31);    // directory index
    uint64_t ti     = read_bits(linear_address, 12, 21);    // table index
    uint64_t offset = read_bits(linear_address, 0, 11);

    if (di >= pd.size() || ti >= pt.size()) {
        return invalid();
    }

    uint64_t pd_entry = pd[di];
    uint64_t pt_entry = pt[ti];

    if (!(pd_entry & 1) || !(pt_entry & 1)) {
        return invalid();
    }

    uint32_t page_frame = read_bits(pt_entry, 12, 31);
    uint32_t physical_address = (page_frame << 12) | offset;

    std::cout << std::hex << physical_address << std::endl;
    return 0;
}

