#include <iostream>
#include <vector>
#include <cstdlib>

typedef std::vector<uint64_t> Table;


void fail()
{
    std::cout << "INVALID" << std::endl;
    exit(0);
}


void check(bool ok)
{
    if (!ok)
        fail();
}


uint64_t readHex()
{
    uint64_t result;
    std::cin >> std::hex >> result;
    return result;
}


Table const readTable()
{
    size_t tableSize;
    std::cin >> std::dec >> tableSize;
    Table table(tableSize);
    for (auto &row: table) {
        row = readHex();
    }
    return table;
}


uint64_t calcLinear(uint64_t logical, uint64_t selector,
                    Table const &gdt, Table const &ldt)
{
    check((selector >> 2) != 0);
    Table const &dt = (selector & 0x4) ? ldt : gdt;

    size_t dtIndex = static_cast<size_t>(selector >> 3);
    check(dtIndex < dt.size());

    uint64_t descriptor = dt[dtIndex];

    check(((descriptor >> (32 + 15)) & 0x1) != 0);

    // check rights?

    uint64_t segmentLimit = ((descriptor & 0xF000000000000) >> 32) |
                            (descriptor & 0xFFFF);
    if (descriptor >> (32 + 23) != 0)
        segmentLimit *= 0x1000;
    check(logical <= segmentLimit);

    uint64_t base = ((descriptor >> 16) & 0xFFFFFF) |
                    ((descriptor >> (32 + 24)) & 0xFF000000);
    return logical + base;
    // return (logical << 3) + base
}


uint64_t calcPhysical(uint64_t linear, Table const &pd, Table const &pt)
{
    size_t pdIndex = static_cast<size_t>(linear >> 22);
    check(pdIndex < pd.size());

    check((pd[pdIndex] & 1) != 0);

    size_t ptIndex = static_cast<size_t>((linear >> 12) & 0x3FF);
    check(ptIndex < pd.size());

    uint64_t ptEntry = pt[ptIndex];
    check((ptEntry & 1) != 0);

    return (linear & 0xFFF) | (ptEntry & ~0xFFF);
}


int main()
{
    uint64_t logical = readHex();
    uint64_t selector = readHex();
    Table gdt = readTable();
    Table ldt = readTable();
    Table pd = readTable();
    Table pt = readTable();

    uint64_t linear = calcLinear(logical, selector, gdt, ldt);
    uint64_t physical = calcPhysical(linear, pd, pt);

    std::cout << std::hex << physical << std::endl;

    return 0;
}
