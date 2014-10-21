#include <stdlib.h>

#include "memory.h"


unsigned short selector;

struct discriptor_table
{
	unsigned long long* records;
	unsigned records_count;
}gdt, ldt;


struct paging_table
{
	unsigned* records;
	unsigned records_count;
}pd, pt;


void init_discriptor_table(FILE* input, struct discriptor_table* dt);
void deinit_descriptor_table(struct discriptor_table* dt);
void init_paging_table(FILE* input, struct paging_table* pt);
void deinit_paging_table(struct paging_table* pt);
int logical_address_translation(unsigned address, unsigned* logical_address);
int physical_address_translation(unsigned logical_address, unsigned* physical_address);



void mem_init(FILE* input)
{
	fscanf(input, "%hx", &selector);
	init_discriptor_table(input, &gdt);
	init_discriptor_table(input, &ldt);
	init_paging_table(input, &pd);
	init_paging_table(input, &pt);
}


void mem_deinit()
{
	deinit_descriptor_table(&gdt);
	deinit_descriptor_table(&ldt);
	deinit_paging_table(&pd);
	deinit_paging_table(&pt);
}


int mem_translate_address(unsigned address, unsigned* physical_address)
{
	unsigned logical_address;
	if (logical_address_translation(address, &logical_address) == 0 &&
		physical_address_translation(logical_address, physical_address) == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


int logical_address_translation(unsigned address, unsigned* logical_address)
{
	unsigned short RPL = selector & 0x00000003;
	unsigned short TI = (selector >> 2) & 0x00000001;
	unsigned index = selector >> 3;
	struct discriptor_table* dt;
	if (TI)
	{
		dt = &ldt;
	}
	else
	{
		dt = &gdt;
		if (index == 0) return -1;
	}
	if (index >= dt->records_count)
	{
		return -1;
	}
	unsigned long long segment_discriptor = dt->records[index];
	unsigned DPL = (segment_discriptor >> 45) & 0x00000003;
	if (DPL < RPL) return -1;
	unsigned limit = 0;
	limit |= segment_discriptor & 0x0000ffff;
	limit |= ((segment_discriptor >> 48) & 0x0000000f) << 16;
	unsigned base = 0;
	base |= (segment_discriptor >> 16)& 0x00ffffff;
	base |= ((segment_discriptor >> 24) & 0x000000ff) << 24;
	unsigned short P = (segment_discriptor >> 47) & 0x00000001;
	unsigned short G = (segment_discriptor >> 23) & 0x00000001;
	if (P == 0) return -1;
	unsigned max_offset;
	if (G)
	{
		max_offset = limit * 4 * 1024 - 1;
	}
	else
	{
		max_offset = limit - 1;
	}
	if (address > max_offset) return -1;
	*logical_address = base + address;
	return 0;
}


int physical_address_translation(unsigned logical_address, unsigned* physical_address)
{
	unsigned short offset = logical_address & 0x00000fff;
	unsigned short page_index = (logical_address >> 12) & 0x000003ff;
	unsigned short dir_index = (logical_address >> 24) & 0x000003ff;
	if (dir_index >= pd.records_count) return -1;
	unsigned dir_record = pd.records[dir_index];
	unsigned short dir_P = dir_record & 0x00000001;
	if (dir_P == 0) return -1;
	if (page_index >= pt.records_count) return -1;
	unsigned page_record = pt.records[page_index];
	unsigned short page_P = page_record & 0x00000001;
	if (page_P == 0) return -1;
	*physical_address = (0xfffff000 & page_record) | offset;
	return 0;
}


void init_discriptor_table(FILE* input, struct discriptor_table* dt)
{
	unsigned i;
	fscanf(input, "%d", &(dt->records_count));
	dt->records = (unsigned long long*)malloc((dt->records_count) * sizeof(unsigned long long));
	for (i = 0; i < dt->records_count; i++)
	{
		fscanf(input, "%llx", &(dt->records[i]));
	}
}


void deinit_descriptor_table(struct discriptor_table* dt)
{
	free(dt->records);
	dt->records = NULL;
	dt->records_count = 0;
}


void init_paging_table(FILE* input, struct paging_table* pt)
{
	unsigned i;
	fscanf(input, "%d", &(pt->records_count));
	pt->records = (unsigned*)malloc((pt->records_count) * sizeof(unsigned));
	for (i = 0; i < pt->records_count; i++)
	{
		fscanf(input, "%x", &(pt->records[i]));
	}
}


void deinit_paging_table(struct paging_table* pt)
{
	free(pt->records);
	pt->records = NULL;
	pt->records_count = 0;
}