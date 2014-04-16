#include <iostream>
#include <cmath>
#include <vector>

using namespace std;
const int HEAD_SIZE = sizeof(int) * 3;

struct Block{
    Block* next;
    Block* prev;
    int size;

    bool is_free(){
        return size > 0 ? true : false;
    }

    char free;
    Block(int size_):
        next(NULL), prev(NULL), size(size_), free(1)
    {}
};

struct memory{
    int mem_size;
    memory(int size_) : mem_size(size_)
    {}

    const int no_blocks  = 1;
    Block* top = new Block(mem_size - HEAD_SIZE);

    int alloc(int in_size){
        int offset = 0;
        if(no_blocks == 0)
            return 0;

        Block *cur_block = top;
        while( cur_block != NULL) {

            if((cur_block->is_free()) && (cur_block->size  - in_size  - HEAD_SIZE >= 0)){
                Block * free_block = new Block(cur_block->size - in_size - HEAD_SIZE);

                cur_block->size = -1*(in_size);
                free_block->prev = cur_block;
                free_block->next = cur_block->next;
                if (cur_block->next != NULL)
                    cur_block->next->prev = free_block;
                cur_block->next = free_block;
                return offset += HEAD_SIZE;
            }
            offset += abs(cur_block->size) + HEAD_SIZE;
            cur_block = cur_block -> next;
        }
        return 0;
    }

    int free(int p){
        Block *cur_block = top;
        int offset = HEAD_SIZE;
        while (cur_block != NULL){
            if (offset == p){
                cur_block ->size *= -1;
                if((cur_block->next != NULL) && (cur_block->next->size >= 0)){
                    cur_block->size +=  cur_block->next->size + HEAD_SIZE;

                    cur_block->next = cur_block->next->next;
                    if(cur_block->next != NULL)
                        cur_block->next->prev = cur_block->next->prev->prev;
                }
                if((cur_block->prev != NULL) && (cur_block->prev->size >= 0)){
                    int size_m = cur_block->size + cur_block->prev->size + HEAD_SIZE;
                    cur_block = cur_block->prev;

                    cur_block->size = size_m;
                    cur_block->next = cur_block->next->next;
                    if(cur_block->next != NULL)
                        cur_block->next->prev = cur_block->next->prev->prev;
                }
                return p;
            }
            offset += abs(cur_block->size) + HEAD_SIZE;
            cur_block  = cur_block ->next;
        }
        return -1;
    }

    vector <int> info(){
        vector <int> rez;
        int size = 0, count = 0, max = 0;
        Block *cur_block = top;
        while (cur_block != NULL){
            if (cur_block -> size < 0){
                size += abs(cur_block -> size);
                count += 1;
            } else
                if (cur_block -> size > max)
                    max = cur_block->size;
            cur_block  = cur_block ->next;
        }
        rez.push_back(count);
        rez.push_back(size);
        rez.push_back(max - HEAD_SIZE > 0 ? max - HEAD_SIZE : 0 );
        return rez;
    }
    void map(){
        Block *cur_block = top;
        while (cur_block != NULL){
            for(int i = 0; i != HEAD_SIZE; ++i)
                cout << "m";
            if (cur_block -> size < 0)
                for(int i = 0; i != -1*(cur_block->size); ++i)
                    cout << "u";
            else
                for(int i = 0; i != cur_block->size; ++i)
                    cout << "f";
            cur_block  = cur_block ->next;
        }
    }
};

int main()
{
    int block_size = 0;
    cin >> block_size;
    memory mem(block_size);


    string command;
    while (true){
        cin >> command;
        if (command == "ALLOC")
        {
            int need_size, offset;
            cin >> need_size;
            offset = mem.alloc(need_size);
            offset > 0 ? cout << "+ " << offset : cout <<"-";
            cout << endl;

        }else if(command == "FREE")
        {
                int p;
                cin >> p;
                mem.free(p) < 0 ? cout << "-" : cout <<"+";
                cout << endl;

        } else if (command == "INFO"){
            vector <int> info = mem.info();
            for(int i = 0; i != 3; ++i )
                cout << info[i] <<" ";
                cout <<endl;
        } else if (command == "MAP"){
            mem.map();
            cout << endl;
        }

    }
    return 0;
}

