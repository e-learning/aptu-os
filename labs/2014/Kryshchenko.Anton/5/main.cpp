#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

void readTable(vector<uint64_t> & table) {

    size_t tabLen;
    uint64_t data;
    cin >> std::dec >> tabLen;
    for(size_t i = 0; i < tabLen; ++i) {
        cin >> std::hex >> data;
        table.push_back(data);
    }
}

uint64_t readBits(const uint64_t & address, size_t start, size_t count) {

    return (address >> start) & ~((uint64_t) - 1 << count);
}

int main()
{

    uint32_t logicAddress;
    uint16_t selector;
    vector<uint64_t> gdt, ldt, pdt, ptt;
    cin >> std::hex >> logicAddress >> selector;
    readTable(gdt);
    readTable(ldt);
    readTable(pdt);
    readTable(ptt);

    const vector<uint64_t> & dt = (readBits(selector, 2, 1) == 1) ? ldt : gdt;
    size_t index = readBits(selector, 3, 13);
    if((dt == gdt && index == 0) || (index >= dt.size())) {
        cout<< "INVALID" << endl;
        return 0;
    }
    uint64_t sd = dt[index];
    if(readBits(sd, 7, 1) == 0) {
        cout<< "INVALID" << endl;
        return 0;
    }

    bool granularity = readBits(sd, 55, 1) == 1;
    uint32_t limit = (readBits(sd, 55, 4) << 16) + readBits(sd, 0, 16);
    if(granularity) {
        limit = limit << 12;
    }
    if(logicAddress > limit) {
        cout<< "INVALID" << endl;
        return 0;
    }

    uint64_t base = readBits(sd, 16, 24) | (readBits(sd, 56, 8) << 24);
    uint64_t linearAddress = base + logicAddress;
    uint64_t dirIndex = readBits(linearAddress, 22, 10);
    uint64_t tableIndex = readBits(linearAddress, 12, 10);
    uint64_t offset = readBits(linearAddress, 0, 12);

    if(dirIndex >= pdt.size() || tableIndex >= ptt.size()) {
        cout<< "INVALID" << endl;
        return 0;
    }

    uint64_t pdEntry = pdt[dirIndex];
    uint64_t ptEntry = ptt[tableIndex];

    if((readBits(pdEntry, 0, 1) == 0) || (readBits(ptEntry, 0, 1) == 0)) {
        cout<< "INVALID" << endl;
        return 0;
    }

    uint32_t pageFrame = readBits(ptEntry, 12, 20) << 12;
    uint32_t physicAddress = pageFrame + offset;
    cout<< std::hex << physicAddress << endl;
    return 0;
}

