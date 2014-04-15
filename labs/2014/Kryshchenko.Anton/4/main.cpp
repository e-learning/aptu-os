#include <iostream>
#include <string>
#include "memoryallocator.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;

int main()
{
    size_t n = 0;
    cin >> n;
    if (n < 100 || n > 10000) {
        cout << "Must be 100 <= N <= 10000" <<endl;
        return 0;
    }
    MemoryAllocator allocator(n);
    Block* root = new Block(n);

    while (true) {
        string command = "";
        cin >> command;
        if(command == "alloc") {
            size_t bytes = 0;
            cin >> bytes;
            if(bytes < 0) {
                cout<< "Must be positive"<<endl;
                return 0;
            }
            int result = allocator.allocate(root, bytes);
            (result < 0 ? cout << "-" : cout<< "+ "<< result) << endl;

        } else if (command == "free") {
            size_t ptr = 0;
            cin >> ptr;
            cout<< (allocator.free_block(root, ptr) ? '-' : '+') << endl;

        } else if (command == "info") {
            size_t blocks_count = 0;
            size_t total_memory_alloc = 0;
            size_t max_block_size_available = 0;
            allocator.get_info(*root,blocks_count, total_memory_alloc,
                               max_block_size_available);

            cout << blocks_count << " " << total_memory_alloc << " "
                    << max_block_size_available << endl;


        } else if (command == "map") {
            allocator.print_map(*root);
            cout<<endl;

        } else if (command.empty()) {
            break;
        }
    }

    allocator.deleteAll(root);
    return 0;
}

