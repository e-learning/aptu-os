#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "utils.h"

ulong dpl(ulong descriptor);
bool granularity(ulong descriptor);
ulong base(ulong descriptor);
ulong limit(ulong descriptor);

#endif /* end of include guard */
