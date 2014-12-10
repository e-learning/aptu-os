#include <iostream>

using std::cout;
using std::cin;

//using namespace std;

struct descriptor_table
{
    size_t size;
    uint64_t *table;

    ~descriptor_table()
    {
        delete[] table;
    }
};

struct page_entry
{
    size_t size;
    uint32_t *table;

    ~page_entry()
    {
        delete[] table;
    }
};

class address_translation_exception : std::exception
{
};


uint64_t read_hex_long();

uint32_t read_hex_int();

descriptor_table read_descriptor_table();

page_entry read_page_entry();

uint32_t get_linear_address(uint32_t &offset, uint32_t segment_selector, descriptor_table &gdt, descriptor_table &ldt);

uint32_t get_physical_address(uint32_t linear_address, page_entry &pde, page_entry &pdt);

int main()
{
    uint32_t offset;
    uint32_t segment_selector;

    offset = read_hex_int();
    segment_selector = read_hex_int();

    descriptor_table GDT = read_descriptor_table();
    descriptor_table LDT = read_descriptor_table();


    page_entry PDE = read_page_entry();
    page_entry PTE = read_page_entry();
    try
    {
        uint32_t linear_address = get_linear_address(offset, segment_selector, GDT, LDT);
        cout<<get_physical_address(linear_address, PDE, PTE);

    } catch (address_translation_exception)
    {
        cout << "INVALID";
    }
    return 0;
}

uint32_t get_physical_address(uint32_t linear_address, page_entry &pde, page_entry &pte)
{
    uint32_t pd_index_mask = 0xffc00000;
    uint32_t pd_index = (pd_index_mask & linear_address) >> 22;
    if ((uint32_t)pde.size < pd_index || !(pde.table[pd_index] & 0x00000001))
    {
        throw address_translation_exception();
    }
    const uint32_t pt_index_mask = 0x003ff000;
    uint32_t pt_index = (pt_index_mask & linear_address) >> 12;
    if ((uint32_t)pte.size < pt_index || !(pte.table[pt_index] & 0x00000001))
    {
        throw address_translation_exception();
    }
    uint32_t offset_mask = 0x00000fff;
    uint32_t offset = offset_mask & linear_address;

    return (pte.table[pt_index] & 0xfffff000) | offset;

}

unsigned int get_linear_address(uint32_t &offset, uint32_t segment_selector, descriptor_table &gdt, descriptor_table &ldt)
{
    uint32_t linear_address = 0;
    unsigned const int TI_mask = 1 << 2;
    unsigned const int index_mask = 0x0000fff8;
    uint32_t const TI = ((TI_mask & segment_selector) >> 2);
    uint32_t index = (index_mask & segment_selector) >> 3;
    uint64_t segment_descriptor;
    if (TI)
    {
        if ( index >= ldt.size)
        {
            throw address_translation_exception();
        }
        segment_descriptor = ldt.table[index];
    }
    else
    {
        if (index <= 0 ||  index >= (uint32_t)gdt.size)
        {
            throw address_translation_exception();
        }
        segment_descriptor = gdt.table[index];
    }
    uint64_t segment_present_mask = (uint64_t) 0x0000a00000000000;

    if(!((segment_descriptor&segment_present_mask)>>47)){
        throw address_translation_exception();
    }
    uint64_t base_address_mask1 = (uint64_t) 0x000000ffffff0000;
    uint64_t base_address_mask2 = (uint64_t) 0xff00000000000000;

    uint32_t base_address = (uint32_t) (((segment_descriptor & base_address_mask1) >> 16) | (segment_descriptor & base_address_mask2 >> 32));
    linear_address = offset + base_address;
    return linear_address;
}

descriptor_table read_descriptor_table()
{
    int size_dt;
    descriptor_table dt;
    cin >> size_dt;
    dt.size = (size_t) size_dt;
    dt.table = new uint64_t[size_dt];
    for (int i = 0; i < size_dt; ++i)
    {
        dt.table[i] = read_hex_long();
    }
    return dt;
}

page_entry read_page_entry()
{
    int size_pe;
    page_entry pe;
    cin >> size_pe;
    pe.size = (size_t) size_pe;
    pe.table = new uint32_t[size_pe];
    for (int j = 0; j < size_pe; ++j)
    {
        pe.table[j] = read_hex_int();
    }
    return pe;
}

uint32_t read_hex_int()
{
    std::string buf;
    cin >> buf;
    buf = "0x" + buf;
    return (uint32_t) (std::stoi(buf, nullptr, 16));
}

uint64_t read_hex_long()
{
    std::string buf;
    cin >> buf;
    buf = "0x" + buf;
    return std::stoul(buf, nullptr, 16);
}