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
    std::cout << MESSAGE;
    exit(-1);
}

uint64_t extract_segment_limit(const uint64_t segment_descriptor)
{
    uint64_t result = 0;

    result |= extract_bits_uint64(segment_descriptor, 48, 52);
    result = result << 16;
    result |= extract_bits_uint64(segment_descriptor, 0, 16);

    if (extract_bit_uint64(segment_descriptor, 55)) // G flag
    {
        result = result << 12;
    }

    return result;
}

uint64_t extract_segment_base(const uint64_t segment_descriptor)
{
    uint64_t result = 0;

    result |= extract_bits_uint64(segment_descriptor, 56, 64);
    result = result << 24;
    result |= extract_bits_uint64(segment_descriptor, 16, 40);

    return result;
}

uint64_t calc_segment_descriptor(const uint16_t segment_selector, const std::vector<uint64_t>& gdt, const std::vector<uint64_t>& ldt)
{
    if (segment_selector == 0) {
        print_and_exit();
    }

    bool table_indicator = extract_bit_uint16(segment_selector, 2);
    uint16_t position = extract_bits_uint16(segment_selector, 3, 16);

    if (table_indicator == 0 && position == 0)
    {
        print_and_exit();
    }

    const std::vector<uint64_t>& table = table_indicator ? ldt : gdt;

    if (table.size() <= position)
    {
        print_and_exit();
    }

    uint64_t seg_desc = table[position];

    if (!extract_bit_uint64(seg_desc, 47)) // P flag
    {
        print_and_exit();
    }

    return seg_desc;
}

uint64_t calc_linear_address(const uint64_t segment_descriptor, const uint32_t offset)
{
    if (offset > extract_segment_limit(segment_descriptor))
    {
        print_and_exit();
    }

    return extract_segment_base(segment_descriptor) + offset;
}

void check_page(const std::vector<uint32_t>& page_directory, const uint64_t addr)
{
    uint64_t pos = extract_bits_uint64(addr, 22, 32);

    if (pos >= page_directory.size() || !extract_bit_uint64(page_directory[pos], 0))
    {
        print_and_exit();
    }
}

uint64_t calc_page_address(const uint64_t linear_address, const std::vector<uint32_t>& page_table)
{
    uint64_t pos = extract_bits_uint64(linear_address, 12, 22);

    if (pos >= page_table.size() || !extract_bit_uint64(page_table[pos], 0))
    {
        print_and_exit();
    }

    return extract_bits_uint64(page_table[pos], 12, 32);
}

uint64_t calc_result(const uint64_t page_address, const uint64_t offset)
{
    uint64_t result = 0;

    result = (page_address << 12);
    result |= offset;

    return result;
}

int main() {
    uint32_t offset = 0;
    uint16_t segment_selector = 0;

    std::cin >> std::hex >> offset >> segment_selector;

    std::vector<uint64_t> gdt;
    std::vector<uint64_t> ldt;

    read_uint64s(gdt, std::cin);
    read_uint64s(ldt, std::cin);

    std::vector<uint32_t> page_directory;
    std::vector<uint32_t> page_table;

    read_uint32s(page_directory, std::cin);
    read_uint32s(page_table, std::cin);

    uint64_t linear_address = calc_linear_address(calc_segment_descriptor(segment_selector, gdt, ldt), offset);

    check_page(page_directory, linear_address);

    uint64_t result = calc_result(
            calc_page_address(linear_address, page_table),
            extract_bits_uint64(linear_address, 0, 12)
    );

    std::cout << std::hex << result << std::endl;

    return 0;
}