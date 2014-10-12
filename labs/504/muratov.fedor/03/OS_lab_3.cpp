

#include "MCB.h"
#include <iostream>
#include <string>
#include "ToyAllocator.h"
#include <algorithm>

using namespace std;

int main()
{
	//cout << "Put N" << endl;
	unsigned N;
	cin >> N;
	ToyAllocator alloc(N);
	while (true)
	{
		string answer;
		//cout << "Put command" << endl;
		cin >> answer;
		//transform(answer.begin(), answer.end(), answer.begin(), tolower);
		if (answer == "exit")
		{
			break;
		}
		if (answer == "MAP")
		{
			cout << alloc.map() << endl;
			continue;
		}
		if (answer == "INFO")
		{
			auto t = alloc.info();
			cout << get<0>(t) << " " << get<1>(t)<< " " << get<2>(t) << endl;
			continue;
		}

		if (answer == "ALLOC")
		{
			unsigned bytes;
			cin >> bytes;
			try
			{
				unsigned p = alloc.allocate(bytes);
				cout << "+ " << p << endl;
			}
			catch (bad_alloc)
			{
				cout << "-" << endl;
			}
			continue;
		}
		
		if (answer == "FREE")
		{
			unsigned pointer;
			cin >> pointer;
			if (alloc.free(pointer) == true)
			{
				cout << "+" << endl;
			}
			else
			{
				cout << "-" << endl;
			}
		}

	}
	//cout << "Good bye" << endl;
	system("pause");
	return 0;
}

