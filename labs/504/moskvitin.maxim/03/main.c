#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alloc.h"


int main() {
    int size;
    scanf("%d\n", &size);
    alloc* allocator = create_allocator((size_t)size);
    char line[100];
    char command[100];
    while (fgets(line, 100, stdin) != NULL)
    {
        sscanf(line, "%s", command);
        if (!strcmp(command, "ALLOC"))
        {
            int size;
            sscanf(line, "%s %d", command, &size);
            size_t offset;
            if (allocate(allocator, (size_t)size, &offset) != -1)
            {
                printf("+ %d\n", (int)offset);
            }
            else
            {
                printf("-\n");
            }
        }
        else if (!strcmp(command, "FREE"))
        {
            int offset;
            sscanf(line, "%s %d", command, &offset);
            if (free_space(allocator, (size_t)offset) != -1)
            {
                printf("+\n");
            }
            else
            {
                printf("-\n");
            }
        }
        else if (!strcmp(command, "INFO"))
        {
            info(allocator);
        }
        else if (!strcmp(command, "MAP"))
        {
            map(allocator);
        }
        else
        {
            printf("Unknown command\n");
        }
    }
    return 0;
}