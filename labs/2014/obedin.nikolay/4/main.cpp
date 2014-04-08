#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

typedef uint16_t ushort;
typedef unsigned char uchar;


uchar  memory[10000];
ushort mem_size = 0;
ushort allocated_blocks = 0;
ushort allocated_bytes  = 0;


struct tag {
    static const ushort overhead = 2;

    ushort start = 0;
    ushort end   = 0;
    ushort size  = 0; // Without head and tail tags
    bool free  = false;

    tag() {}

    tag(ushort start, ushort end, ushort size, bool free)
        : start(start), end(end), size(size), free(free)
    {}

    bool read(bool from_start=true)
    {
        if (!valid())
            return false;
        uchar *word = from_start ? memory+start : memory+end-tag::overhead;
        free = !(word[0] & (1 << 7));
        size = word[1];
        ushort head = word[0] & (UCHAR_MAX ^ (1 << 7));
        size |= (head << 8);
        if (from_start)
            end = start + size + tag::overhead*2;
        else
            start = end - size - tag::overhead*2;
        return valid();
    }

    void write()
    {
        uchar *words[2] = { memory+start, memory + end - tag::overhead };
        for (uchar *word: words) {
            word[1] = (uchar) size;
            word[0] = (uchar) (size >> 8);
            word[0] |= (!free << 7);
        }
    }

    bool valid()
    { return start >= 0 && start < mem_size && end >= 0 && end <= mem_size; }
};

int allocate(ushort size)
{
    tag t;
    while(t.read() && (!t.free || t.size < size))
        t.start = t.end;

    if (!t.free || !t.valid())
        return -1;

    ushort dsize = t.size - size;
    ushort allocated_end = t.end - dsize;

    if (dsize < 2*tag::overhead+1) {
        allocated_end = t.end;
        size = t.size;
    } else {
        ushort rsize = dsize - 2*tag::overhead;
        tag r = { allocated_end, t.end, rsize, true };
        r.write();
    }

    t = { t.start, allocated_end, size, false };
    t.write();

    allocated_bytes  += size;
    allocated_blocks += 1;

    return t.start + tag::overhead;
}

int deallocate(ushort ptr)
{
    tag t;
    t.start = ptr - tag::overhead;
    if (!t.read() || t.free)
        return -1;

    ushort start = t.start;
    ushort end   = t.end;

    tag p;
    p.end = t.start - tag::overhead;
    if (p.read(true) && p.free)
        start = p.start;

    tag n;
    n.start = t.end;
    if (n.read() && n.free)
        end = n.end;

    allocated_bytes  -= t.size;
    allocated_blocks -= 1;

    ushort size = end - start - 2*tag::overhead;
    t = { start, end, size, true };
    t.write();

    return 0;
}

void print_map()
{
    tag t;
    t.read();
    while (t.valid()) {
        cout << "mm" << string(t.size, t.free ? 'f' : 'u') << "mm";
        t.start = t.end;
        t.read();
    }
    cout << endl;
}

void print_info()
{
    ushort max_alloc = 0;
    tag t;
    t.read();
    while (t.valid()) {
        if (t.free && t.size > max_alloc)
            max_alloc = t.size;
        t.start = t.end;
        t.read();
    }
    cout << allocated_blocks << " "
         << allocated_bytes << " "
         << max_alloc << endl;
}

void prepare()
{
    tag t;
    t.free = true;
    t.size = mem_size - 2*tag::overhead;
    t.end  = mem_size - 1;
    t.write();
}

int main(int argc, const char *argv[])
{
    cin >> mem_size;
    if (mem_size < 10 || mem_size > 10000) {
        cerr << "Memory out of limits. Exiting." << endl;
        return -1;
    }

    prepare();

    while (true) {
        string cmd;
        cin >> cmd;
        transform(begin(cmd), end(cmd), begin(cmd), ::tolower);

        if (cmd.compare("map") == 0) {
            print_map();
        } else if (cmd.compare("info") == 0) {
            print_info();
        } else if (cmd.compare("alloc") == 0) {
            ushort size;
            cin >> size;
            int ptr = allocate(size);
            ptr == -1 ? cout << "- " : cout << "+ " << ptr;
            cout << endl;
        } else if (cmd.compare("free") == 0) {
            ushort ptr;
            cin >> ptr;
            cout << (deallocate(ptr) == 0 ? "+" : "-") << endl;
        } else if (cmd.compare("quit") == 0) {
            break;
        }
    }

    return 0;
}
