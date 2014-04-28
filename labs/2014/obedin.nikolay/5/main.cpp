/*
 * Made according to: http://intel.ly/1mKetuz
 */
#include <cassert>
#include <climits>
#include <iostream>
    using std::cout; using std::cin;
    using std::hex; using std::dec; using std::endl;
#include <vector>
    using std::vector;


using addr = uint64_t;
using table = vector<addr>;

addr LA;
addr SEL;
table GDT, LDT, PD, PT;

void read_data()
{
    cin >> hex >> LA >> SEL;

    table *tables[] = { &GDT, &LDT, &PD, &PT };
    for (table *t: tables) {
        int size;
        cin >> dec >> size;
        t->resize(size);
        for (int i = 0; i < size; i++)
            cin >> hex >> (*t)[i];
    }
}

void fail()
{
    cout << "INVALID" << endl;
    std::exit(0);
}

addr read_bits(addr address, size_t start, size_t end)
{
    assert(start <= end);
    addr mask = ~(UINT64_MAX << (end-start+1));
    return (address >> start) & mask;
}

bool read_bit(addr address, size_t start)
{
    return (bool)read_bits(address, start, start);
}

addr get_physical_address()
{
    table &t   = read_bit(SEL, 2) ? LDT : GDT;
    addr index = read_bits(SEL, 3, 15);

    if (t == GDT && index == 0)
        fail(); // Nullptr access
    if (index >= t.size())
        fail(); // No such segment

    addr seg_desc = t[index];
    if (!read_bit(seg_desc, 32+15))
        fail(); // Segment is not present

    bool granularity = read_bit(seg_desc, 32+23);
    addr limit = (read_bits(seg_desc, 0, 15))
               | (read_bits(seg_desc, 32+16, 32+19) << 16);
    limit <<= (granularity ? 12 : 0);

    if (LA > limit)
        fail(); // Offset is off the limit

    addr base = (read_bits(seg_desc, 32+24, 32+31) << 24)
              | (read_bits(seg_desc, 32+0, 32+7) << 16)
              | (read_bits(seg_desc, 16, 31));

    addr linear_addr = base + LA;
    addr directory   = read_bits(linear_addr, 22, 31);
    addr table       = read_bits(linear_addr, 12, 21);
    addr offset      = read_bits(linear_addr, 0, 11);

    if (directory >= PD.size() || table >= PT.size())
        fail(); // No such PD or PT entries

    addr pd_entry = PD[directory];
    addr pt_entry = PT[table];

    if (!read_bit(pd_entry, 0) || !read_bit(pt_entry, 0))
        fail(); // PD or PT entry is not present

    addr page_frame = read_bits(pt_entry, 12, 31);
    return (page_frame << 12) | offset;
}

int main(int argc, const char *argv[])
{
    read_data();
    cout << hex << get_physical_address() << endl;
    return 0;
}
