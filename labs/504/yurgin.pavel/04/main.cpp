#include <iostream>
#include <vector>

using namespace std;

uint64_t read_bits(const uint64_t & data, size_t begin, size_t end) {
    return (data >> begin) & ~((uint64_t)-1 << (end - begin + 1));
}

int main() {
    uint32_t log_address;
    int16_t selector;
    cin >> hex >> log_address;
    cin >> hex >> selector;

    size_t gdt_size;
    cin >> dec >> gdt_size;
    vector<uint64_t> gdt(gdt_size);
    for ( size_t i = 0; i < gdt_size; ++i)
    {
        cin >> hex >> gdt[i];
    }
    size_t ldt_size;
    cin >> dec >> ldt_size;
    vector<uint64_t> ldt(ldt_size);
    for (size_t i = 0; i < ldt_size; ++i)
    {
        cin >> hex >> ldt[i];
    }
    size_t pd_size;
    cin >> dec >> pd_size;
    vector<uint64_t> pd(pd_size);
    for (size_t i = 0; i < pd_size; ++i)
    {
        cin >> hex >> pd[i];
    }
    size_t pt_size;
    cin >> dec >> pt_size;
    vector<uint64_t> pt(pt_size);
    for (size_t i = 0; i < pt_size; ++i )
    {
        cin >> hex >> pt[i];
    }

    vector<uint64_t> dt;
    if (selector & (1 << 2))
    {
        dt = ldt;
    }
    else
    {
        dt = gdt;
    }
    size_t sd_index = selector >> 3;

    if ((dt == gdt && sd_index == 0) || sd_index >= dt.size())
    {
        cout << "INVALID" << endl;
        return -1;
    }

    uint64_t segment_d = dt[sd_index];
    if (!(segment_d & (static_cast<uint64_t >(1) << 47)))
    {
        cout << "INVALID" << endl;
        return -1;
    }
    uint64_t base = (read_bits(segment_d, 16, 39) | read_bits(segment_d, 56, 63) << 24);
    uint64_t lin_address = base + log_address;
    uint64_t offset = read_bits(lin_address, 0, 11);
    uint64_t tab_index = read_bits(lin_address, 12, 21);
    uint64_t dir_index = read_bits(lin_address, 22, 31);

    if (dir_index >= pd.size() || tab_index >= pt.size())
    {
        cout << "INVALID" << endl;
        return -1;
    }
    uint64_t pd_entry = pd[dir_index];
    uint64_t pt_entry = pt[tab_index];

    if (!(pd_entry & 1) || !(pt_entry & 1))
    {
        cout << "INVALID" << endl;
        return -1;
    }

    uint32_t page = read_bits(pt_entry, 12, 31);
    uint32_t phys_address = (page << 12) | offset;
    cout << hex << phys_address << endl;
    return 0;
}