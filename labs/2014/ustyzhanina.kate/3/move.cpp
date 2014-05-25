#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;
#include "fs.h"

int main(int args, char * argv[])
{
	try
	{
		if (args < 4)
			throw runtime_error("you should enter 3 command line params");

		FileSystem file(argv[1]);
		file.move(argv[2], argv[3]);
	}
	catch (runtime_error const & err)
	{
		cerr << "Fail: " << err.what() << endl;
	}

	return 0;
}


