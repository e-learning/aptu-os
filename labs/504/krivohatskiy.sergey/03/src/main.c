#include <stdio.h>
#include <malloc.h>
#include "allocator.h"
#include <string.h>
#include <stdlib.h>

char *my_getline(int *was_eof) {
    *was_eof = 0;
    char * line = malloc(100), *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    for (;;) {
        c = fgetc(stdin);
        if (c == EOF)
        {
            *was_eof = 1;
            break;
        }

        if (--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if (linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if (c == '\n')
            break;
        (*line++ = c);
    }
    *line = '\0';
    return linep;
}

int main(void)
{
    struct allocator allocator;
    unsigned int size = -1;
    scanf("%d", &size);
    if(init_allocator(&allocator, size))
    {
        return -1;
    }
    int was_eof;
    do
    {
        char *line = my_getline(&was_eof);
        if (!line)
        {
            break;
        }
        if(!strcmp("INFO", line))
        {
            struct stat stat = info(&allocator);
            printf("max_alloc: %d used_blocks: %d used_mem: %d\n", stat.max_alloc, stat.used_blocks, stat.used_mem);
            free(line);
            continue;
        }
        if(!strcmp("MAP", line))
        {
            char *map_str = map(&allocator);
            if(map_str)
            {
                printf("%s\n", map_str);
                free(map_str);
            }
            free(line);
            continue;
        }
        char *space_pos = strchr(line, ' ');
        if(!space_pos)
        {
            free(line);
            continue;
        }
        *space_pos = '\0';
        ++space_pos;
        if(!strcmp("FREE", line))
        {
            char *p = (char*) atoi(space_pos);
            printf("%c\n", allocator_free(&allocator, p) ? '-' : '+');
            free(line);
            continue;
        }
        if(!strcmp("ALLOC", line))
        {
            int size = atoi(space_pos);
            char *p = alloc(&allocator, size);
            if(p)
            {
                printf("+ %d\n", (int)p);
            }
            else
            {
                printf("-\n");

            }
            free(line);
            continue;
        }
        free(line);
    } while (!was_eof);
    destroy_allocator(&allocator);
    return 0;
}

