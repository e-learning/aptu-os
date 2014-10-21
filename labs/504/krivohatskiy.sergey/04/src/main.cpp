#include <iostream>
#include <stdint.h>

union selector
{
	uint16_t int_value;
	struct
	{
		unsigned int rpl : 2;
		unsigned int ldt : 1;
		unsigned int index : 13;
	} value;
};

union descriptor
{
	uint64_t int_value;
	struct
	{
		unsigned int seg_limit1 : 16;
		unsigned int base1 : 16;
		unsigned int base2 : 8;
		unsigned int type : 4;
		unsigned int s : 1;
		unsigned int dpl : 2;
		unsigned int p : 1;
		unsigned int seg_limit2 : 4;
		unsigned int avl : 1;
		unsigned int l : 1;
		unsigned int db : 1;
		unsigned int g : 1;
		unsigned int base3 : 8;
	} value;
};

union int32
{
	uint32_t int_value;
	uint16_t words[2];
	uint8_t bytes[4];
};

union lin_addr
{
	uint64_t int_value;
	struct
	{
		unsigned int offset : 12;
		unsigned int table : 10;
		unsigned int directory : 10;
	} value;
};

union pde
{
	uint32_t int_value;
	struct
	{
		unsigned int p : 1;
		unsigned int ignore : 11; 
		unsigned int offset : 20;
	} value;
};

int main(int, char*[])
{
	int32 addr;
	selector selector;
	std::cin >> std::hex >> addr.int_value;
	std::cin >> std::hex >> selector.int_value;
	if (!selector.value.ldt && !selector.value.index)
	{
		// null seg
		std::cout << "INVALID";
		return 0;
	}

	int ldt_rec_count, gdt_rec_count;

	std::cin >> std::dec >> gdt_rec_count;
	descriptor *gdt = new descriptor[gdt_rec_count];
	for (int i = 0; i < gdt_rec_count; ++i)
	{
		std::cin >> std::hex >> gdt[i].int_value;
	}

	std::cin >> std::dec >> ldt_rec_count;
	descriptor *ldt = new descriptor[ldt_rec_count];
	for (int i = 0; i < ldt_rec_count; ++i)
	{
		std::cin >> std::hex >> ldt[i].int_value;
	}

	descriptor used_descriptor;

	if (selector.value.ldt)
	{
		used_descriptor = ldt[selector.value.index];
	}
	else
	{
		used_descriptor = gdt[selector.value.index];
	}
	if (!used_descriptor.value.p)
	{
		// not present seg
		std::cout << "INVALID";
		delete[] ldt;
		delete[] gdt;
		return 0;
	}

    uint32_t seg_limit = used_descriptor.value.seg_limit1 + (used_descriptor.value.seg_limit2 << 16);

	bool expand_down = used_descriptor.value.s && !(used_descriptor.value.type & 1 << 3) && (used_descriptor.value.type & 1 << 2);

	if (used_descriptor.value.g)
	{
		seg_limit <<= 12;
		if (!expand_down && addr.int_value >> 12 << 12 >= seg_limit) // ignore 12 least bits
		{
			// bad offset
			std::cout << "INVALID";
			delete[] ldt;
			delete[] gdt;
			return 0;
		}
	}
	else
	{
		if (!expand_down && addr.int_value >= seg_limit)
		{
			// bad offset
			std::cout << "INVALID";
			delete[] ldt;
			delete[] gdt;
			return 0;
		}
	}

    uint32_t base = used_descriptor.value.base1 + (used_descriptor.value.base2 << 16) + (used_descriptor.value.base3 << 24);

	lin_addr lin_addr;
	lin_addr.int_value = base + addr.int_value;

	int pd_rec_count, pt_rec_count;
	std::cin >> std::dec >> pd_rec_count;
	pde *pd = new pde[pd_rec_count];
	for (int i = 0; i < pd_rec_count; ++i)
	{
		std::cin >> std::hex >> pd[i].int_value;
	}
	std::cin >> std::dec >> pt_rec_count;

	pde *pt = new pde[pt_rec_count];
	for (int i = 0; i < pt_rec_count; ++i)
	{
		std::cin >> std::hex >> pt[i].int_value;
	}

	if (!pd[lin_addr.value.directory].value.p || !pt[lin_addr.value.table].value.p)
	{
		// pde present fail
		std::cout << "INVALID";
		delete[] pd;
		delete[] pt;
		delete[] ldt;
		delete[] gdt;
		return 0;
	}

	std::cout << std::hex << (pt[lin_addr.value.table].value.offset << 12) + lin_addr.value.offset;

	delete[] pd;
	delete[] pt;
	delete[] ldt;
	delete[] gdt;
	return 0;
}
