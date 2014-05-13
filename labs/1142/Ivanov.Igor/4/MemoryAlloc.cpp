#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

struct Block
{
    Block* next;
    Block* prev;
    int size;

    bool que_free(){ return size > 0 ? true : false; }

    char free;
    Block(int _size): next(NULL), prev(NULL), size(_size), free(1) {}
};

const int HEAD_SIZE = sizeof(int);

struct Memory
{
    int mem_size;
    Memory(int _size) : mem_size(_size) {}

    const int no_blocks  = 1;
    Block* top = new Block(mem_size);

    int Alloc(int in_size)
    {
        int offset = 0;
        if(no_blocks == 0)
            return 0;

        Block *cur_block = top;
        while( cur_block != NULL) {

            if((cur_block->que_free()) && (cur_block->size  - in_size  - HEAD_SIZE >= 0)){
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

    int Free(int p)
    {
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

    vector <int> Info()
    {
        vector <int> info;
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
        info.push_back(count);
        info.push_back(size);
        info.push_back(max - HEAD_SIZE > 0 ? (max - HEAD_SIZE) : 0 );
        return info;
    }

    void Map()
    {
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
    cout << "Введите размер распределяемого блока(от 100 до 10000) N= ";
    int block_size = 0;
    cin >> block_size;
    if((block_size<100) && (block_size>10000))
    {
	cout << "Ошибка!" << endl;
	return 0;
    }
    Memory Grand(block_size);

    cout << "Вводите команды" << endl;

    string command;
    while (true)
    {
        cin >> command;
        if (command == "ALLOC")
        {
            int need_size, offset;
            cin >> need_size;
            offset = Grand.Alloc(need_size);
            offset > 0 ? cout << "+ " << offset : cout <<"-";
            cout << endl;

        }
	if(command == "FREE")
        {
                int p;
                cin >> p;
                Grand.Free(p) < 0 ? cout << "-" : cout <<"+";
                cout << endl;

        }
	if (command == "INFO")
	{
            vector <int> Info = Grand.Info();
            for(int i = 0; i != 3; ++i )
                cout << Info[i] <<" ";
                cout <<endl;
        }
	if (command == "MAP")
	{
            Grand.Map();
            cout << endl;
        }

    }
    return 0;
}
