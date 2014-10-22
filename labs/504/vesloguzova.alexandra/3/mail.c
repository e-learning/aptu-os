#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct allocator
{
    int buffer_size;
    int block_count;
    int allocate_memory_size;
    char *buffer;
};

struct list_elem
{
    int size;
    int handler;
    struct list_elem *next;
};

typedef struct allocator allocator;
typedef struct list_elem list_elem;

void map(allocator a);

void info(allocator a);

int free_block(allocator *a, int offset);

int alloc(allocator *a, int count);

int max_free_space(allocator a);

void defragmentation(allocator *a);

int main(void)
{
    allocator a;
    int buffer_size;
    scanf("%d", &buffer_size);
    if(buffer_size<=0){
        printf("Incorrect size!!\n");
        return -1;
    }
    a.buffer_size = buffer_size;
    a.allocate_memory_size = 0;
    a.block_count = 0;
    a.buffer = (char *) malloc(a.buffer_size * sizeof(char));
    list_elem *sys_info = (list_elem *) a.buffer;
    sys_info->next = 0;
    sys_info->size = 0;

    char buffer[6];
    while (scanf("%s", buffer))
    {
        if (strncmp("ALLOC", buffer, 5) == 0)
        {
            int s;
            scanf("%d", &s);
            int result = alloc(&a, s);
            if (result > 0)
            {
                printf("+%d\n", result);
            }
            else
            {
                printf("-\n");
            }
        }
        else if (strncmp("FREE", buffer, 4) == 0)
        {
            int p;
            scanf("%d", &p);
            if (free_block(&a, p))
            {
                printf("+\n");
                defragmentation(&a);
            } else
            {
                printf("-\n");
            }
        } else if (strncmp("INFO", buffer, 4) == 0)
        {
            info(a);
        } else if (strncmp("MAP", buffer, 3) == 0)
        {
            map(a);
        } else if (strncmp("EXIT", buffer, 4) == 0)
        {
            break;
        }
    }
    free(a.buffer);
    return 0;
}


int alloc(allocator *a, int count)
{
    list_elem *root = (list_elem *) a->buffer;
    list_elem *node = (list_elem *) a->buffer;
    int free_bloc_size = 0;
    while (node->next)
    {
        free_bloc_size = ((int) (node->next - node) - 2 * (int) sizeof(list_elem) - node->size);
        if (free_bloc_size >= count)
        {
            list_elem *new_block = node + node->size + sizeof(list_elem);
            new_block->next = node->next;
            new_block->size = count;
            node->next = new_block;
            a->block_count++;
            a->allocate_memory_size += count;
            new_block->handler = node->handler+1;
            return new_block->handler;
        }
        node = node->next;
    }
    int free_space_size = a->buffer_size - (int) (node - root) - 2 * (int) sizeof(list_elem) - node->size;
    if (free_space_size >= count)
    {
        list_elem *new_block = node + node->size + sizeof(list_elem);
        new_block->next = 0;
        new_block->size = count;
        node->next = new_block;
        a->block_count++;
        a->allocate_memory_size += count;
        new_block->handler = node->handler+1;
        return new_block->handler;
    } else
    {
        return -1;
    }
}

void defragmentation(allocator *a){
    list_elem *root = (list_elem *) a->buffer;
    list_elem *node = (list_elem *) a->buffer;
    int free_bloc_size = 0;
    while (node->next)
    {
        free_bloc_size = ((int) (node->next - node) - 2 * (int) sizeof(list_elem) - node->size);
        if (free_bloc_size > 0)
        {

            char* p_old_data = (char*)node->next;
            char* p_new_data = (char*)node + sizeof(list_elem) + node->size;
            size_t data_size = sizeof(list_elem)+ node->next->size;
            int i;
            for (i = 0; i < data_size; ++i)
            {
                p_new_data[i] = p_old_data[i];
            }
            node->next =(list_elem*)p_new_data;

        }
        node = node->next;
    }
}

int free_block(allocator *a, int offset)
{
    list_elem *node = (list_elem *) a->buffer;
    if (offset == 0)
    {
        return 0;
    }
    while (node->next)
    {
        if (node->next->handler == offset)
        {
            a->allocate_memory_size -= node->next->size;
            a->block_count--;
            node->next = node->next->next;
            return 1;
        }
        else
        {
            node = node->next;
        }
    }
    return 0;

}

void info(allocator a)
{
    printf("%d %d %d\n", a.block_count, a.allocate_memory_size, max_free_space(a));


}

void map(allocator a)
{
    list_elem *root = (list_elem *) a.buffer;
    list_elem *node = (list_elem *) a.buffer;
    while (node)
    {
        int i = 0;
        for (i = 0; i < (int) sizeof(list_elem); i++)
        {
            printf("m");
        }
        for (i = 0; i < node->size; ++i)
        {
            printf("u");
        }
        int free_block_size;
        if (node->next)
        {
            free_block_size = ((int) (node->next - node) - (int) sizeof(list_elem) - node->size);
        } else
        {
            free_block_size = a.buffer_size - (int) (node - root) - (int) sizeof(list_elem) - node->size;
        }
        for (i = 0; i < free_block_size; ++i)
        {
            printf("f");
        }
        node = node->next;
    }
    printf("\n");
}

int max_free_space(allocator a)
{
    list_elem *root = (list_elem *) a.buffer;
    list_elem *node = (list_elem *) a.buffer;
    int max_free_block_size = 0;
    while (node)
    {
        int free_block_size;
        if (node->next)
        {
            free_block_size = ((int) (node->next - node) - (int) sizeof(list_elem) - node->size);
        } else
        {
            free_block_size = a.buffer_size - (int) (node - root) - (int) sizeof(list_elem) - node->size;
        }
        if (free_block_size > max_free_block_size)
        {
            max_free_block_size = free_block_size;
        }
        node = node->next;
    }
    if (max_free_block_size > (int) sizeof(list_elem))
    {
        return max_free_block_size - (int) sizeof(list_elem);
    }
    else return 0;
}
