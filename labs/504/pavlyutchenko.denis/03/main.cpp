#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <list>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>
#include <functional>

using namespace std;

class allacator
{
public:
    explicit allacator(unsigned int N)
    {
        memory_size = N;

        MCB first_node;
        memory.push_back(first_node);
    }

    string alloc(const unsigned int s)
    {
        if (s >= memory_size || s == 0)
        {
            return "-";
        }

        unsigned int mcb_size = get_MCB_size();
        unsigned int total_size = 0;

        //Если есть какой-нибудь свободный блок, подходящий нам
        for (MCB& mcb : memory)
        {
            //Если текущий блок свободен
            if (!mcb.is_occupied())
            {
                //Если размер текущего блока нас устраивает
                if (mcb.get_size() >= s)
                {
                    mcb.occupy();

                    return "+ " + to_string(total_size);
                }
            }

            total_size += mcb.get_size();
            total_size += mcb_size;
        }


        // Если мы можем выделить память в новом блоке памяти
        if (total_size + mcb_size + s <= memory_size)
        {
            MCB node(s);
            memory.push_back(node);

            return "+ " + to_string(total_size);
        }

        //Возможно, можно смёржить несколько блоков

        int out_index = 1;
        int inner_index = 1;
        int segment = 0;

        int max_segment = 0;
        int max_out_index = 0;
        int max_inner_index = 0;

        bool is_enough = false;

        for (auto iterator = (++memory.begin()); iterator != memory.end(); ++iterator)
        {
            if (is_enough)
            {
                break;
            }

            segment = 0;
            inner_index = out_index + 1;

            //Этот элемент уже занят?
            if ((*iterator).is_occupied())
            {
                out_index++;
                continue;
            }

            segment = (*iterator).get_size();

            auto tmp_it = iterator;

            for (auto inner_itaretor = (++tmp_it); inner_itaretor != memory.end(); ++inner_itaretor)
            {
                // Этот элемент уже занят?
                if ((*inner_itaretor).is_occupied())
                {
                    inner_index++;
                    break;
                }

                segment += (*inner_itaretor).get_size();

                if (segment > max_segment)
                {
                    max_segment = segment;
                    max_out_index = out_index;
                    max_inner_index = inner_index;

                    unsigned int avaible_spase = max_segment + (max_inner_index - max_out_index) * mcb_size;

                    if (avaible_spase >= s)
                    {
                        is_enough = true;
                        break;
                    }
                }

                inner_index++;
            }

            out_index++;
        }

        unsigned int avaible_spase = max_segment + (max_inner_index - max_out_index) * mcb_size;

        if (avaible_spase < s)
        {
            return "-";
        }

        int current_index = 0;
        total_size = 0;

        int is_succesful = -1;

        for (auto& mcb : memory)
        {
            if (current_index == max_out_index)
            {
                mcb.increase_size_of(avaible_spase - mcb.get_size());

                if (mcb.get_size() >= s)
                {
                    mcb.occupy();
                }

                is_succesful = total_size;
                break;
            }

            total_size += mcb.get_size();
            total_size += mcb_size;

            current_index++;
        }

        current_index = 0;

        for (auto itr = memory.begin(); itr != memory.end();)
        {
            if (current_index > max_out_index && current_index <= max_inner_index)
            {
                itr = memory.erase(itr);
            }
            else
            {
                ++itr;
            }

            current_index++;
        }

        if (is_succesful > -1)
        {
            return "+ " + to_string(is_succesful);
        }

        return "-";
    }

    string free(const unsigned int p)
    {
        if (p == 0)
        {
            return "-";
        }

        unsigned int mcb_size = get_MCB_size();
        unsigned int total_size = 0;

        auto it = memory.begin();
        unsigned int current_index = 0;

        while (it != memory.end())
        {
            MCB& mcb = *it;

            //Нашли нужную MCB структуру по заданному смещению
            if (total_size == p)
            {
                // Этот блок Уже свободен?
                if (!mcb.is_occupied())
                {
                    return "-";
                }

                mcb.free();

                // Если это последний блок
                if (current_index == memory.size() - 1)
                {
                    //Удаляем текущий блок
                    memory.erase(it++);

                    --it;

                    //Пока предыдущая MCB структура свободна
                    while (!(*it).is_occupied() && current_index >= 2)
                    {
                        //Удаляем предыдущий блок
                        memory.erase(it++);

                        --it;
                    }
                }

                return "+";
            }

            ++it;

            total_size += mcb.get_size();
            total_size += mcb_size;

            current_index += 1;
        }

        return "-";
    }

    string info() const
    {
        unsigned int number_of_bloks = 0;
        unsigned int count_of_allocated_memory = 0;

        unsigned int max_s = 0;
        unsigned int total_size = 0;
        unsigned int mcb_size = get_MCB_size();

        int current_index = 0;

        for (auto mcb : memory)
        {
            if (current_index == 0)
            {
                current_index++;
                continue;
            }

            if (mcb.is_occupied())
            {
                number_of_bloks += 1;
                count_of_allocated_memory += mcb.get_size();
            }
            else
            {
                max_s = max(mcb.get_size(), max_s);
            }

            total_size += mcb.get_size();
            total_size += mcb_size;

            current_index++;
        }

        int free_space = memory_size - total_size - mcb_size - mcb_size;
        if (free_space < 0)
        {
            free_space = 0;
        }

        max_s = max(static_cast<unsigned int>(free_space), max_s);

        //Мёржим различные блоки

        int out_index = 1;
        int inner_index = 1;
        int segment = 0;

        int max_segment = 0;
        int max_out_index = 0;
        int max_inner_index = 0;

        for (auto iterator = (++memory.begin()); iterator != memory.end(); ++iterator)
        {
            segment = 0;
            inner_index = out_index + 1;

            //Этот элемент уже занят?
            if ((*iterator).is_occupied())
            {
                out_index++;
                continue;
            }

            segment = (*iterator).get_size();

            auto tmp_it = iterator;

            for (auto inner_itaretor = (++tmp_it); inner_itaretor != memory.end(); ++inner_itaretor)
            {
                // Этот элемент уже занят?
                if ((*inner_itaretor).is_occupied())
                {
                    inner_index++;
                    break;
                }

                segment += (*inner_itaretor).get_size();

                if (segment > max_segment)
                {
                    max_segment = segment;
                    max_out_index = out_index;
                    max_inner_index = inner_index;
                }

                inner_index++;
            }

            out_index++;
        }

        unsigned int avaible_spase = max_segment + (max_inner_index - max_out_index) * mcb_size;

        max_s = max(max_s, avaible_spase);

        return to_string(number_of_bloks) + " " + to_string(count_of_allocated_memory) + " " + to_string(max_s);
    }

    string map() const
    {
        string result = "";

        for (unsigned int i = 0; i < memory_size; ++i)
        {
            result += "f";
        }

        int current_index = 0;
        int mcb_size = get_MCB_size();

        for (auto mcb : memory)
        {
            for (int i = current_index; i < mcb_size + current_index; ++i)
            {
                result[i] = 'm';
            }

            current_index += mcb_size;

            for (unsigned int i = current_index; i < current_index + mcb.get_size(); ++i)
            {
                if (mcb.is_occupied())
                {
                    result[i] = 'u';
                }
            }

            current_index += mcb.get_size();
        }

        return result;
    }

    void print_mcb() const
    {
        bool flag = true;

        for (auto mcb : memory)
        {
            if (flag)
            {
                flag = false;
                continue;
            }

            cout << mcb.is_occupied() << " " << mcb.get_size() << endl;
        }
    }

private:
    unsigned int memory_size;

    struct MCB;
    list<MCB> memory;

    unsigned int get_MCB_size() const
    {
        return sizeof(MCB);
    }

    struct MCB
    {
    public:
        explicit MCB(unsigned int s = 0)
        {
            size = s;
            occupied = true;
        }

        // Этот участок занят?
        bool is_occupied() const
        {
            return occupied;
        }

        // Размер текущего участка
        unsigned int get_size() const
        {
            return size;
        }

        // Занять место
        void occupy()
        {
            occupied = true;
        }

        // Освободить место
        void free()
        {
            occupied = false;
        }

        // Увеличить размер на
        void increase_size_of(unsigned int i)
        {
            size += i;
        }

    private:
        unsigned int size;
        bool occupied;
    };
};

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

int main(int argc, char** argv) {

    int N;
    cin >> N;

    allacator my_allacator(N);

    string line;
    getline(cin, line);

    const string alloc = "ALLOC";
    const string free = "FREE";
    const string info = "INFO";
    const string map = "MAP";

    while (!getline(cin, line).eof())
    {
        if (line == "") {
            break;
        }

        if (line.compare(0, alloc.length(), alloc) == 0)
        {
            vector<string> x = split(line, ' ');
            int S = atoi(x[1].c_str());

            cout << my_allacator.alloc(S) << endl;
        }

        if (line.compare(0, free.length(), free) == 0)
        {
            vector<string> x = split(line, ' ');
            int P = atoi(x[1].c_str());

            cout << my_allacator.free(P) << endl;
        }

        if (line.compare(0, info.length(), info) == 0)
        {
            cout << my_allacator.info() << endl;
        }

        if (line.compare(0, map.length(), map) == 0)
        {
            cout << my_allacator.map() << endl;
        }
    }

    return 0;
}