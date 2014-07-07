#include <iostream>
#include <vector>

using namespace std;

uint32_t logical_address;
uint16_t selector;
vector<uint64_t> GDT, LDT, PD, PT;


bool invalid = 0;

void readTable(vector<uint64_t> & t) {
    int count;
    uint64_t buf;
    cin >> dec >> count;
    for(int i=0; i!=count; ++i) {
        cin >> hex >> buf;
        t.push_back(buf);
    }
}

uint64_t getBits( uint64_t descriptor, int r, int l) {
    return (descriptor >> r) & ( ~( (uint64_t)(-1) << (l-r+1)) );
}

uint32_t getPhysicalAddress() {

    vector<uint64_t> DT;
    bool TI = selector & (uint16_t)(1 << 2);
    if(TI == 1) { DT  = LDT; } else { DT  = GDT; }

    uint16_t selector_index = selector >> 3;

    if(TI == 0 && selector_index == 0)
        { invalid = 1; return 0; }


    uint64_t segment_descriptor = DT[selector_index];

    if ( ( segment_descriptor & (uint64_t)(32+15) ) == 0)
        { invalid = 1; return 0; }


    uint64_t base = getBits(segment_descriptor, 16, 31)
                  + (getBits(segment_descriptor, 32+0, 32+7) << 16)
                  + (getBits(segment_descriptor, 32+24, 32+31) << 24);


    uint64_t linear_address = base + logical_address;


    ///////////////////////////////////////////////////


    uint64_t offset = getBits(linear_address, 0, 11);
    uint64_t table_index = getBits(linear_address, 12, 21);
    uint64_t directory_index = getBits(linear_address, 22, 31);

    uint64_t directory_entry  = PD[directory_index];
    uint64_t page_entry = PT[table_index];

    if ( (directory_entry & 1)==0 || (page_entry & 1)==0 )
        { invalid = 1; return 0; }

    uint32_t page_frame = getBits(page_entry, 12, 31);
    uint32_t physical_address = (page_frame << 12) + offset;

    return physical_address;
}

int main() {

    cin >> hex >> logical_address >> selector;
    readTable(GDT);
    readTable(LDT);
    readTable(PD);
    readTable(PT);

    uint32_t physical_address = getPhysicalAddress();

    if(invalid == 1)
        cout << "INVALID" << endl;
    else
        cout << hex << physical_address << endl;

    return 0;
}
