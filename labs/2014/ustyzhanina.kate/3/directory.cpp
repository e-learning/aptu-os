#include "directory.h"

//size of entry without meta data
size_t DirectoryD::get_size()
{
	size_t rez = 0;
	rez += (sizeof(size_t) + 10) * files.size();
	return rez;
}
size_t FileD::get_size()
{
	size_t rez = 0;

	for (size_t i = 0; i != data.size(); ++i)
		rez += data[i].second;

	return rez;
}


