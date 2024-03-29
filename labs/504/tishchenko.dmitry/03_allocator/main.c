#include <stdio.h>
#include <stdlib.h>
#include "heap_functions.h"
#include "requests.h"

extern inline unsigned short get_block_size(const struct Heap *heap, struct Block *block, struct Block *nextBlock);

int main()
{
    unsigned short heapSize;
    struct Heap heap;
    char* requestLine;
    size_t requestSize;

    requestLine = (char*) malloc(sizeof(char)*16);
    getline(&requestLine, &requestSize, stdin);
    heapSize = atoi(requestLine);
    init(&heap, heapSize);

    while(getline(&requestLine, &requestSize, stdin)!=-1)
    {
        struct Request request;
        request = parseRequest(requestLine);
        switch(request.type)
        {
            case ALLOC:
            {
                short offset;
                offset = heap_allocate(&heap, request.parameter);
                if (offset > 0) fprintf(stdout, "+ %d\n", offset);
                else fprintf(stdout, "-\n");
            }
                break;
            case FREE:
            {
                unsigned short result;
                result = heap_free(&heap, request.parameter);
                if (!result) fprintf(stdout, "+\n");
                else fprintf(stdout, "-\n");
            }
                break;
            case INFO:
                fprintf(stdout, "%s\n", heap_info(&heap));
                break;
            case MAP:
            {
                const char *map = heap_map(&heap);
                fprintf(stdout, "%s\n", map);
                free((void*)map);
            }
                break;
            case UNKNOWN:
                fprintf(stdout, "UNKNOWN REQUEST\n");
                break;
        }
    }
    free(requestLine);
    destroy(&heap);
    return 0;
}
