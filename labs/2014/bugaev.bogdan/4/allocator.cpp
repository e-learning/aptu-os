#include <iostream>
#include <string>
#include <cstdlib>

int const UNDEFINED = -1;


struct Header
{
    int size;
    short prev;
    short next;
};

int const HEADER_SIZE = sizeof(Header);


int size;
char *ptr;
int start;
int blockCount;


char *convertPtr(int p)
{
    if (p < 0 || p >= size)
        return 0;
    return ptr + p;
}


Header *getHeader(int p)
{
    return (Header *) convertPtr(p);
}


void setHeader(int p, int size, short prev = UNDEFINED, short next = UNDEFINED)
{
    if (p == UNDEFINED) return;
    Header tmp = {size, prev, next};
    *getHeader(p) = tmp;
}

void setSize(int block, int size)
{
    if (block == UNDEFINED)
        return;
    getHeader(block)->size = size;
}

void setPrev(int block, int prev)
{
    if (block == UNDEFINED)
        return;
    getHeader(block)->prev = prev;
}

void setNext(int block, int next)
{
    if (block == UNDEFINED)
        return;
    getHeader(block)->next = next;
}


void destroy()
{
    size = 0;
    start = 0;
    delete ptr;
    ptr = 0;
}


bool init(int memSize)
{
    destroy();
    ptr = new (std::nothrow) char[memSize];
    if (!ptr)
        return false;
    size = memSize;
    blockCount = 0;
    start = 0;
    setHeader(start, size);
    return true;
}


int memAlloc(int const s)
{
    int block = start;
    int ss = s + HEADER_SIZE;
    while (block != UNDEFINED
           && getHeader(block)->size < ss + (block == 0) * HEADER_SIZE) {
        block = getHeader(block)->next;
    }
    if (block == UNDEFINED)
        return UNDEFINED;

    ++blockCount;

    Header *header = getHeader(block);

    if (header->size < ss + (int) sizeof(int)) {
        setPrev(header->next, header->prev);
        setNext(header->prev, header->next);
        return block + HEADER_SIZE;
    }

    int const bb = block + header->size - ss;
    setHeader(bb, ss);
    setSize(block, header->size - ss);
    return bb + HEADER_SIZE;
}


bool intersects(int b1, int b2)
{
    if (b1 == UNDEFINED || b2 == UNDEFINED)
        return false;
    if (b1 > b2) std::swap(b1, b2);
    return b1 + getHeader(b1)->size > b2;
}


void tryToMerge(int &l, int &r)
{
    if (l == UNDEFINED || r == UNDEFINED)
        return;
    Header *hl = getHeader(l);
    Header *hr = getHeader(r);
    if (l + hl->size != r)
        return;
    setNext(l, hr->next);
    setPrev(hr->next, l);
    setSize(l, hl->size + hr->size);
    r = l;
    --blockCount;
}


bool memFree(int p)
{
    p -= HEADER_SIZE;
    if (p < 0 || p >= size)
        return false;

    int r = start;
    while (r != UNDEFINED) {
        if (intersects(p, r))
            return false;
        Header *h = getHeader(r);
        if (p < r && p > h->prev)
            break;
        r = h->next;
    }

    int l;
    if (r == UNDEFINED) {
        l = start;
        while (getHeader(l)->next != UNDEFINED) {
            l = getHeader(l)->next;
        }
    } else {
        l = getHeader(r)->prev;
    }

    setPrev(p, l);
    setNext(p, r);

    setNext(l, p);
    setPrev(r, p);

    --blockCount;

    tryToMerge(l, p);
    tryToMerge(p, r);

    return true;
}


void printStats()
{
    int maxBlock = 0;
    int allocated = size - blockCount * HEADER_SIZE;
    int b = start;
    while (b != UNDEFINED) {
        int s = getHeader(b)->size;
        allocated -= s;
        maxBlock = std::max(maxBlock, s - HEADER_SIZE - (b == start) * HEADER_SIZE);
        b = getHeader(b)->next;
    }
    std::cout << blockCount << ' '
              << allocated << ' '
              << maxBlock << std::endl;
}


void printBlock(Header const *h, int &p, char symbol)
{
    for (int i = 0; i < HEADER_SIZE; ++i) {
        std::cout << 'm';
        ++p;
    }
    for (int i = HEADER_SIZE; i < h->size; ++i) {
        std::cout << symbol;
        ++p;
    }
}


void printMap()
{
    int b = start;
    int p = start;
    while (b != UNDEFINED) {
        while (p < b) {
            Header *h = getHeader(p);
            printBlock(h, p, 'u');
        }
        Header *h = getHeader(b);
        printBlock(h, p, 'f');
        b = h->next;
    }
    while (p < size) {
        Header *h = getHeader(p);
        printBlock(h, p, 'u');
    }
    std::cout << std::endl;
}


int main()
{
    int memSize;
    std::cin >> memSize;

    if (!init(memSize)) {
        std::cerr << "Error" << std::endl;
        return EXIT_FAILURE;
    }

    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "ALLOC") {
            int s;
            std::cin >> s;
            int p = memAlloc(s);
            if (p != UNDEFINED)
                std::cout << "+ " << p << std::endl;
            else
                std::cout << "-" << std::endl;
        } else if (cmd == "FREE") {
            int p;
            std::cin >> p;
            std::cout << (memFree(p) ? "+" : "-") << std::endl;
        } else if (cmd == "INFO") {
            printStats();
        } else if (cmd == "MAP") {
            printMap();
        } else {
            std::cout << "Illegal command" << std::endl;
        }
    }

    destroy();

    return 0;
}
