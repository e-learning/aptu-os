#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

#ifndef max
    #define max( a, b ) ( ( ( a ) >= ( b ) ) ? ( a ) : ( b ) )
#endif

typedef int bool;
typedef struct MB MB;

struct MB {
    size_t size;
    MB* next;
};

MB* find_pointer(char * const memory, size_t size) {
    if (((MB*)memory)->size < sizeof(MB)) return NULL;
    MB* current_block = ((MB*)memory)->next;
    if (current_block == NULL) {
        if (((MB*)memory)->size - sizeof(MB) >= size + sizeof(MB)) return (MB*)memory;
        else return NULL;
    }
    if ((size_t)((char*)current_block - (memory + sizeof(MB))) >= size + sizeof(MB)) return (MB*)memory;
    while (current_block->next != NULL) {
        if ((size_t)((char*)current_block->next - ((char*)current_block + current_block->size + sizeof(MB))) >= size + sizeof(MB))
            return current_block;
        current_block = current_block->next;
    }
    size_t memory_size = ((MB*)memory)->size;
    if((size_t)(memory + memory_size - (size_t)((char*)current_block + current_block->size + sizeof(MB))) >= size + sizeof(MB))
        return current_block;
    return NULL;
}

size_t allocate_memory(char* const memory, size_t const size) {
    MB* next_free_block = find_pointer(memory, size);
    if (next_free_block == NULL) return 0;
    MB* new_block = NULL;
    if ((char*)next_free_block == memory) new_block = (MB*)((char*)next_free_block + sizeof(MB));
    else new_block = (MB*)((char*)next_free_block + sizeof(MB) + next_free_block->size);
    new_block->size = size;
    new_block->next = next_free_block->next;
    next_free_block->next = new_block;
    return (char*)(new_block) - memory;
}

bool free_memory(char* const memory, size_t const pointer) {
    MB* memory_block = (MB*)(memory + pointer);
    MB* current_block = (MB*)memory;
    while (current_block->next != NULL) {
        if (current_block->next == memory_block) {
            current_block->next = current_block->next->next;
            return true;
        }
        current_block = current_block->next;
    }
    return false;
}

void info(char* const memory) {
    size_t block_count = 0;
    size_t sum_memory = 0;
    size_t max_success = 0;
    MB* current_block = ((MB*)memory)->next;
    if (current_block == NULL) max_success = ((MB*)memory)->size - sizeof(MB);
    while (current_block != NULL) {
        ++block_count;
        sum_memory += current_block->size;
        if (current_block->next != NULL)
            max_success = max(max_success, (char*)current_block->next - ((char*)current_block + current_block->size + sizeof(MB)));
        else
            max_success = max(max_success, memory + ((MB*)memory)->size - ((char*)current_block + current_block->size + sizeof(MB)));
        current_block = current_block->next;
    }
    if (max_success < sizeof(MB)) max_success = sizeof(MB);
    printf("%zu %zu %zu\n", block_count, sum_memory, max_success - sizeof(MB));
}

void map(char* const memory) {
    for(size_t i = 0; i < sizeof(MB); ++i)
        printf("m");
    MB* current_block = ((MB*)memory)->next;
    if (current_block != NULL && (char*)current_block != memory + sizeof(MB)) {
        for(size_t i = 0; i < (size_t)((char*)current_block - (memory + sizeof(MB))); ++i)
            printf("f");
    }
    size_t sum_bytes = sizeof(MB);
    while (current_block != NULL) {
        for(size_t i = 0; i < sizeof(MB); ++i)
            printf("m");
        for(size_t i = 0; i < current_block->size; ++i)
            printf("u");
        sum_bytes += sizeof(MB) + current_block->size;
        if (current_block->next != NULL) {
            size_t free_bytes = ((char*)current_block->next - ((char*)current_block + sizeof(MB) + current_block->size));
            sum_bytes += free_bytes;
            for(size_t i = 0; i != free_bytes; ++i)
                printf("f");
        }
        current_block = current_block->next;
    }
    for(size_t i = 0; i < ((MB*)memory)->size - sum_bytes; ++i)
        printf("f");
    printf("\n");
}

int main(void)
{
    size_t mem_size = 0;
    scanf("%zu", &mem_size);
    char* const memory = (char*) malloc(mem_size);
    MB* super_record = (MB*)memory;
    super_record->size = mem_size;
    super_record->next = NULL;

    char cmd[255];
    while (1) {
        scanf("%s", cmd);
        if (strcmp(cmd, "ALLOC") == 0) {
            size_t size = 0;
            scanf("%zu", &size);
            size_t p = 0;
            if ((p = allocate_memory(memory, size))) printf("+ %zu\n", p);
            else printf("-\n");
        }
        else if (strcmp(cmd, "FREE") == 0) {
            size_t pointer = 0;
            scanf("%zu", &pointer);
            if (free_memory(memory, pointer)) printf("+\n");
            else printf("-\n");
        }
        else if (strcmp(cmd, "INFO") == 0) {
            info(memory);
        }
        else if (strcmp(cmd, "MAP") == 0) {
            map(memory);
        }
    }

    free(memory);
    return 0;
}

