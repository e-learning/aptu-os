#include <stdint.h>

struct BD
{
    int64_t id;
    int64_t next;
    int64_t len;

    BD()
    {
        id = -1;
        next = -1;
        len = -1;
    }
};