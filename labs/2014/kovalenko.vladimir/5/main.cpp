#include <cassert>
#include <climits>
#include <iostream>
#include <vector>

using namespace std;


typedef uint64_t Address;
typedef vector<Address> Table;

Address _la_;
Address _sel_;
Table _gdt_, _ldt_, PD, PT;

void fail()
{
    cout << "INVALID" << endl;
    exit(1);
}

Address readBits(Address address, size_t start, size_t end)
{
    if(start > end){
			fail();
		}
    Address mask = ~(UINT64_MAX << (end-start+1));
    return (address >> start) & mask;
}

bool readBit(Address address, size_t start)
{
    return (bool) readBits(address, start, start);
}

Address getPhysicalAddress()
{
    Table &tbl = readBit(_sel_, 2) ? _ldt_ : _gdt_;
    Address index = readBits(_sel_, 3, 15);
    if ( (tbl == _gdt_ && index == 0) || index >= tbl.size())
        fail();
    Address seg_desc = tbl[index];
    if (!readBit(seg_desc, 32 + 15))
        fail();
    bool gran = readBit(seg_desc, 32 + 23);
    Address lim = (readBits(seg_desc, 0, 15))
               | (readBits(seg_desc, 32 + 16, 32 + 19) << 16);
    lim <<= (gran ? 12 : 0);
    if (_la_ > lim) fail();
    Address base = (readBits(seg_desc, 32 + 24, 32 + 31) << 24)
              | (readBits(seg_desc, 32 + 0, 32 + 7) << 16)
              | (readBits(seg_desc, 16, 31));
    Address linear = base + _la_;
    Address dir = readBits(linear, 22, 31);
    Address table = readBits(linear, 12, 21);
    Address offset = readBits(linear, 0, 11);
    if (dir >= PD.size() || table >= PT.size())
        fail();
    Address pd_entry = PD[dir];
    Address pt_entry = PT[table];
    if (!readBit(pd_entry, 0) || !readBit(pt_entry, 0))
        fail();
    Address pageFrame = readBits(pt_entry, 12, 31);
    return (pageFrame << 12) | offset;
}

int main(int argc, const char *argv[])
{
    cin >> hex >> _la_ >> _sel_;
    Table *tables[] = { &_gdt_, &_ldt_, &PD, &PT };
    for (Table *t: tables) {
        int size;
        cin >> dec >> size;
        t->resize(size);
        for (int i = 0; i < size; i++)
            cin >> hex >> (*t)[i];
    }
    cout << hex << getPhysicalAddress() << endl;
    return 0;
}
