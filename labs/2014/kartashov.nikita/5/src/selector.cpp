#include "selector.h"


TABLE get_table_from_selector(ulong selector)
{
	const ulong SECOND_BIT = 1 << 2;
	if (selector & SECOND_BIT)
		return LDT;
	else
		return GDT;
}

ulong privilege_from_selector(ulong selector)
{
	return selector & nbits(2);
}

ulong index_from_selector(ulong selector)
{
	return bits(selector, 3, 15);
}
