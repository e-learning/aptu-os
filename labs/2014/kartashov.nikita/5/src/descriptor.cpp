#include "descriptor.h"


ulong dpl(ulong descriptor)
{
	return bits(descriptor, 32 + 13, 32 + 15);
}

bool granularity(ulong descriptor)
{
	const ulong GRANULARITY_BIT = 1UL << (32 + 23);
	return (descriptor & GRANULARITY_BIT) > 0;
}

ulong base(ulong descriptor)
{
	ulong high_part = bits(descriptor, 24 + 32, 32 + 32) << 24;
	ulong low_part = bits(descriptor, 16, 32 + 8);
	return high_part | low_part;
}

ulong limit(ulong descriptor)
{
	ulong low_part = descriptor & nbits(16);
	ulong high_part = bits(descriptor, 32 + 16, 32 + 20) << 16;
	ulong lim = high_part | low_part;
	bool g = granularity(descriptor);
	if (g)
		lim <<= 12; // 4K
	return lim;
}

