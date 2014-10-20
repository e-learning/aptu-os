#include <iostream>
#include <fstream>
#include <vector>

const char* MESSAGE = "INVALID";

void read_uint64s(std::vector<uint64_t>& uint64s, std::istream& is)
{
    size_t size = 0;
    is >> std::dec >> size;

    for (size_t i = 0; i < size; ++i)
    {
        uint64_t current = 0;
        is >> std::hex >> current;

        uint64s.push_back(current);
    }
}

void read_uint32s(std::vector<uint32_t>& uint32s, std::istream& is)
{
    size_t size = 0;
    is >> std::dec >> size;

    for (size_t i = 0; i < size; ++i)
    {
        uint32_t current = 0;
        is >> std::hex >> current;

        uint32s.push_back(current);
    }
}

uint16_t extract_bits_uint16(const uint16_t uint16, const size_t from, const size_t to)
{
    return (uint16 >> from) & (~(UINT16_MAX << (to - from)));
}

bool extract_bit_uint16(const uint16_t uint16, const size_t position)
{
    return extract_bits_uint16(uint16, position, position + 1);
}

uint64_t extract_bits_uint64(const uint64_t uint64, const size_t from, const size_t to)
{
    return (uint64 >> from) & (~(UINT64_MAX << (to - from)));
}

bool extract_bit_uint64(const uint64_t uint64, const size_t position)
{
    return extract_bits_uint64(uint64, position, position + 1);
}

void print_and_exit()
{
    std::cout << MESSAGE << std::endl;
    exit(-1);
}

uint64_t extract_limit(const uint64_t seg_desc)
{
    uint64_t result = 0;

    result |= extract_bits_uint64(seg_desc, 48, 52);
    result = result << 16;
    result |= extract_bits_uint64(seg_desc, 0, 16);

    if (extract_bit_uint64(seg_desc, 55))
    {
        result = result << 12;
    }

    return result;
}

uint64_t extract_base(const uint64_t seg_desc)
{
    uint64_t result = 0;

    result |= extract_bits_uint64(seg_desc, 56, 64);
    result |= extract_bits_uint64(seg_desc, 16, 40);

    return result;
}

uint64_t calc_seg_desc(const uint16_t sel, const std::vector<uint64_t>& gdt, const std::vector<uint64_t>& ldt)
{
    if (sel == 0) {
        print_and_exit();
    }

    uint16_t index = extract_bits_uint16(sel, 3, 16);
    bool table_indicator = extract_bit_uint16(sel, 2);

    if (table_indicator == 0 && index == 0)
    {
        print_and_exit();
    }

    const std::vector<uint64_t>& table = table_indicator ? ldt : gdt;

    if (table.size() <= index)
    {
        print_and_exit();
    }

    uint64_t seg_desc = table[index];

    if (!extract_bit_uint64(seg_desc, 47))
    {
        print_and_exit();
    }

    return seg_desc;
}

uint64_t calc_addr(const uint64_t seg_desc, const uint32_t log_addr)
{
    if (log_addr > extract_limit(seg_desc))
    {
        print_and_exit();
    }

    return extract_base(seg_desc) + log_addr;
}

void check_pd(const std::vector<uint32_t> pd, const uint64_t addr)
{
    uint64_t pd_n = extract_bits_uint64(addr, 22, 32);

    if (pd_n >= pd.size() || !extract_bit_uint64(pd[pd_n], 0))
    {
        print_and_exit();
    }
}

uint64_t get_p_addr(const uint64_t addr, const std::vector<uint32_t> pt)
{
    uint64_t pt_n = extract_bits_uint64(addr, 12, 22);

    if (pt_n >= pt.size() || !extract_bit_uint64(pt[pt_n], 0))
    {
        print_and_exit();
    }

    return extract_bits_uint64(pt[pt_n], 12, 32);
}

uint64_t get_result(uint64_t ph_add, uint64_t offset)
{
    uint64_t result = 0;

    result = (ph_add << 12);
    result |= offset;

    return result;
}

int main() {
    uint32_t log_addr = 0;
    uint16_t sel = 0;

    std::cin >> std::hex >> log_addr >> sel;

    std::vector<uint64_t> gdt;
    std::vector<uint64_t> ldt;

    read_uint64s(gdt, std::cin);
    read_uint64s(ldt, std::cin);

    std::vector<uint32_t> pd;
    std::vector<uint32_t> pt;

    read_uint32s(pd, std::cin);
    read_uint32s(pt, std::cin);

    uint64_t addr = calc_addr(calc_seg_desc(sel, gdt, ldt), log_addr);

    check_pd(pd, addr);

    uint64_t result = get_result(
            get_p_addr(addr, pt),
            extract_bits_uint64(addr, 0, 12)
    );

    std::cout << std::hex << result << std::endl;

    return 0;
}