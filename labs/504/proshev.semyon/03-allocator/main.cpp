#include <iostream>
#include <fstream>
#include <stddef.h>

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
    size_t size_;
    MCB* root_;

public:
    Allocator(size_t size) : buffer_(new char[size]), size_(size) {
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
        if (desired_size > size_ - sizeof(MCB))
        {
            std::cout << "-" << std::endl;
            return;
        }

        MCB* current = root_;

        while (current->used || current->size < desired_size + sizeof(MCB))
        {
            current = current->next;

            if (current == nullptr)
            {
                std::cout << "-" << std::endl;
                return;
            }
        }

        if ((char*) (current + 2) > buffer_ + size_ - desired_size)
        {
            current->next = nullptr;
        }
        else
        {
            MCB* next = (MCB*) ((char*) (current + 1) + desired_size);

            next->size = current->size - desired_size - sizeof(MCB);
            next->used = false;
            next->prev = current;
            next->next = current->next;

            if (current->next != nullptr) {
                current->next->prev = next;
            }

            current->next = next;
        }

        current->size = desired_size + sizeof(MCB);
        current->used = true;

        std::cout << "+" << " " << (char*) (current + 1) - (char*) root_ << std::endl;
    }

    void my_free(const size_t offset)
    {
        if (offset > size_)
        {
            std::cout << "-" << std::endl;
            return;
        }

        MCB* current = root_;
        char* pointer = buffer_ + offset;

        while (pointer != (char*) (current + 1))
        {
            current = current->next;

            if (current == nullptr)
            {
                std::cout << "-" << std::endl;
                return;
            }
        }

        current->used = false;

        if (current->next == nullptr && (char*) (current + 1) + current->size != buffer_ + size_)
        {
            current->size += buffer_ + size_ - (char*) (current) - current->size;
        }

        if (current->next != nullptr && !current->next->used)
        {
            MCB* next = current->next;

            current->size += next->size;
            current->next = next->next;

            if (next->next != nullptr)
            {
                next->next->prev = current;
            }
        }

        if (current->prev != nullptr && !current->prev->used)
        {
            MCB* prev = current->prev;

            prev->size += current->size;
            prev->next = current->next;

            if (current->next != nullptr)
            {
                current->next->prev = prev;
            }
        }

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

        while (1)
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
            }
            else
            {
                for (size_t i = 0; i < current->size - sizeof(MCB); ++i)
                {
                    std::cout << "f";
                }
            }

            if (current->next == nullptr)
            {
                for (size_t i = 0; i < buffer_ + size_ - (char*) (current) - current->size; ++i)
                {
                    std::cout << "f";
                }

                break;
            }

            current = current->next;
        }

        std::cout << std::endl;
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