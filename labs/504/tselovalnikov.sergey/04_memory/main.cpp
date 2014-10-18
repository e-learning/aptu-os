#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>

using namespace std;

const int _4KB = 12;

// https://staktrace.com/nuggets/index.php?id=11&replyTo=0
// selector
const int TI_BIT = 2;
const int INDEX = 3;

// descriptor
const int GRANULARITY_BIT = 55;
const int PRESENTATION_BIT = 47;

const int SEGMENTATION_LIMIT_1 = 0;
const int SEGMENTATION_LIMIT_2 = 48;

const int BASE_ADDRESS_1 = 16;
const int BASE_ADDRESS_2 = 56;

// http://wiki.osdev.org/Paging
// http://blogs.msdn.com/b/reiley/archive/2012/01/07/x86-linear-address-space-paging-revisited.aspx
// paging
const int PRESENT_BIT = 0;
const int PDE_START = 22;
const int PTE_START = 12;
const int OFFSET_START = 0;


typedef vector<uint64_t> vu;

// google предоставляет замечательные инструменты
template<typename T>
T bits(T value, size_t offset, size_t count) {
    return (value >> offset) & ~(~((T) 0) << count);
}


uint32_t segmentation(uint32_t logical, uint16_t selector, vu &GDT, vu &LDT) {

    // 2 бита - RPL, бит TI, 13 бит индекс
    uint16_t TI = bits(selector, TI_BIT, 1);

    size_t index_bit_count = 16 - 3;
    uint16_t index = bits(selector, INDEX, index_bit_count);

    if (TI == 0 && index == 0) {
        throw invalid_argument("First Descriptor in GDT is Not Used");
    }

    vu table = TI ? LDT : GDT;
    uint64_t descriptor = table[index];

    // The Segment Not Present exception occurs when trying to load a segment or gate
    // which has it's Present-bit set to 0. However when loading a stack-segment selector
    // which references a descriptor which is not present, a Stack-Segment Fault occurs.
    if (bits(descriptor, PRESENTATION_BIT, 1) == 0) {
        throw invalid_argument("Segment Not Present");
    }


    // set means the limit gets multiplied by 4K
    uint64_t granularity = bits(descriptor, GRANULARITY_BIT, 1);

    uint32_t limit = (bits(descriptor, SEGMENTATION_LIMIT_2, 4) << 16) + bits(descriptor, SEGMENTATION_LIMIT_1, 16);
    if(granularity) {
        limit <<= _4KB;
    }

    // A word at an offset greater than the (effective-limit – 1)
    if (logical > limit) {
        throw invalid_argument("General Protection Fault");
    }

    /*
    Base address
    Defines the location of byte 0 of the segment within the 4-GByte linear address space. The
    processor puts together the three base address fields to form a single 32-bit value. Segment base
    addresses should be aligned to 16-byte boundaries. Although 16-byte alignment is not required,
    this alignment allows programs to maximize performance by aligning code and data on 16-byte
    boundaries.
     */
    uint32_t base = (bits(descriptor, BASE_ADDRESS_2, 8) << 24) + (bits(descriptor, 32, 8) << 16) +
            (bits(descriptor, BASE_ADDRESS_1, 16));
    return base + logical;
}

uint32_t paging(uint32_t linear, vu &PDT, vu &PTT) {
    //Bits 31:22 identify the first paging structure, which is known as PDE.
    //Bits 21:12 identify the second paging structure, which is known as PTE.
    //Bits 11:0 are the page offset within the 4-KByte page frame.
    uint16_t dir_index = bits(linear, PDE_START, 10);
    uint16_t tbl_index = bits(linear, PTE_START, 10);
    uint16_t offset = bits(linear, OFFSET_START, 12);

    uint64_t PDE = PDT[dir_index];
    uint64_t PTE = PTT[tbl_index];

    /*
    P, or 'Present'. If the bit is set, the page is actually in physical memory at the moment.
    For example, when a page is swapped out, it is not in physical memory and therefore not 'Present'.
    If a page is called, but not present, a page fault will occur, and the OS should handle it. (See below.)
     */
    if (!bits(PDE, PRESENT_BIT, 1)) {
        throw invalid_argument("PDE: not present");
    }
    if (!bits(PTE, PRESENT_BIT, 1)) {
        throw invalid_argument("PTE: not present");
    }

    /*
    Bits 20:12 identify the third paging structure, which is the page frame.
    Bits 11:0 are the page offset within the 4-KByte page frame.
     */
    uint32_t page_frame = bits(PTE, PTE_START, 20) << _4KB;
    return page_frame + offset;
}

void read(vu &t, istream &is) {
    uint64_t len;
    is >> dec >> len;
    for (uint64_t i = 0; i < len; ++i) {
        uint64_t tmp;
        is >> hex >> tmp;
        t.push_back(tmp);
    }
}


int main() {
//    ifstream cin("test.in");

    uint32_t logicalAddr;
    uint16_t selector;
    cin >> hex >> logicalAddr >> selector;

    vu GDT;
    vu LDT;
    vu PDT;
    vu PTT;

    try {
        read(GDT, cin);
        read(LDT, cin);
        uint32_t linear = segmentation(logicalAddr, selector, GDT, LDT);

        read(PDT, cin);
        read(PTT, cin);
        uint32_t physical = paging(linear, PDT, PTT);

        cout << hex << physical << endl;
    } catch (exception e) {
        cout << "INVALID\n";
    }
    return 0;
}