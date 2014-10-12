#include <iostream>
#include <climits>
#include <string>

#define MAGIC_NUMBER 45
#define BUSY 1
#define FREE 2

struct MCB
{
    MCB    *prev = nullptr;
    MCB    *next = nullptr;
    bool    status = BUSY; // 0 - free, 1 - busy, 2 - system
    short magic_number = MAGIC_NUMBER;
    size_t size = 0;
};

typedef MCB mcb_t;

class Memory_allocator
{
public:
    Memory_allocator(size_t size)
        : data(new char[size])
        , all_size(size)
        , used_memory(0)
        , reserved_blocks(0)
        , free_memory(size - sizeof(mcb_t))
        , end(data + size)
        , zero_MCB(reinterpret_cast<MCB *>(data))

    {
        zero_MCB->next = nullptr;
        zero_MCB->prev = nullptr;
        zero_MCB->status = BUSY;
        zero_MCB->size = 0;
    }
    ~Memory_allocator()
    {
        delete[] data;
    }

    int allocate(size_t size)
    {
        if (size < 1)
        {
            return -1;
        }
        MCB * next_MCB = nullptr;
        if (size + sizeof(MCB) > free_memory)
        {
            return -1;
        }
        if (zero_MCB->next == nullptr)
        {
            next_MCB = reinterpret_cast<MCB *>(end_mcb(zero_MCB));
            init_MCB(next_MCB, size);
            next_MCB->prev = zero_MCB;
            zero_MCB->next = next_MCB;
            used_memory += size;
	    reserved_blocks += 1;
            free_memory -= (size + sizeof(mcb_t));
            
        }
        else 
        {
            MCB * left_MCB = find_min_gap_for_size(size);
            if (left_MCB != nullptr)
            {
                next_MCB = reinterpret_cast<MCB *>(end_mcb(left_MCB));
                init_MCB(next_MCB, size);
                used_memory += size;
                free_memory -= (size + sizeof(mcb_t));
                reserved_blocks += 1;

                if (left_MCB->next != nullptr)
                {
                    next_MCB->next = left_MCB->next;
                    next_MCB->next->prev = next_MCB;
                    left_MCB->next = next_MCB;
                    next_MCB->prev = left_MCB;
                }
                else
                {
                    left_MCB->next = next_MCB;
                    next_MCB->prev = left_MCB;
                }
            }
            else
            {
                return -1;
            }
        }
        int offset = get_offset(next_MCB);
        return offset;
    }
    void free(unsigned offset)
    {
      /*
        if (offset < 0)
        {
            std::cout << "-" << std::endl;
            return;
        }*/

        char *start_mcb = data + offset - sizeof(MCB);
        MCB *expected_mcb = reinterpret_cast<MCB *>(start_mcb);
        if (expected_mcb->magic_number == MAGIC_NUMBER &&  expected_mcb->status == BUSY)
        {
            if (expected_mcb->next != nullptr)
            {
                (expected_mcb->prev)->next = expected_mcb->next;
                (expected_mcb->next)->prev = expected_mcb->prev;
            }
            else
            {
                (expected_mcb->prev)->next = nullptr;
            }
            reserved_blocks -= 1;
            used_memory -= expected_mcb->size;
            free_memory += expected_mcb->size + sizeof(MCB);
            expected_mcb->status = FREE;
            std::cout << "+";
        }
        else
        {
            std::cout << "-" << std::endl;
        }
    }

    std::string map()
    {
        MCB *mcb = zero_MCB;
        std::string memory_map;

        while (mcb->next != nullptr)
        {
            memory_map += mcb_stat(mcb);
            for (int i = 0; i < start_mcb(mcb->next) - end_mcb(mcb); ++i)
            {
                memory_map.push_back('f');
            }
            mcb = mcb->next;
        }
        memory_map += mcb_stat(mcb);
        for (auto i = end_mcb(mcb); i != end; ++i)
        {
            memory_map.push_back('f');
        }
        return memory_map;
    }
    void info()
    {
        //std::cout << "reserved blocks " << reserved_blocks << std::endl;
        //std::cout << "used memory " << used_memory << std::endl;
        //std::cout << "max available " << max_alloc_size() << std::endl;
        std::cout << reserved_blocks << " " << used_memory << " " << max_alloc_size() << std::endl;
    }

private:
    const int SYSTEM = 2;
    //const int FREE = 0;
    //const int BUSY = 1;
    char * data;
    size_t all_size;
    size_t used_memory;
    size_t reserved_blocks;
    size_t free_memory;
    char * end;
    MCB * const zero_MCB;

    char * start_mcb(MCB * mcb)
    {
        char * start_mcb = reinterpret_cast<char *>(mcb);
        return start_mcb;
    }

    char * end_mcb(MCB * mcb)
    {
        return start_mcb(mcb) + sizeof(mcb_t) + mcb->size;
    }

    char * start_mcb_data(MCB * mcb)
    {
        char * start_mcb_data = start_mcb(mcb) + sizeof(MCB);
        return start_mcb_data;
    }

    size_t get_blocks_gap(MCB * prev, MCB * next)
    {
        size_t gap = start_mcb(next) - end_mcb(prev);
        return gap;
    }

    int get_offset(MCB * mcb)
    {
        //size_t size_zero = sizeof(mcb_t);
        //size_t size = sizeof(mcb_t);
        //return start_mcb_data(mcb) - start_mcb(zero_MCB);
        return start_mcb_data(mcb) - data;
    }

    MCB * find_min_gap_for_size(size_t size)
    {
        if (zero_MCB->next == nullptr)
        {
            return zero_MCB;
        }
        MCB *mcb = zero_MCB;
        std::pair<MCB *, size_t> min_gap(nullptr, INT_MAX);
        while (mcb->next != nullptr)
        {
            size_t gap = get_blocks_gap(mcb, mcb->next);
            if (gap >= size + sizeof(MCB) && gap < min_gap.second)
            {
                min_gap = std::make_pair(mcb, gap);
            }
            mcb = mcb->next;
        }
        if (min_gap.first != nullptr)
        {
            return min_gap.first;
        }
        else if (static_cast<unsigned>(end - end_mcb(mcb)) >= size + sizeof(MCB))
        {
            return mcb;
        }
        else
        {
            return nullptr;
        }
    }

    size_t max_alloc_size()
    {
        MCB * mcb = zero_MCB;
        size_t max_alloc_size = 0;
        while (mcb->next != nullptr)
        {
            size_t gap_size = get_blocks_gap(mcb, mcb->next);
            if (gap_size > max_alloc_size + sizeof(MCB))
            {
                max_alloc_size = gap_size;
            }
            mcb = mcb->next;
        }
        if (static_cast<unsigned>(end - end_mcb(mcb)) > max_alloc_size + sizeof(MCB))
        {
            max_alloc_size = end - end_mcb(mcb);
        }
        if (max_alloc_size != 0)
        {
            return max_alloc_size - sizeof(MCB);
        }
        else
        {
            return 0;
        }
    }

    std::string mcb_stat(MCB * mcb)
    {
        std::string mcb_map;
        for (auto i = mcb; i != mcb + sizeof(MCB); ++i)
        {
            mcb_map.push_back('m');
        }
        for (size_t i = 0; i < mcb->size; ++i)
        {
            mcb_map.push_back('u');
        }
        return mcb_map;
    }

    void init_MCB(MCB * mcb, size_t size)
    {
        mcb->size = size;
        mcb->next = nullptr;
        mcb->prev = nullptr;
        mcb->magic_number = MAGIC_NUMBER;
        mcb->status = BUSY;
    }
};