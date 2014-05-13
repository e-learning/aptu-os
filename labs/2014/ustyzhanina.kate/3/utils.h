#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>

struct Utils
{
	std::string path_append(std::string const & path, std::string const & folder)
	{
		std::string resultPath(path);

		if (resultPath[resultPath.length()] != '/')
			resultPath += '/';

		return resultPath + folder;
	}

	vector<string> split(const string & str, const char delimiter)
	{
		stringstream test(str);
		string segment;
		vector<string> seglist;

		while(getline(test, segment, delimiter))
		{
			seglist.push_back(segment);
		}

		return seglist;
	}

	string read_string(ifstream & in)
	{
		char * buffer = new char[10] {};
		in.read(buffer, 10);
		return string(buffer);
	}

	size_t write_string(ofstream & s, string const & str)
	{
		char * buffer = new char[10] {};
		std::copy(str.begin(), str.end(), buffer);
		s.write(buffer, 10);
		return 10;
	}

	size_t get_file_size(const char * fileName)
	{
		struct stat file_stat;
		stat(fileName, &file_stat);
		return file_stat.st_size;
	}

	string time_to_string(time_t time)
	{
		char buffer[128];
		strftime(buffer, 128, "%D %T", localtime(&time));
		return string(buffer);
	}

};

#endif /* end of include guard: UTILITIES_HPP */
