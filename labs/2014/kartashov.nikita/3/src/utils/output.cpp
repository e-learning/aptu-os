#include "FileSystem.h"

std::ostream& operator<<(std::ostream& os, File const& file)
{
	os << (file.isDirectory ? "dir" : "file")
		 << "\t "
		 << file.name
		 << "\t";

	if (file.isDirectory)
		os << std::endl;
	else
		os << file.sizeInBlocks
			 << " blocks"
			 << '\t' << ctime(&file.lastModified);
	return os;
}

std::ostream& operator<<(std::ostream& os, std::vector<File> const& v)
{
	os << v.size() << (v.size() == 1 ? " file" : " files") << std::endl;
	for (auto it = v.begin(); it != v.end(); ++it)
		os << *it;
	return os;
}

