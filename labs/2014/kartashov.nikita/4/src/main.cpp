#include <iostream>
#include <string>

#include "memory_storage.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

int main()
{
	int n = 0;
	cin >> n;
	memory_storage storage(n);
	string buf;

	while (cin >> buf)
	{
		if (buf == "INFO")
		{
			memory_info inf = storage.info();
			cout << inf.allocated_blocks << " " << inf.allocated_memory << " " << inf.maximum_allocatable << endl;
			continue;
		}
		if (buf == "MAP")
		{
			cout << storage.map() << endl;
			continue;
		}
		int bufint = 0;
		cin >> bufint;
		if (buf == "ALLOC")
		{
			int value = storage.allocate(bufint);
			if (value == FAIL)
			{
				cout << "-" << endl;
			}
			else
			{
				cout << "+ " << value << endl;
			}
		}
		else
		{
			int value = storage.free(bufint);
			if (value != SUCCESS)
			{
				cout << "-" << endl;
			}
			else
			{
				cout << "+" << endl;
			}
		}
	}
	return 0;
}
