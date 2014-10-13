#include "common.h"


void print_free(size_t system_length, size_t user_length)
{
    for (size_t i = 0; i < system_length;++i)
    {
        std::cout << 'm';
    }

    for (size_t i = 0; i < user_length; ++i)
    {
        std::cout << 'f';
    }
}


void print_engaged(size_t system_length, size_t user_length)
{
    for (size_t i = 0; i < system_length;++i)
    {
        std::cout << 'm';
    }
    for (size_t i = 0; i < user_length;++i)
    {
        std::cout << 'u';
    }
}

void initialize_allocator(Allocator & allocator, size_t N)
{
    allocator.size = N;
    allocator.user_blocks = 0;
    allocator.memory_allocated = 0;
    allocator.free_size = N;

    MCB init_mcb;
    init_mcb.bytes_before_next = N - sizeof(MCB);
    init_mcb.next_mcb = NULL;
    init_mcb.is_free = true;
    memcpy(allocator.memory_block, &init_mcb, sizeof(init_mcb));
    allocator.free_size -= sizeof(MCB);
}

void info(Allocator & allocator)
{
    size_t max_free_memory_block = 0;
    MCB *ptr = (MCB*)allocator.memory_block;
    while (ptr->next_mcb != NULL)
    {
        if (ptr->bytes_before_next > max_free_memory_block && ptr->is_free)
            max_free_memory_block = ptr->bytes_before_next;
        ptr = ptr->next_mcb;
    }
    if (ptr->bytes_before_next > max_free_memory_block && ptr->is_free)
        max_free_memory_block = ptr->bytes_before_next;
    std::cout << allocator.user_blocks << " " << allocator.memory_allocated
              << " " << max_free_memory_block << std::endl;
}


void map(Allocator & allocator)
{
    MCB *ptr = (MCB*)allocator.memory_block;
    while (ptr->next_mcb != NULL)
    {
        if ( ptr->is_free )
        {
            print_free(sizeof(MCB), ptr->bytes_before_next);
        }
        else
        {
            print_engaged(sizeof(MCB), ptr->bytes_before_next);
        }
        ptr = ptr->next_mcb;
    }
    if ( ptr->is_free )
    {
        print_free(sizeof(MCB), ptr->bytes_before_next);
    }
    else
    {
        print_engaged(sizeof(MCB), ptr->bytes_before_next);
    }
    std::cout << std::endl;
}


int alloc(Allocator & allocator, size_t S)
{
    int return_offset = -1;
    int current_offset = 0;
    if (allocator.free_size < S)
        return return_offset;
    else
    {
        MCB *ptr = (MCB*)allocator.memory_block;
        while ((ptr->bytes_before_next < S || !ptr->is_free) && ptr->next_mcb != NULL)
        {
            current_offset += ptr->bytes_before_next + sizeof(MCB);
            ptr = ptr->next_mcb;
        }
        if (ptr->bytes_before_next > S + 2 * sizeof(MCB))
        {
            ptr->is_free = false;
            MCB next_mcb;
            next_mcb.is_free = true;
            next_mcb.bytes_before_next = ptr->bytes_before_next - S - sizeof(MCB);
            ptr->bytes_before_next = S;
            next_mcb.next_mcb = ptr->next_mcb;
            int next_offset = current_offset + S + sizeof(MCB);
            memcpy(&allocator.memory_block[next_offset], &next_mcb, sizeof(MCB));
            allocator.free_size = allocator.free_size - S - sizeof(MCB);
            ptr->next_mcb = (MCB*)&allocator.memory_block[next_offset];
            return_offset = current_offset;
        }
        else if (ptr->bytes_before_next >= S)
        {
            ptr->is_free = false;
            return_offset = current_offset;
            allocator.free_size = allocator.free_size - ptr->bytes_before_next;
        }
        if (return_offset != -1)
        {
            allocator.user_blocks++;
            allocator.memory_allocated += ptr->bytes_before_next;
        }
    }
    return return_offset;
}


MCB * collapse_prev(MCB * mcb, MCB * prev, Allocator & allocator, int & mcb_offset)
{
    prev->next_mcb = mcb->next_mcb;
    prev->bytes_before_next += mcb->bytes_before_next + sizeof(MCB);
    allocator.free_size += sizeof(MCB);
    int bytes_in_mcb = sizeof(MCB) + mcb->bytes_before_next;
    memset(&allocator.memory_block[mcb_offset], 0, sizeof(MCB));
    mcb_offset = mcb_offset - bytes_in_mcb;
    return prev;
}

MCB * collapse_next(MCB * mcb, MCB * next, Allocator & allocator, int mcb_offset)
{
    mcb->next_mcb = next->next_mcb;
    int offset_to_erase = mcb_offset + sizeof(MCB) + mcb->bytes_before_next;
    mcb->bytes_before_next += next->bytes_before_next + sizeof(MCB);
    allocator.free_size += sizeof(MCB);
    memset(&allocator.memory_block[offset_to_erase], 0, sizeof(MCB));
    return mcb;
}

int free_alloc(Allocator & allocator, int P)
{
    MCB *mcb = (MCB*)allocator.memory_block;
    int current_offset = 0;
    MCB *previous = NULL;
    while (current_offset != P && mcb->next_mcb != NULL)
    {
        previous = mcb;
        current_offset += mcb->bytes_before_next + sizeof(MCB);
        mcb = mcb->next_mcb;
    }
    if (current_offset != P || mcb->is_free)
        return -1;
    mcb->is_free = true;
    allocator.user_blocks--;
    allocator.memory_allocated -= mcb->bytes_before_next;
    allocator.free_size += mcb->bytes_before_next;
    if (previous != NULL && previous->is_free)
    {
        mcb = collapse_prev(mcb, previous, allocator, current_offset);
    }
    if (mcb->next_mcb != NULL && mcb->next_mcb->is_free)
    {
        mcb = collapse_next(mcb, mcb->next_mcb, allocator, current_offset);
    }
    return 0;
}


int parse_command_line(std::string command, std::vector<std::string> const & commands, int & param)
{
    std::stringstream ssbuf(command);
    std::string operation;
    int operation_code = -1;
    ssbuf >> operation;
    if (operation == commands[0])
    {
        operation_code = ALLOC;
        ssbuf >> param;
    }
    if (operation == commands[1])
    {
        operation_code = FREE;
        ssbuf >> param;
    }
    if (operation == commands[2])
    {
        operation_code = INFO;
    }
    if (operation == commands[3])
    {
        operation_code = MAP;
    }
    if (operation == commands[4])
    {
        operation_code = EXIT;
    }
    return operation_code;
}


int main()
{
    const std::vector<std::string> supported_commands =
    {
        std::string("ALLOC"),
        std::string("FREE"),
        std::string("INFO"),
        std::string("MAP"),
        std::string("EXIT"),
    };

    Allocator allocator;
    int N;
    std::cin >> N;
    if ( N < 100 || N > 10000)
    {
        std::cout << "Your memory block has invalid size.\nShould be from 100 to 10000" << std::endl;
        return -1;
    }
    std::cin.ignore(256, '\n');
    allocator.memory_block = new char[N];
    if (allocator.memory_block == NULL)
    {
        std::cout << "Memory allocation failed" << std::endl;
        return -1;
    }

    initialize_allocator(allocator, N);

    std::string line;
    while (getline(std::cin, line))
    {
        int param = 0;
        int offset = -1;
        switch (parse_command_line(line, supported_commands, param))
        {
            case ALLOC:
                offset = alloc(allocator, param);
                if (offset < 0)
                {
                    std::cout << "-" << std::endl;
                }
                else
                {
                    std::cout << "+" << offset << std::endl;
                }
                break;
            case FREE:
                if (free_alloc(allocator, param) < 0)
                {
                    std::cout << "-" << std::endl;
                }
                else
                {
                    std::cout << "+" << std::endl;
                }
                break;
            case INFO:
                info(allocator);
                break;
            case MAP:
                map(allocator);
                break;
            case EXIT:
                std::cout << "Goodbye" << std::endl;
                delete[] allocator.memory_block;
                exit(0);
            default:
                std::cout << "Unknown operation. Try ALLOC S, FREE P, INFO and MAP" << std::endl;
        }
    }
    return 0;
}


