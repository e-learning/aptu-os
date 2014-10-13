#include <iostream>
#include <fstream>

struct MCB
{
    size_t size;
    MCB* next;
    MCB* prev;
    bool used;
};

class Allocator
{

private:
    char* buffer_;
    const size_t size_;
    MCB* root_;

public:
    Allocator(const size_t size) : buffer_(new char[size]), size_(size) {
        root_ = (MCB*) buffer_;

        root_->size = size;
        root_->next = nullptr;
        root_->prev = nullptr;
        root_->used = false;
    }

    ~Allocator() {
        delete[] buffer_;
    }

    void my_alloc(const size_t desired_size)
    {
        MCB* available_mcb = find_available_mcb(desired_size);

        if (available_mcb == nullptr)
        {
            std::cout << "-" << std::endl;
            return;
        }

        if (available_mcb->next == nullptr)
        {
            insert_new_last_mcb_if_possible(available_mcb, desired_size);
        }
        else
        {
            insert_new_mcb_if_possible(available_mcb, desired_size);
        }

        available_mcb->used = true;
        available_mcb->size = desired_size + sizeof(MCB);

        std::cout << "+" << " " << (char*) (available_mcb + 1) - (char*) root_ << std::endl;
    }

    void my_free(const size_t offset)
    {
        MCB* mcb = find_mcb_by_offset(offset);

        if (mcb == nullptr || !mcb->used)
        {
            std::cout << "-" << std::endl;
            return;
        }

        mcb->used = false;

        if (mcb->next == nullptr)
        {
            mcb->size = buffer_ + size_ - (char*) mcb;
        }
        else
        {
            mcb->size = (char*) mcb->next - (char*) mcb;
        }

        merge_mcb_and_next_if_possible(mcb);
        merge_mcb_and_next_if_possible(mcb->prev);

        std::cout << "+" << std::endl;
    }

    void my_info()
    {
        MCB* current = root_;
        size_t used = 0;
        size_t allocated = 0;
        size_t available = 0;

        while (current != nullptr)
        {
            if (current->used)
            {
                used++;
                allocated += current->size - sizeof(MCB);
            }
            else
            {
                if (current->size != 0)
                {
                    available = std::max(available, current->size - sizeof(MCB));
                }
            }

            current = current->next;
        }

        std::cout << used << " " << allocated << " " << available << std::endl;
    }

    void my_map()
    {
        MCB* current = root_;

        while (current != nullptr)
        {
            for (size_t i = 0; i < 32; ++i)
            {
                std::cout << "m";
            }

            if (current->used) {
                for (size_t i = 0; i < current->size - sizeof(MCB); ++i)
                {
                    std::cout << "u";
                }

                print_f_if_needed(current);
            }
            else
            {
                for (size_t i = 0; i < current->size - sizeof(MCB); ++i)
                {
                    std::cout << "f";
                }
            }

            current = current->next;
        }

        std::cout << std::endl;
    }

    void print_f_if_needed(const MCB* current) {
        if (current->next == nullptr)
        {
            size_t space_before_end = buffer_ + size_ - (char*) (current) - current->size;

            for (size_t i = 0; i < space_before_end; ++i)
            {
                std::cout << "f";
            }
        }
        else
        {
            size_t space_before_next = (char*) current->next - (char*) current - current->size;

            for (size_t i = 0; i < space_before_next; ++i)
            {
                std::cout << "f";
            }
        }
    }

private:
    MCB* find_available_mcb(const size_t desired_size)
    {
        MCB* mcb = root_;

        while (mcb != nullptr && (mcb->used || mcb->size < desired_size + sizeof(MCB)))
        {
            mcb = mcb->next;
        }

        return mcb;
    }

    void insert_new_last_mcb_if_possible(MCB* last_mcb, const size_t size) {
        char* new_last_ptr = (char*) (last_mcb + 1) + size;

        if (new_last_ptr <= buffer_ + size_ - sizeof(MCB))
        {
            MCB* new_last_mcb = (MCB*) new_last_ptr;

            new_last_mcb->size = last_mcb->size - size - sizeof(MCB);
            new_last_mcb->used = false;
            new_last_mcb->prev = last_mcb;
            new_last_mcb->next = nullptr;

            last_mcb->next = new_last_mcb;
        }
    }

    void insert_new_mcb_if_possible(MCB* mcb, const size_t size) {
        char* new_ptr = (char*) (mcb + 1) + size;

        if (new_ptr != (char*) mcb->next && new_ptr + sizeof(MCB) <= (char*) mcb->next)
        {
            MCB* new_mcb = (MCB*) new_ptr;

            new_mcb->size = mcb->size - size - sizeof(MCB);
            new_mcb->used = false;
            new_mcb->prev = mcb;
            new_mcb->next = mcb->next;

            mcb->next->prev = new_mcb;
            mcb->next = new_mcb;
        }
    }

    MCB* find_mcb_by_offset(const size_t offset)
    {
        MCB* mcb = root_;

        while (mcb != nullptr && buffer_ + offset != (char*) (mcb + 1))
        {
            mcb = mcb->next;
        }

        return mcb;
    }

    void merge_mcb_and_next_if_possible(MCB *mcb)
    {
        if (mcb == nullptr || mcb->next == nullptr || mcb->next->used || mcb->used)
        {
            return;
        }

        MCB* next = mcb->next;

        mcb->next = next->next;

        if (mcb->next == nullptr)
        {
            mcb->size = buffer_ + size_ - (char*) mcb;
        }
        else
        {
            mcb->next->prev = mcb;
            mcb->size = (char*) mcb->next - (char*) mcb;
        }
    }

};

int main(int argc, char** argv)
{
    size_t n;

    std::cin >> n;

    Allocator allocator(n);

    std::string command;

    while (std::cin >> command)
    {
        if (command == "ALLOC")
        {
            size_t desired_size;

            std::cin >> desired_size;

            allocator.my_alloc(desired_size);
        }
        else if (command == "FREE")
        {
            size_t pointer;

            std::cin >> pointer;

            allocator.my_free(pointer);
        }
        else if (command == "INFO")
        {
            allocator.my_info();
        }
        else if (command == "MAP")
        {
            allocator.my_map();
        }
        else
        {
            std::cout << "Invalid command" << std::endl;
        }
    }

    return 0;
}