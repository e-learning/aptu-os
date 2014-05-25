#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;
#include "fs.h"

int main(int args, char * argv[])
{
	try
	{
		if (args < 2)
			throw runtime_error("you should enter command line params");

		FileSystem file(argv[1]);
		file.format();
	}
	catch (runtime_error const & err)
	{
		cerr << "Fail: " << err.what() << endl;
	}

	return 0;
}
