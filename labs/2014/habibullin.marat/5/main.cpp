#include <iostream>
#include <vector>

#include <cstdio>

using std::cin;
using std::cout;
using std::vector;
using std::endl;
using std::hex;
using std::dec;

bool DescriptorInGDT(size_t log_addr_sel) {
    const size_t mask = 4;
    if(log_addr_sel & mask) {
        return false;
    }
    return true;
}

size_t GetDescriptorIndex(size_t log_addr_sel) {
    const size_t shift = 3;
    return log_addr_sel >> shift;
}

void PrintInvalid() { cout << "INVALID" << endl; }

bool SegmentIsPresent(size_t descriptor) {
    const size_t shift = 47;
    return (descriptor >> shift) & 1;
}

size_t GetLimit(size_t descriptor) {
    size_t sl_0_15 = descriptor & 0xFFFF;
    size_t sl_16_19 = (descriptor >> 48) & 0xF;
    return (sl_16_19 << 16) + sl_0_15;
}

bool GFlagIsSet(size_t desctiptor) {
    const size_t shift = 55;
    return (desctiptor >> shift) & 1;
}

size_t GetBaseAddr(size_t descriptor) {
    const size_t shift1 = 16;
    size_t ba_0_15 = (descriptor >> shift1) & 0xFFFFFF;
    const size_t shift2 = 56;
    size_t ba_24_31 = descriptor >> shift2;
    return (ba_24_31 << 24) + ba_0_15;
}

size_t GetLinAddr(size_t descriptor, size_t log_addr_offs) {
    size_t base_addr = GetBaseAddr(descriptor);
    return base_addr + log_addr_offs;
}

bool OffsetInLimit(size_t log_addr_offs, size_t descriptor) {
    size_t seg_limit = GetLimit(descriptor);
    if(GFlagIsSet(descriptor)) {
        return log_addr_offs <= seg_limit * 4096;
    }
    return log_addr_offs <= seg_limit;
}

size_t GetPageTabEntryInd(size_t lin_addr) {
    size_t shift = 12;
    return (lin_addr >> shift) & 0x3FF;
}

size_t GetOffsInPage(size_t lin_addr) {
    return lin_addr & 0xFFF;
}

size_t GetPageBaseAddr(vector<size_t>& page_tab, size_t page_tab_entry_ind) {
    size_t page_tab_entry = page_tab[page_tab_entry_ind];
    return page_tab_entry >> 12;
}

void Translate(size_t log_addr_sel, size_t log_addr_offs, vector<size_t>& gdt, vector<size_t>& ldt, vector<size_t>& page_tab) {
    size_t descrInd = GetDescriptorIndex(log_addr_sel);
    size_t descriptor;
    if(DescriptorInGDT(log_addr_sel)) {
        if(descrInd == 0) {
            PrintInvalid();
            return;
        }
        descriptor = gdt[descrInd];
    }
    else {
        descriptor = ldt[descrInd];
    }
    if(!SegmentIsPresent(descriptor)) {
        PrintInvalid();
        return;
    }
    if(!OffsetInLimit(log_addr_offs, descriptor)) {
        PrintInvalid();
        return;
    }
    size_t lin_addr = GetLinAddr(descriptor, log_addr_offs);
    size_t page_tab_entry_ind = GetPageTabEntryInd(lin_addr);
    size_t offs_in_page = GetOffsInPage(lin_addr);
    size_t page_addr = GetPageBaseAddr(page_tab, page_tab_entry_ind);
    cout << hex << (page_addr << 12) + offs_in_page << endl;
}

int main() {

//    std::freopen("../VirtMemTranslator/test2", "r", stdin);

    size_t log_addr_offs;
    cin >> hex >> log_addr_offs;
    size_t log_addr_sel;
    cin >> hex >> log_addr_sel;
    size_t gdt_size;
    cin >> dec >> gdt_size;
    vector<size_t> gdt;
    for(size_t i = 0; i != gdt_size; ++i) {
        size_t entry;
        cin >> hex >> entry;
        gdt.push_back(entry);
    }
    size_t ldt_size;
    cin >> dec >> ldt_size;
    vector<size_t> ldt;
    for(size_t i = 0; i != ldt_size; ++i) {
        size_t entry;
        cin >> hex >> entry;
        ldt.push_back(entry);
    }
    size_t page_dir_size;
    cin >> dec >> page_dir_size;
    for(size_t i = 0; i != page_dir_size; ++i) {
        size_t entry;
        cin >> hex >> entry;
    }
    size_t page_tab_size;
    cin >> dec >> page_tab_size;
    vector<size_t> page_tab;
    for(size_t i = 0; i != page_tab_size; ++i) {
        size_t entry;
        cin >> hex >> entry;
        page_tab.push_back(entry);
    }
    Translate(log_addr_sel, log_addr_offs, gdt, ldt, page_tab);
    return 0;
}

