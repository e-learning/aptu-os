//#include "stdafx.h"
#include "AddressGetter.h"


AddressGetter::AddressGetter()
{
	cin >> hex >> logical_address;
	cin >> selector;
	readData(GDT);
	readData(LDT);
	readData(PD);
	readData(PT);
}

uint32_t AddressGetter::PhphysicalAddress()
{
	vector<uint64_t> table;
	bool tableFlag = selector & (uint16_t) (1 << 2);
	if (tableFlag)
	{
		table = LDT;
	}
	else 
	{
		table = GDT;
	}

	uint16_t index = selector >> 3;

	if (!tableFlag && !index)
	{
		throw exception();
	}


	uint64_t segmentDescriptor = table[index];

	if (!(segmentDescriptor & (uint64_t) (47)))
	{
		throw exception();
	}

	uint64_t base = getBitsFromRegister(segmentDescriptor, 16, 31) + (getBitsFromRegister(segmentDescriptor, 32, 39) << 16) + (getBitsFromRegister(segmentDescriptor, 56, 61) << 24);
//  cout<< hex <<base;
	uint64_t linearAddress = base + logical_address;

	uint64_t offset = getBitsFromRegister(linearAddress, 0, 11);
	uint64_t tableIndex = getBitsFromRegister(linearAddress, 12, 21);
//	cout << hex<< tableIndex
	uint64_t directoryIndex = getBitsFromRegister(linearAddress, 22, 31);

	uint64_t directoryNote = PD[directoryIndex];
	uint64_t pageNote = PT[tableIndex];

	if (!(directoryNote & 1) || !(pageNote & 1))
	{
		throw exception();
	}

	uint32_t pageFrame = getBitsFromRegister(pageNote, 12, 31);

	return (pageFrame << 12) + offset;
}

void AddressGetter::readData(vector<uint64_t> &toWrite) {
	int counter;
	uint64_t buf;
	cin >>dec >> counter;
//	cout << "Counter" << counter << endl;
	for (int i = 0; i < counter; i++) 
	{
		cin >> hex >> buf;
		toWrite.push_back(buf);
	}
}

uint64_t AddressGetter::getBitsFromRegister(uint64_t _register, int r, int l)
{
	return (_register >> r) & (~((uint64_t) (-1) << (l - r)));
}

AddressGetter::~AddressGetter()
{
}