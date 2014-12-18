#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>
using namespace std;

struct ptr
{
    int offset;
    int size;
    int pointer;
};

vector <ptr> blocks;
int N;

bool comparator(ptr a, ptr b)
{
    return a.offset < b.offset;
}

void alloc(int size)
{
    if (blocks.size() == 0)
    {
        ptr temp;
        temp.offset = 0;
        temp.size = size + sizeof(ptr);
        temp.pointer = temp.offset;
        blocks.push_back(temp);
        cout << "+ " << temp.pointer << endl;
        return;
    }
    for (unsigned int i = 0; i < blocks.size() - 1; i++)
    {
        if (blocks[i].offset + blocks[i].size + size < blocks[i+1].offset)
        {
            ptr temp;
            temp.offset = blocks[i].offset + blocks[i].size;
            temp.size = size + sizeof(ptr);
            temp.pointer = temp.offset;
            cout << "+ " << temp.pointer << endl;
            auto it = blocks.begin() + i;
            blocks.insert(it, temp);
            sort(blocks.begin(), blocks.end(), comparator);
            return;
        }
    }
    ptr temp;
    temp.offset = (blocks.end() - 1)->offset + (blocks.end() - 1)->size;
    temp.size = size + sizeof(ptr);
    temp.pointer = temp.offset;
    blocks.push_back(temp);
    cout << "+ " << temp.pointer << endl;
}

void free(int offset)
{
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        if (blocks[i].pointer == offset)
        {
            auto it = blocks.begin() + i;
            blocks.erase(it);
            cout << "+" << endl;
            return;
        }
    }
    cout << "-" << endl;
}

void info()
{
    int max = 0;
    int sum = 0;
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        sum += blocks[i].size;
        if (blocks[i].size > max)
        {
            max = blocks[i].size;
        }
    }
    cout << blocks.size() << " " << sum << " " << max << endl;
}

void map()
{
    int counter = 0;
    if (blocks.size() == 0)
    {
        for (int i = 0; i < sizeof(ptr); i++)
        {
            cout << "m";
        }
        for (int i = sizeof(ptr); i < N; i++)
        {
            cout << "f";
        }
        cout << endl;
        return;
    }
    for (unsigned int i = 0; i < blocks.size() - 1; i++)
    {
        for (unsigned int j = 0; j < sizeof(ptr); j++)
        {
            ++counter;
            cout << "m";
        }
        for (int j = sizeof(ptr); j < blocks[i].size; j++)
        {
            ++counter;
            cout << "u";
        }
        for (int j = 0; j < blocks[i+1].offset - blocks[i].offset - blocks[i].size; j++)
        {
            ++counter;
            cout << "f";
        }
    }
    for (unsigned int i = 0; i < sizeof(ptr); i++)
    {
        ++counter;
        cout << "m";
    }
    for (int i = sizeof(ptr); i < (blocks.end() - 1)->size; i++)
    {
        ++counter;
        cout << "u";
    }
    for (int i = 0; i < N - counter/N; i++)
    {
        cout << "f";
    }
    cout << endl;
}

void defragm()
{
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        if (blocks[i].offset + blocks[i].size < blocks[i + 1].offset)
        {
            blocks[i+1].offset = blocks[i].offset + blocks[i].size;
        }
    }
}



int main()
{
    cin >> N;
    string input;
    while (input != "exit")
    {
        getline(cin, input);
        if (strncmp(input.c_str(), "ALLOC", 5) == 0)
        {
            alloc(atoi(input.c_str() + 6));
        }
        else if (strncmp(input.c_str(), "FREE ", 5) == 0)
        {
            free(atoi(input.c_str() + 5));
        }
        else if (strcmp(input.c_str(), "INFO") == 0)
        {
            info();
        }
        else if (strcmp(input.c_str(), "MAP") == 0)
        {
            map();
        }
        else if (strcmp(input.c_str(), "DEFRAGM") == 0)
        {
            defragm();
        }
    }
    return 0;
}

