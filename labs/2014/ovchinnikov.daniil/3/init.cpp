#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

int main (const int argc, const char * args[]) {
	if (argc < 2){
		cout << "Enter root location" << endl;
		return 1;
	} else {
		string root = args[1];
		string config = root + "/config";
		ifstream config_file(config.c_str());
		if (config_file.fail()) {
			cout << "Config file cannot be opened." << endl;
			return 2;
		} else {
			int block_size, block_number;
			config_file >> block_size >> block_number;
			cout << "Initializing " << block_number << " blocks of " << block_size << " bytes" << endl;
			for (int i = 0; i < block_number; ++i) {
				string block_name = root + "/" + to_string(i);
				ofstream(block_name.c_str()).close();
				truncate(block_name.c_str(), block_size);
			}
		}
	}

    return 0;
}
