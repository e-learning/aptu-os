#include <iostream>
#include <fstream>
#include "mem.h"


int main()
{
    unsigned int logAddr;
    unsigned short segmentRegister;
    std::cin >> std::hex >> logAddr >> segmentRegister;

    DescriptorTable gdt(std::cin);
    DescriptorTable ldt(std::cin);
    DirTable pageDirecory(std::cin);
    PageTable pageTable(std::cin);


    DescriptorTable* currentDescriptorTable;
    if(segmentRegister & 4) {
        currentDescriptorTable = &ldt;
    } else {
        currentDescriptorTable = &gdt;
    }

    unsigned int dInd = segmentRegister >> 3;
    Descriptor* descriptor = currentDescriptorTable->getDescriptor(dInd);

    if ((currentDescriptorTable == &gdt && dInd == 0) || !descriptor || !descriptor->segmentPresent() || !descriptor->checkOffset(logAddr)) {
        std::cout << "INVALID" << std::endl;
        return 1;
    }

    unsigned int linearAddress = descriptor->getBaseAddress () + logAddr;
    if (!pageDirecory.entryPresent(linearAddress) || !pageTable.entryPresent(linearAddress)) {
        std::cout << "INVALID" << std::endl;
        return 1;
    }

    unsigned int phyAddress = (pageTable.getBasePhysicalAddress(linearAddress) << 12) + (linearAddress & 4095);
    std::cout << std::hex << phyAddress << std::endl;

    return 0;
}
