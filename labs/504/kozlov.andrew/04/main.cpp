#include <iostream>
#include <cstdint>
#include <climits>
#include <vector>

typedef std::vector<uint64_t> table_t;

size_t read_dec() {
    size_t result;
    std::cin >> std::dec >> result;

    return result;
}

uint64_t read_hex() {
    uint64_t result;
    std::cin >> std::hex >> result;

    return result;
}

table_t* read_table() {
    size_t size = read_dec();

    table_t * result = new table_t(size);

    for (size_t i = 0; i < size; i++) {
        (*result)[i] = read_hex();
    }

    return result;
}

uint64_t get_bits(uint64_t src, size_t begin, size_t end) {
    uint64_t mask = ~(UINT64_MAX << (end - begin));

    return (src >> begin) & mask;
}

bool get_bit(uint64_t src, size_t index) {
    return get_bits(src, index, index + 1);
}

uint64_t get_limit(uint64_t segment_descriptor) {    
    uint64_t result = get_bits(segment_descriptor, 32 + 16, 32 + 20) << 16;
    result |= get_bits(segment_descriptor, 0, 16);

    bool granularity = get_bit(segment_descriptor, 32 + 23);
    if (granularity) {
        result <<= 12;
    }

    return result;
}

uint64_t get_base(uint64_t segment_descriptor) {
    uint64_t result = get_bits(segment_descriptor, 32 + 24, 32 + 32) << 24;
    result |= get_bits(segment_descriptor, 32 + 0, 32 + 8) << 16;
    result |= get_bits(segment_descriptor, 16 + 0, 16 + 16);

    return result;
}

const std::pair<bool, uint64_t> false_pair = std::make_pair(false, 0);

uint64_t logical_address, selector;
table_t *gdt, *ldt, *pd, *pt;

std::pair<bool, uint64_t> get_address() {
    table_t* table = get_bit(selector, 2) ? ldt : gdt;
    uint64_t index = get_bits(selector, 3, 16);

    if ((table == gdt && index == 0) || (index >= table->size())) {
        return false_pair;
    }

    uint64_t segment_descriptor = (*table)[index];

    bool is_present = get_bit(segment_descriptor, 32 + 15);
    if (!is_present) {
        return false_pair;
    }

    uint64_t limit = get_limit(segment_descriptor);
    if (logical_address > limit) {
        return false_pair;
    }

    uint64_t address = get_base(segment_descriptor) + logical_address;

    uint64_t directory_index = get_bits(address, 22, 32);
    if (directory_index >= pd->size() || !get_bit((*pd)[directory_index], 0)) {
        return false_pair;
    }
    
    uint64_t table_index = get_bits(address, 12, 22);
    if (table_index >= pt->size()) {
        return false_pair;
    }
    
    uint64_t table_entry = (*pt)[table_index];
    if (!get_bit(table_entry, 0)) {
        return false_pair;
    }

    uint64_t result = get_bits(table_entry, 12, 32) << 12;
    result |= get_bits(address, 0, 12);

    return std::make_pair(true, result);
}

int main() {
    logical_address = read_hex();
    selector = read_hex();

    gdt = read_table();
    ldt = read_table();
    pd = read_table();
    pt = read_table();

    auto result = get_address();
    if (result.first) {
        std::cout << std::hex << result.second;
    } else {
        std::cout << "INVALID";
    }
    std::cout << std::endl;

    return 0;
}