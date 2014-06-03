#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <cstddef>
#include <vector>
#include <queue>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <time.h>

using namespace std;

struct DirectoryD
{
	typedef pair<string, size_t> name_blocknum;

	string name;
	time_t time;
	size_t first_block;
	//all is file
	queue <name_blocknum> files;
	vector <size_t> blocks;

	DirectoryD()
	{}
	DirectoryD(string m_name, time_t m_time, size_t m_first_block, queue <name_blocknum> m_files):
		name(m_name), time(m_time), first_block(m_first_block), files(m_files)
	{
	}

	size_t get_size();
};

struct FileD
{
	string name;
	time_t time;
	size_t first_block;
	vector <size_t> blocks;
	vector <pair<char *, size_t> > data;
	size_t data_size;

	FileD()
	{}

	FileD(string m_name, time_t m_time):
		name(m_name), time(m_time)
	{}

	size_t get_size();
};
#endif
