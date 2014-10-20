#include <stdlib.h>
#include <stdio.h>
#include "page_table.h"
#include <stdint.h>
#include <inttypes.h>

extern unsigned translate(unsigned pointer, unsigned short selector,
        struct entry_table *page_dir, struct entry_table *page_table,
        struct descriptor_table *gdt, struct descriptor_table *ldt);

void init(unsigned *pointer, unsigned short *selector,
        struct entry_table *page_dir, struct entry_table *page_table,
        struct descriptor_table *gdt, struct descriptor_table *ldt)
{
    int index;
    scanf("%x", pointer);
    scanf("%hx", selector);

    scanf("%u", &(gdt->size));
    gdt->content = malloc(gdt->size*sizeof(uint64_t));
    for(index = 0; index < gdt->size; index++)
    {
        scanf("%"SCNx64, gdt->content+index);
    }

    scanf("%u", &(ldt->size));
    ldt->content = malloc(ldt->size*sizeof(uint64_t));
    for(index = 0; index < ldt->size; index++)
    {
        scanf("%"SCNx64, ldt->content+index);
    }

    scanf("%u", &(page_dir->size));
    page_dir->content = malloc(page_dir->size*sizeof(unsigned));
    for(index = 0; index < page_dir->size; index++)
    {
        scanf("%x", page_dir->content+index);
    }

    scanf("%u", &(page_table->size));
    page_table->content = malloc(page_table->size*sizeof(unsigned));
    for(index = 0; index < page_table->size; index++)
    {
        scanf("%x", page_table->content+index);
    }
}

void destroy(struct entry_table *page_dir, struct entry_table *page_table,
        struct descriptor_table *gdt, struct descriptor_table *ldt)
{
    free(page_dir->content);
    free(page_table->content);
    free(gdt->content);
    free(ldt->content);
}

int main()
{
    unsigned pointer;
    unsigned short selector;
    struct entry_table page_dir, page_table;
    struct descriptor_table gdt, ldt;
    unsigned result;

    init(&pointer, &selector, &page_dir, &page_table, &gdt, &ldt);

    result = translate(pointer, selector, &page_dir, &page_table, &gdt, &ldt);
    if(result)
    {
        printf("%x\n",result);
    }
    else
    {
        printf("INVALID\n");
    }

    destroy(&page_dir, &page_table, &gdt, &ldt);

    return 0;
}