#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <queue>

using namespace std;


struct io {
    int begin;
    int duration;
    int need_time;
    io(int m_begin, int m_duration):begin(m_begin), duration(m_duration), need_time(m_duration) {}
};

struct process {
    string name;
    int begin;
    int duration;
    int need_time;
    queue <io> io_list;

    bool isClose();

    process(string m_name, int m_begin, int m_duration, queue <io> m_io_list):
        name(m_name), begin(m_begin), duration(m_duration), io_list(m_io_list), need_time(m_duration)
    {}


    bool finishes_in_time(size_t time) const
    {
        return need_time <= time;
    }

    bool io_in_time(size_t time) const
    {
        return !io_list.empty() &&  (duration - need_time) + time >= io_list.front().begin;
    }
};

bool process::isClose() {
    return (need_time == 0) ? true : false;
}

vector<string> split(string s) {
    vector<string> elems;
    istringstream ss(s);
    while(ss>>s)
        elems.push_back(s);

    return elems;
}

process handle_string(string in_string) {
    vector <string> parts = split(in_string);
    string proc_name = parts[0];
    int proc_begin = atoi(parts[1].c_str());
    int proc_duration = atoi(parts[2].c_str());
    int size = parts.size();
    queue <io> proc_io_operations;
    if (size > 3) {
        for(int i = 3; i !=  size; i += 2) {
            io one_op(atoi(parts[i].c_str()), atoi(parts[i+1].c_str()));
            proc_io_operations.push(one_op);
        }

    }
    process pr(proc_name, proc_begin, proc_duration, proc_io_operations);
    return pr;
}
struct compare_proc_begins {
    bool operator()(process const & p1, process const & p2) {
        return !(p1.begin < p2.begin);
    }
};

class compare_proc_priority
{
public:
    compare_proc_priority(size_t quant): m_quant(quant) {}

    bool operator()(process const& right, process const& left)
    {
        if(left.io_in_time(m_quant))
            return left.io_in_time(m_quant);
        else if (left.finishes_in_time(m_quant))
                return left.finishes_in_time(m_quant);
        else if (right.finishes_in_time(m_quant))
            return !right.finishes_in_time(m_quant);
        else return !right.io_in_time(m_quant);
    }


private:
    size_t m_quant;
};

typedef priority_queue <process, vector <process>, compare_proc_priority> queue_by_proc_priority;
typedef priority_queue <process, vector <process>, compare_proc_begins> queue_by_proc_beginings;

void handle_processes(queue_by_proc_priority &available_queue, queue_by_proc_beginings &not_available_queue, size_t quant)
{
    int timer = 0;

    while (!(available_queue.empty()) || (!(not_available_queue.empty()))) {
        while(!(not_available_queue.empty()) &&(not_available_queue.top().begin <= timer)) {
            available_queue.push(not_available_queue.top());
            not_available_queue.pop();

        }

        if (available_queue.empty()) {
            cout << timer << " IDLE" << endl;
            timer = not_available_queue.top().begin;

        }
        else
        {
            process current_process = available_queue.top();
            available_queue.pop();

            if (current_process.finishes_in_time(quant))
            {   cout << timer << " " << current_process.name << endl;
                timer += current_process.need_time;
            }
            else if (current_process.io_in_time(quant))
            {
                io current_io = current_process.io_list.front();

                current_process.io_list.pop();

                cout << timer << " " << current_process.name << endl;
                size_t time_to_io = current_io.begin - (current_process.duration - current_process.need_time);
                current_process.need_time -= time_to_io + current_io.duration;
                timer += time_to_io;

                current_process.begin = timer + current_io.duration;
                not_available_queue.push(current_process);
            }    else            {
                current_process.need_time -= quant;
                cout << timer << " " << current_process.name << endl;

                timer += quant;
                current_process.begin = timer;

                not_available_queue.push(current_process);
            }
        }
    }
}

int main()
{
    int quant;
    cin >> quant;
    compare_proc_priority comparer(quant);
    queue_by_proc_priority available_processes(comparer);
    queue_by_proc_beginings not_available_processes;

    string line = "";
    while (getline(cin, line)) {
        if (!line.empty()) {
            not_available_processes.push(handle_string(line));
        }
    }
    handle_processes(available_processes, not_available_processes, quant);

    return 0;
}

