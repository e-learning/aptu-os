#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <map>

using namespace std;

typedef unsigned short offset_t;

offset_t total_size;

char * data;

struct MemBlock;
MemBlock * get_block(offset_t offset) {
    return reinterpret_cast<MemBlock *>(data + offset);
}

struct MemBlock {
    offset_t offset;
    offset_t size;
    offset_t actually_allocated;

    bool free = true;

    offset_t next_offset() {
        return offset + size;
    }
    offset_t prev_offset;

    MemBlock * next() {
        if (offset + size == total_size) {
            return 0;
        } else {
            return get_block(next_offset());
        }
    }

    MemBlock * prev() {
        if (offset == 0) {
            return 0;
        } else {
            return get_block(prev_offset);
        }
    }

    offset_t user_size();
    offset_t user_offset();
};

const offset_t service_data_size = sizeof(MemBlock);

offset_t MemBlock::user_size() {
    return size - service_data_size;
}

offset_t MemBlock::user_offset() {
    return offset + service_data_size;
}

pair<bool, offset_t> allocate(offset_t size) {
    for (auto block = get_block(0); block != 0; block = block->next()) {
        if (block->free && block->user_size() >= size) {
            
            if (block->user_size() > size + service_data_size) {
                offset_t new_block_offset = block->offset + service_data_size + size;
                
                MemBlock * new_block = get_block(new_block_offset);

                new_block->free = true;
                new_block->offset = new_block_offset;
                new_block->size = block->user_size() - size;
                new_block->prev_offset = block->offset;
                
                block->size = service_data_size + size;
            }
            
            block->free = false;
            block->actually_allocated = size;

            return make_pair(true, block->user_offset());
        }
    }

    return make_pair(false, 0);
}

bool freem(offset_t offset) {
    auto block = get_block(offset - service_data_size);
    block->free = true;

    if (block->next() != 0 && block->next()->free) {
        block->size += block->next()->size;
    }

    if (block->prev() != 0 && block->prev()->free) {
        block->prev()->size += block->size;
    }

    return true;
}

string info() {
    offset_t used_blocks = 0;
    offset_t used_size = 0;
    offset_t max_free = 0;

    for (auto block = get_block(0); block != 0; block = block->next()) {
        if (block->free) {
            max_free = max(max_free, block->user_size());
        } else {
            used_blocks++;
            used_size += block->actually_allocated;
        }
    }

    return to_string(used_blocks) + " " + to_string(used_size) + " " + to_string(max_free);
}

string memmap() {
    string result(total_size, 'f');

    for (auto block = get_block(0); block != 0; block = block->next()) {
        if (!block->free) {
            for (offset_t i = block->offset; i < block->user_offset(); i++) {
                result[i] = 'm';
            }
            for (offset_t i = block->user_offset(); i < block->user_offset() + block->actually_allocated; i++) {
                result[i] = 'u';
            }
            for (offset_t i = block->user_offset() + block->actually_allocated; i < block->offset + block->size; i++) {
                result[i] = 'm';
            }
        }
    }

    return result;
}


int main() {
    
    cin >> total_size;
    data = new char[total_size];
    auto first_block = get_block(0);
    first_block->offset = 0;
    first_block->size = total_size;
    first_block->free = true;
    
    string cmd;

    while (cin >> cmd) {
        if (cmd == "ALLOC") {
            offset_t p;
            cin >> p;

            auto res = allocate(p);

            if (!res.first) {
                cout << "-";
            } else {
                cout << "+" << " " << res.second;
            }

            cout << endl;
 
            continue;
        }
        if (cmd == "FREE") {
            offset_t p;
            cin >> p;

            cout << (freem(p) ? '+' : '-') << endl;

            continue;
        }
        if (cmd == "INFO") {
            cout << info() << endl;
        }
        if (cmd == "MAP") {
            cout << memmap() << endl;
        }
    }

    delete [] data;

    return 0;
}
