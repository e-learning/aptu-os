//============================================================================
// Name        : memory.cpp
// Author      : atamas
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
using namespace std;

struct Selector{
	enum dt_type{
		GDT,
		LDT
	} DT;

	uint16_t RPL;

	uint16_t index;
};

struct Descriptor{
	uint32_t base_adress;
	uint32_t segment_limit;
	bool present;
};

struct LinearAdress{
	LinearAdress(uint32_t adress){
		page_dir = (adress & 0xFFC00000) >> 22;
		page = (adress & 0x003FF000) >> 12;
		off = adress & 0x00000FFF;
	}
	uint32_t page_dir;
	uint32_t page;
	uint32_t off;
};

std::istream & operator >> (std::istream & in, Selector & sl){
	uint16_t selector;
	in >> hex >> selector;
	sl.RPL = ( selector & 0x3);
	sl.index = ( selector & 0xFFF8 ) >> 3;
	if( (selector & 0x4) == 0){
		sl.DT = Selector::GDT;
	} else{
		sl.DT = Selector::LDT;
	}
	return in;
}

std::istream & operator >> (std::istream & in, Descriptor & ds){
	uint64_t descriptor;
	in >> hex >> descriptor;
	ds.base_adress = ((descriptor & 0xFFFF0000) >> 16) | ((descriptor & 0xFF00000000) >> 16) | ((descriptor & 0xFF00000000000000) >> 32);
	ds.segment_limit = ((descriptor & 0xFFFF)) | ((descriptor & 0xF000000000000) >> 32);
	bool granularity = (descriptor & 0x80000000000000) != 0;
	if(granularity){
		ds.segment_limit *= 0x00001000;
	}
	ds.present = (descriptor & 0x800000000000) != 0;
	return in;
}



int get_linear_adress(uint32_t logicalAdress, Selector selector, vector<Descriptor> & GDT, vector<Descriptor> & LDT){
	Descriptor descriptor;
	if(selector.DT == Selector::GDT){
		if(selector.index == 0){
			cout << "INVALID";
			exit(0);
		}
		descriptor = GDT[selector.index];
	} else{
		descriptor = LDT[selector.index];
	}

	if(!descriptor.present){
		cout << "INVALID";
		exit(0);
	}

	if(logicalAdress > descriptor.segment_limit){
		cout << "INVALID";
		exit(0);
	}

	return descriptor.base_adress + logicalAdress;
}

int get_physical_adress(LinearAdress linear_adress, vector<uint64_t> & pd, vector<uint64_t> & pt){
	if( (pd[linear_adress.page_dir] & 0x1) == 0 || ((pt[linear_adress.page] & 0x1) == 0)){
		cout << "INVALID";
		exit(0);
	}
	return (pt[linear_adress.page] & ~0xFFF) + linear_adress.off;
}

int main() {
	uint32_t logical_adress;
	cin >> hex >> logical_adress;

	Selector selector;
	cin >> selector;

	int GDT_size;
	cin >> dec >> GDT_size;
	vector<Descriptor> GDT(GDT_size);
	for( int i = 0; i < GDT_size; ++i){
		cin >> GDT[i];
	}

	int LDT_size;
	cin >> dec >> LDT_size;
	vector<Descriptor> LDT(LDT_size);
	for( int i = 0; i < LDT_size; ++i){
		cin >> LDT[i];
	}

	int pd_size;
	cin >> dec >> pd_size;
	vector<uint64_t> pd(pd_size);
	cin >> hex;
	for( int i = 0; i < pd_size; ++i){
		cin >>  pd[i];
	}

	int pt_size;
	cin >> dec >> pt_size;
	vector<uint64_t> pt(pt_size);
	cin >> hex;
	for( int i = 0; i < pt_size; ++i){
		cin >> pt[i];
	}
	uint32_t linear_adress = get_linear_adress(logical_adress, selector, GDT, LDT);
	cout << hex << get_physical_adress(linear_adress, pd, pt);
	return 0;

}
