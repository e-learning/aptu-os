#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>


using namespace std;
typedef vector <uint64_t> Table;

Table read_tabel()
{
    size_t row_count = 0;
    cin >> dec >> row_count;
    Table rez;
    for (size_t i = 0; i != row_count; ++i) {
        uint64_t record;
        cin >> hex >> record;
        rez.push_back(record);
    }
    return rez;
}

void show_tabel (Table tabel)
{
    for (size_t i = 0; i !=  tabel.size(); ++i)
        cout << hex << tabel[i] <<endl;
}

//
uint64_t get_index(uint64_t segment_selector)
{
    return (segment_selector & 0xFFF8) >> 3;
}

bool get_ti(uint64_t segment_selector) {
    return segment_selector & 0x4;
}

uint64_t get_base(uint64_t descriptor) {
    return ((descriptor >> 16) & 0xFFFF) | ((descriptor >> 32) & 0xFF) |
           ((descriptor >> (32 + 24)) & 0xFF000000);
}

uint64_t get_limits(uint64_t descriptor)
{
    uint64_t limits = (descriptor & 0xFFFF) | ((descriptor & 0xF000000000000) >> 32);
    if (descriptor >> (32+23))
        limits *= 0x1000;
    return limits;
}

uint64_t get_page_index(uint64_t linear_adress) {
    return (linear_adress >> 12) & 0x3FF;
}

uint64_t get_dir_index(uint64_t linear_adress) {
    return (linear_adress >> 22) & 0x3FF;
}


void check_invalid(bool expr)
{
    if (expr)
        throw runtime_error("INVALID");
}
uint64_t get_linear_adress(uint64_t logical_adress, uint64_t segment_selector,
                           Table const & gdt, Table const & ldt) {
    uint64_t index = get_index(segment_selector);
    bool Ti = get_ti(segment_selector);
    Table const &cur = Ti ? gdt : ldt;
    check_invalid((index == 0) && (!Ti));
    uint64_t segment_descriptor = cur[index];
    check_invalid(logical_adress > get_limits(segment_descriptor));
    return logical_adress + get_base(segment_descriptor);
}

uint64_t get_physical_adress(uint64_t logical_adress, uint64_t segment_selector,
                             Table const & gdt, Table const & ldt, Table const & pdt,
                             Table const & ptt) {

    uint64_t linear_adress = get_linear_adress(logical_adress, segment_selector, gdt, ldt);

    check_invalid((ptt[get_dir_index(linear_adress)] & 0x1) == 0);
    uint64_t pt_ind = get_page_index(linear_adress);
    check_invalid((ptt[pt_ind] & 0x1) == 0);
    return (ptt[pt_ind] & ~0xFFF) + (linear_adress & 0xFFF);
}

int main()
{
    uint64_t logical_address;
    uint64_t segment_selector;

    cin >> hex >> logical_address;
    cin >> hex >> segment_selector;

    Table gdt, ldt, pdt, ptt;

    gdt = read_tabel();
    ldt = read_tabel();
    pdt = read_tabel();
    ptt = read_tabel();

    try
    {
        cout << hex
             << get_physical_adress(logical_address, segment_selector,
                                    gdt, ldt, pdt, ptt) << endl;
    }
    catch(runtime_error const & err)
    {
        cout << err.what()<<endl;
    }
    return 0;
}

