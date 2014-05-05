#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <stdint.h>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::hex;
using std::dec;
using std::string;
using std::runtime_error;

struct NoPageException: public runtime_error{
    NoPageException(string const &msg):
        runtime_error(msg)
    {}
};

template<typename T>
void read_vector (vector<T>& v, size_t size) {
    for (;size != 0; --size) {
        T temp = 0;
        cin >> hex >> temp;
        v.push_back(temp);
    }
}

uint16_t get_index_from_selector (uint16_t const selector) {
    return (selector & 0xFFF8) >> 3;
}

bool check_ti_selector (uint16_t const selector) {
    return (selector & 0x4) != 0;
}

uint32_t get_base_address_from_descriptor (uint64_t const descriptor) {
    return ((descriptor & 0xFFFF0000) >> 16) | ((descriptor & 0xFF00000000) >> 16) | ((descriptor & 0xFF00000000000000) >> 32);
}

uint32_t get_limit_from_descriptor (uint64_t const descriptor) {
    return ((descriptor & 0xFFFF)) | ((descriptor & 0xF000000000000) >> 32);
}

bool check_g_descriptor (uint64_t const descriptor) {
    return (descriptor & 0x80000000000000) != 0;
}

bool check_p_descriptor (uint64_t const descriptor) {
    return (descriptor & 0x800000000000) != 0;
}

bool check_p_entry (uint64_t const entry) {
    return (entry & 0x1) != 0;
}

uint32_t get_page_base_from_page_table (uint64_t const page_table_entry) {
    return ((page_table_entry & 0xFFFFF000) >> 12);
}

uint32_t get_directory_index_from_linear_address (uint32_t const linear_address) {
    return ((linear_address & 0xFFC00000) >> 22);
}

uint32_t get_page_index_from_linear_address (uint32_t const linear_address) {
    return ((linear_address & 0x3FF000) >> 12);
}

uint32_t get_offset_from_linear_address (uint32_t const linear_address) {
    return (linear_address & 0xFFF);
}

uint32_t get_linear_address (uint32_t logical_address, uint16_t selector, vector<uint64_t> const & gdt, vector<uint64_t> const & ldt) {
    if (!check_ti_selector(selector) && get_index_from_selector(selector) == 0) throw NoPageException("INVALID");
    uint64_t descriptor = check_ti_selector(selector) ? ldt[get_index_from_selector(selector)] : gdt[get_index_from_selector(selector)];
    if (!check_p_descriptor(descriptor)) throw NoPageException("INVALID");
    uint32_t base_address = get_base_address_from_descriptor(descriptor);
    uint32_t limit = get_limit_from_descriptor(descriptor);
    if (check_g_descriptor(descriptor)) limit = limit * 0x1000;
    if (logical_address > limit) throw NoPageException("INVALID");
    return base_address + logical_address;
}

uint32_t get_physical_address (uint32_t linear_address, vector<uint64_t> const & pd, vector<uint64_t> const & pt) {
    if (!check_p_entry(pd[get_directory_index_from_linear_address(linear_address)])) throw NoPageException("INVALID");
    if (!check_p_entry(pt[get_page_index_from_linear_address(linear_address)])) throw NoPageException("INVALID");
    return (pt[get_page_index_from_linear_address(linear_address)] & ~0xFFF) + get_offset_from_linear_address(linear_address);
}

int main () {
    uint32_t logical_adress = 0;
    uint16_t selector = 0;

    size_t gdt_records_count = 0;
    size_t ldt_records_count = 0;
    size_t pd_records_count = 0;
    size_t pt_records_count = 0;

    vector<uint64_t> gdt(0);
    vector<uint64_t> ldt(0);
    vector<uint64_t> pd(0);
    vector<uint64_t> pt(0);

    cin >> hex >> logical_adress >> selector;

    cin >> dec >> gdt_records_count;
    read_vector(gdt, gdt_records_count);

    cin >> dec >> ldt_records_count;
    read_vector(ldt, ldt_records_count);

    cin >> dec >> pd_records_count;
    read_vector(pd, pd_records_count);

    cin >> dec >> pt_records_count;
    read_vector(pt, pt_records_count);

    try {
        uint32_t linear_address = get_linear_address(logical_adress, selector, gdt, ldt);
        uint32_t physical_address = get_physical_address(linear_address, pd, pt);
        cout << hex << physical_address << endl;
    } catch (NoPageException e) {
        cout << e.what() << endl;
    }

    return 0;
}
