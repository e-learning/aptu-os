#include <iostream>
#include <algorithm>
#include <functional>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::function;

size_t n;

struct Block {
    bool free;
    size_t pointer;
    Block * next;

    Block(): free(true),pointer(0), next(0) {
    }

    ~Block() {
        delete next;
    }

    size_t data_size() const {
        return (next ? next->pointer : n) - data_pointer();
    }

    size_t data_pointer() const {
        return pointer + sizeof(Block);
    }
};

int allocate(Block * root, const size_t bytes) {
    if (!root) {
        return -1;
    }

    if (root->free && bytes <= root->data_size()) {
        root->free = false;
        if (root->data_pointer() + bytes + sizeof(Block) < n) {
            Block * b = new Block();
            b->pointer = root->pointer + sizeof(Block) + bytes;
            b->next = root->next;
            root->next = b;
        }
        return root->data_pointer();
    } else {
        return allocate(root->next, bytes);
    }
}

void merge_with_next(Block * block) {
    Block * next = block->next;
    block->next = next->next;
    delete next;
}

int free(Block * current, const size_t data_pointer) {
    if (!current) {
        return -1;
    }
    if (current->data_pointer() == data_pointer) {
        current->free = true;
        if (current->next && current->next->free) {
            merge_with_next(current);
        }
        return 0;
    } else {
        int res = free(current->next, data_pointer);
        if (res == 0 && current->free
                && current->next && current->next->free) {
            merge_with_next(current);
        }
        return res;
    }
}

void loopThrough(const Block & start, const function<void (const Block &)> action){
    action(start);
    for (const Block * current = & start; current->next; current = current->next) {
       action(*current);
    }
}

int main()
{
    cin >> n;
    if (n < 100 || 10000 < n) {
        return -1;
    }

    Block *root = new Block();

    while (cin) {
        string action;
        cin >> action;
        std::transform(action.begin(), action.end(), action.begin(), ::tolower);
        if (action == "alloc") {
            size_t bytes;
            cin >> bytes;
            int r = allocate(root, bytes);
            if (r == -1) {
                cout << "-" << endl;
            } else {
                cout << "+ " << r << endl;
            }
        } else if (action == "free") {
            size_t pointer;
            cin >> pointer;
            cout << (free(root, pointer) ? '-' : '+') << endl;
        } else if (action == "info") {
            size_t blocks_count             = 0;
            size_t total_memory_allocated   = 0;
            size_t max_block_size_available = 0;
            loopThrough(*root, [&blocks_count,&total_memory_allocated,&max_block_size_available](const Block & b) {
                if (!b.free) {
                    blocks_count++;
                    total_memory_allocated += b.data_size();
                } else if (max_block_size_available < b.data_size()) {
                    max_block_size_available = b.data_size();
                }
            });
            cout << blocks_count << " "
                 << total_memory_allocated << " "
                 << max_block_size_available << endl;
        } else if (action == "map") {
            loopThrough(*root, [](const Block & b) {
                for (size_t i = 0; i< sizeof(Block); i++) {
                    cout << 'm';
                }
                for (size_t i = 0; i < b.data_size();i++) {
                    cout << (b.free ? 'f' : 'u');
                }
            });
            cout << endl;
        }
    }
    delete root;
    return 0;
}

