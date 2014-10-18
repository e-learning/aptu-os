#pragma once
#include <iostream>
#include <vector>
//typedef __int16 int16_t;
//typedef unsigned __int16 uint16_t;
//typedef __int32 int32_t;
//typedef unsigned __int32 uint32_t;
//typedef __int64 int64_t;
//typedef unsigned __int64 uint64_t;

using namespace std;
class AddressGetter
{
public:
	AddressGetter();
	uint32_t PhphysicalAddress();
	~AddressGetter();
private:
	void readData(vector<uint64_t> &toWrite);
	uint64_t getBitsFromRegister(uint64_t _register, int r, int l);
	uint32_t logical_address;
	uint16_t selector;
	vector<uint64_t> GDT, LDT;
	vector<uint64_t> PD, PT;

	
};

