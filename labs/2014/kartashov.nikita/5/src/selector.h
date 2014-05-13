#ifndef SELECTOR_H
#define SELECTOR_H

#include "utils.h"

enum TABLE
{
	GDT,
	LDT
};

TABLE get_table_from_selector(ulong selector);
ulong privilege_from_selector(ulong selector);
ulong index_from_selector(ulong selector);

#endif /* end of include guard */
