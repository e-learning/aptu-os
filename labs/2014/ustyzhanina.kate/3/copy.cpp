#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;
#include "fs.h"

int main(int args, char * argv[])
{
	try
	{
		FileSystem file(argv[1]);
		file.copy(argv[2], argv[3]);
	}
	catch (runtime_error const & err)
	{
		cerr << "Fail: " << err.what() << endl;
	}

	return 0;
}


