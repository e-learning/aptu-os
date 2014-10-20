#include <iostream>
#include <cstdint>
#include <climits>
#include <vector>

using namespace std;

size_t rdec()
{
    size_t val;
    cin >> dec >> val;

    return val;
}

uint64_t rhex()
{
    uint64_t val;
    cin >> hex >> val;

    return val;
}

vector<uint64_t>* rtab()
{
    size_t size = rdec();

    vector<uint64_t> * val = new vector<uint64_t>(size);

    for (size_t i = 0; i < size; i++)
    {
        (*val)[i] = rhex();
    }

    return val;
}

uint64_t takebits(uint64_t src, size_t from, size_t to)
{
    uint64_t mask = ~(UINT64_MAX << (to - from));

    return (src >> from) & mask;
}

bool takeonebit(uint64_t src, size_t index)
{
    return takebits(src, index, index + 1);
}

uint64_t takelim(uint64_t descr)
{
    uint64_t val = takebits(descr, 32 + 16, 32 + 20) << 16;
    val |= takebits(descr, 0, 16);

    bool gr = takeonebit(descr, 32 + 23);
    if (gr)
    {
        val <<= 12;
    }

    return val;
}

uint64_t takebase(uint64_t descr)
{
    uint64_t val = takebits(descr, 32 + 24, 32 + 32) << 24;
    val |= takebits(descr, 32 + 0, 32 + 8) << 16;
    val |= takebits(descr, 16 + 0, 16 + 16);

    return val;
}

const pair<bool, uint64_t> pairfalse = make_pair(false, 0);

uint64_t logic_addr, selection;

vector<uint64_t> *GDT, *LDT, *PD, *PT;

pair<bool, uint64_t> takeaddr()
{
    vector<uint64_t>* table = takeonebit(selection, 2) ? LDY : GDT;

    uint64_t index = takebits(selection, 3, 16);

    if ((table == GDT && index == 0) || (index >= table->size()))
    {
        return pairfalse;
    }

    uint64_t descr = (*table)[index];

    bool exist = takeonebit(descr, 32 + 15);
    if (!exist)
    {
        return pairfalse;
    }

    uint64_t limit = takelim(descr);
    if (logic_addr > limit)
    {
        return pairfalse;
    }

    uint64_t addr = takebase(descr) + logic_addr;

    uint64_t directory_index = takebits(addr, 22, 32);
    if (directory_index >= PD->size() || !takeonebit((*PD)[directory_index], 0))
    {
        return pairfalse;
    }

    uint64_t table_index = takebits(addr, 12, 22);
    if (table_index >= PT->size())
    {
        return pairfalse;
    }

    uint64_t table_entry = (*PT)[table_index];
    if (!takeonebit(table_entry, 0))
    {
        return pairfalse;
    }

    uint64_t val = takebits(table_entry, 12, 32) << 12;
    val |= takebits(addr, 0, 12);

    return make_pair(true, val);
}

int main()
{
    const char *inv = "INVALID";

    logic_addr = rhex();
    selection = rhex();

    GDT = rtab();
    LDT = rtab();
    PD = rtab();
    PT = rtab();

    auto val = takeaddr();

    if (val.first)
    {
        cout << hex << val.second;
    }
    else
    {
        cout << inv << endl;
    }

    return 0;
}
