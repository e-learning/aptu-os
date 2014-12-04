#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <limits>
#include <cstdint>

using namespace std;

bitset<16> read16()
{
    uint16_t val;
    cin >> hex >> val;
    return bitset<16>(val);
}

bitset<32> read32()
{
    uint32_t val;
    cin >> hex >> val;
    return bitset<32>(val);
}

bitset<64> read64()
{
    uint64_t val;
    cin >> hex >> val;
    bitset<64> res(val >> 32);
    res <<= 32;
    res |= bitset<64>(val);
    return res;
}

vector<bitset<64> > gdt;
vector<bitset<64> > ldt;

vector<bitset<32> > pd;
vector<bitset<32> > pt;

void readDT(vector<bitset<64> >& dt)
{
    int n;
    cin >> dec >> n;
    dt = vector<bitset<64> >(n);
    for (int i = 0; i < n; ++i)
    {
        dt[i] = read64();
    }
}

void readP(vector<bitset<32> >& p)
{
    int n;
    cin >> dec >> n;
    p = vector<bitset<32> >(n);
    for (int i = 0; i < n; ++i)
    {
        p[i] = read32();
    }
}

void invalid()
{
    cout << "INVALID" << endl;
    exit(0);
}



bitset<32> logical_to_linear(bitset<32> logical, bitset<16> selector)
{
    vector<bitset<64> > &dt = selector[3] ? ldt : gdt;

    uint16_t idx = (selector >> 3).to_ulong();
    if (idx >= dt.size())
        invalid();
    if (idx == 0 && !selector[3])
        invalid();

    bitset<64> desc = dt[idx];
    if (!desc[47]) //P flag
        invalid();

    bitset<64> base;
    base |= desc >> 56;
    base <<= 8;
    base |= (desc << 24) >> 56;
    base <<= 16;
    base |= (desc << 32) >> 48;

    bitset<64> limit;
    limit |= (desc << 12) >> 60;
    limit <<= 16;
    limit |= (desc << 48) >> 48;
    if (desc[55])
    {
        uint32_t ilimit = limit.to_ulong();
        ilimit *= 4 * 1024;
        limit = bitset<64>(ilimit);
    }

    uint64_t min_offset, max_offset;
    bool expand_down = desc[44] & desc[42];
    if (expand_down)
    {
        min_offset = limit.to_ulong();
        max_offset = numeric_limits<uint32_t>::max();
    }
    else
    {
        min_offset = 0;
        max_offset = limit.to_ulong();
    }

    uint64_t offset = logical.to_ulong();
    if (desc[55])
    {
        if ((offset >> 12) << 12 > max_offset || offset < min_offset)
            invalid();
    }
    else if (offset > max_offset || offset < min_offset)
        invalid();

    uint64_t linear = base.to_ulong() + offset;
    if (linear > numeric_limits<int>::max())
        invalid();
    return bitset<32>(linear);
}


uint32_t linear_to_phys(bitset<32> linear)
{
    uint32_t dir  = (linear >> 22).to_ulong();
    if (dir >= pd.size())
        invalid();
    uint32_t page = ((linear << 10) >> 22).to_ulong();
    if (page >= pt.size())
        invalid();

    if ((pd[dir].to_ulong() & 1) == 0 || (pt[page].to_ulong() & 1) == 0)
        invalid();

    uint32_t phys = (pt[page] >> 12).to_ulong();
    phys <<= 12;
    phys |= ((linear << 20) >> 20).to_ulong();
    return phys;
}


int main()
{
    bitset<32> logical = read32();
    bitset<16> selector = read16();
    readDT(gdt);
    readDT(ldt);
    readP(pd);
    readP(pt);
    bitset<32> linear = logical_to_linear(logical, selector);
    cout << hex << linear_to_phys(linear) << endl;
    return 0;
}

