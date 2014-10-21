#include <iostream>
#include <vector>

using namespace std;

void readTable(vector <uint64_t> &t)
{
    size_t n;
    uint64_t temp;
    cin >> dec >> n;
    for (size_t i = 0; i < n; ++i)
    {
        cin >> hex >> temp;
        t.push_back(temp);
    }
}

uint64_t read_bits(const uint64_t & data, size_t start, size_t end)
{
    return (data >> start) & ~((uint64_t)-1 << (end - start + 1));
}

int main()
{
    uint32_t logical_address;
    uint16_t selector;

    vector <uint64_t> gdt, ldt, pd, pt;

    cin >> hex >> logical_address >> selector;
    readTable(gdt);
    readTable(ldt);
    readTable(pd);
    readTable(pt);

    vector <uint64_t> & dt = (1 << 2) & selector ? ldt : gdt;

    size_t index = selector >> 3;

    if ((dt == gdt && index == 0) || index >= dt.size())
    {
        cout << "INVALID" << endl;
        return 0;
    }

    uint64_t sd = dt[index];

    if ((((uint64_t)1 << (32 + 15) & sd)) == 0)
    {
        cout << "INVALID" << endl;
        return 0;
    }

    uint64_t base = (read_bits(sd, 16, 32 + 7)) | (read_bits(sd, 32 + 24, 32 + 31) << 24);

    uint64_t linear_address = base + logical_address;
    uint64_t directory     = read_bits(linear_address, 22, 31);
    uint64_t table     = read_bits(linear_address, 12, 21);
    uint64_t offset = read_bits(linear_address, 0, 11);

    if (directory >= pd.size() || table >= pt.size())
    {
        cout << "INVALID" << endl;
        return 0;
    }

    uint64_t pd_entry = pd[directory];
    uint64_t pt_entry = pt[table];

    if (!(pd_entry & 1) || !(pt_entry & 1))
    {
        cout << "INVALID" << endl;
        return 0;
    }

    uint32_t page_frame = read_bits(pt_entry, 12, 31);
    uint32_t physical_address = (page_frame << 12) | offset;

    cout << hex << physical_address << endl;
    return 0;
}
