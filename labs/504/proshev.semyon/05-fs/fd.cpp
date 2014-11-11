#include <stdint.h>

struct FD
{
    int64_t id;

    int64_t parent_id;
    int64_t first_child_id;

    int64_t next_id;
    int64_t prev_id;

    int64_t first_block_id;

    char name[10];

    bool is_dir;

    FD()
    {
        id = -1;

        parent_id = -1;
        first_child_id = -1;

        next_id = -1;
        prev_id = -1;

        first_block_id = -1;
    }
};