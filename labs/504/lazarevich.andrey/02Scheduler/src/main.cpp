#include "common.hpp"

using namespace std;

string next_block(string const & task, int & start_space)
{
    if (start_space)
        ++start_space; //jump over space symbol if not in the beginning of the string
    int next_space = start_space;
    string sub = "";
    while (static_cast<unsigned int>(next_space) <= task.length())
    {
        if (task[next_space] == ' ' || static_cast<unsigned int>(next_space) == task.length())
        {
            sub = task.substr(start_space, next_space - start_space);
            break;
        }
        ++next_space;
    }
    start_space = next_space;
    return sub;
}

IO next_io_block(string const & task, int & start_space)
{
    IO io_task;
    io_task.start_io = 0;
    io_task.end_io = 0;
    if (static_cast<unsigned int>(start_space + 1) < task.length())
    {
        int next_space = start_space + 1;
        while (static_cast<unsigned int> (next_space) <= task.length())
        {
            if (task[next_space] == ')')
                break;
            ++next_space;
        }
        while (static_cast<unsigned int> (start_space) <= task.length())
        {
            if (task[start_space] == '(')
                break;
            ++start_space;
        }
        string sub = task.substr(start_space + 1, next_space - start_space -1);
        std::stringstream ss(sub);
        string item;
        vector<string>elems;
        while (getline(ss, item, ','))
        {
               elems.push_back(item);
        }
        io_task.start_io = std::stoi(elems[0]);
        io_task.end_io = std::stoi(elems[1]);
        start_space = next_space;
    }
    return io_task;
}

Task parse_input_process(string const & task)
{
    Task parsed_task;
    vector<IO> io_tasks;
    int start_space = 0;
    parsed_task.name = next_block(task, start_space);
    parsed_task.start_time = stoi(next_block(task, start_space));
    parsed_task.end_time = stoi(next_block(task, start_space));

    do
    {
        IO io_task = next_io_block(task, start_space);
        if (!(io_task.end_io == 0 && io_task.start_io == 0))
            io_tasks.push_back(io_task);
    }while (static_cast<unsigned int>(start_space + 1) < task.length());

    parsed_task.io_tasks = io_tasks;
    parsed_task.status = ReadyL;
    parsed_task.quant_avail = 0;

    return parsed_task;
}

int set_highest_priority(vector<Task> & tasks)
{
    int proc = 0;
    if (!tasks.empty())
    {
        int size = static_cast<int>(tasks.size());
        for (int i = 0; i < size; ++i)
        {
            if (tasks[i].start_time == 0)
                tasks[i].status = ReadyH;
            if (tasks[i].status == ReadyH && !tasks[i].io_tasks.empty())
                proc = i;
        }
    }
    return proc;
}

void check_idle(vector<Task> &tasks, int &cpu_stat)
{
    int avail_proc = 0;
    for (int i = 0; static_cast<unsigned int>(i) < tasks.size(); ++i)
    {
        if (tasks[i].start_time <= ::cur_time && tasks[i].status == ReadyL && !tasks[i].io_tasks.empty())
        {
            tasks[i].status = ReadyH;
            ++avail_proc;
        }
        if (tasks[i].status == ReadyH || tasks[i].status == Runned)
            ++avail_proc;
    }
    if (avail_proc == 0)
    {
        if (cpu_stat == Work)
        {
            cout << ::cur_time << " IDLE" << endl;
            cpu_stat = Idle;
        }
    }
    else
        cpu_stat = Work;
}

void check_io_procs(Task &task, int quant, int &runned_proc)
{
    if (task.status == Blocked)
    {
        for (int i = 1; static_cast<unsigned int>(i) < task.io_tasks.size(); ++i)
        {
            task.io_tasks[i].start_io = task.io_tasks[i].start_io - 1;
        }
        task.io_tasks[0].end_io = task.io_tasks[0].end_io - 1;
        task.end_time = task.end_time - 1;
    }
    if (!task.io_tasks.empty())
        if (task.io_tasks[0].end_io == 0)
        {
            task.io_tasks.erase(task.io_tasks.begin());
            task.status = ReadyL;
            task.quant_avail = quant - 1;
        }

    if (!task.io_tasks.empty() && task.status != Blocked)
        if (task.io_tasks[0].start_io == 0)
        {
            task.status = Blocked;
            runned_proc = -1;
        }
}

int check_runned_proc(Task &task, int quant, int &runned_proc, int const &i)
{
    if (task.status == Blocked)
        return runned_proc;

    if (task.status == Runned && task.quant_avail > 0)
    {
        for (int i = 0; static_cast<unsigned int>(i) < task.io_tasks.size(); ++i)
        {
            task.io_tasks[i].start_io = task.io_tasks[i].start_io - 1;
        }
        task.end_time = task.end_time - 1;
        task.quant_avail = task.quant_avail - 1;
        runned_proc = i;
    }
    if (task.status == Runned && task.quant_avail == 0 && task.io_tasks.empty())
    {
        task.status = ReadyL;
        runned_proc = -1;
    }
    if (task.status == Runned && task.quant_avail == 0 && !task.io_tasks.empty())
    {
        task.status = ReadyH;

    }
    if (!task.io_tasks.empty())
    {
        if ( runned_proc == -1 && task.status == ReadyH && task.io_tasks[0].start_io <= quant)
        {
            runned_proc = i;
            task.status = Runned;
            task.quant_avail = quant - 1;
            task.end_time = task.end_time; //SHIT
            cout << ::cur_time << " " << task.name << endl;
        }
    }
    else
    {
        if( runned_proc == -1 && task.status == ReadyH)
        {
            runned_proc = i;
            task.status = Runned;
            task.quant_avail = quant - 1;
            task.end_time = task.end_time - 1; //SHIT
            cout << ::cur_time << " " << task.name << endl;
        }
    }
    return runned_proc;
}

void check_finished_proc(Task &task, vector<Task> &tasks, int &i, int &runned_proc)
{
    if (task.end_time == 0)
    {
        tasks.erase(tasks.begin() + i);
        i = i - 1;
        runned_proc = -1;
    }
}

void select_new_task(vector<Task> &tasks, int &runned_proc, int quant)
{
    for (int i = 0; static_cast<unsigned int>(i) < tasks.size(); ++i)
    {
        if (tasks[i].start_time <= ::cur_time && tasks[i].status != Blocked)
        {
            runned_proc = i;
            tasks[i].status = Runned;
            tasks[i].quant_avail = quant;
            tasks[i].end_time = tasks[i].end_time; //SHIT
            cout << ::cur_time << " " << tasks[i].name << endl;
            break;
        }
    }
}

void main_cycle(int quant, vector<Task> &tasks, int &cpu_stat)
{
    int runned_proc = -1;
    for (int i = 0; static_cast<unsigned int>(i) < tasks.size(); i++)
    {
        check_runned_proc(tasks[i], quant, runned_proc, i);
        check_io_procs(tasks[i], quant, runned_proc);
        check_finished_proc(tasks[i], tasks, i, runned_proc);
    }
    if (runned_proc == -1)
        select_new_task(tasks, runned_proc, quant);
    check_idle(tasks, cpu_stat);
    ++::cur_time;
}

int main()
{
    int cpu_stat = Work;
    int quant = 0;
    vector<Task>tasks;
    cin >> quant;
    cin.ignore(256, '\n');
    for (string buffer; getline(cin, buffer);)
    {
        if (buffer == "")
            break;
        Task parsed_task = parse_input_process(buffer);
        tasks.push_back(parsed_task);
    }

    set_highest_priority(tasks);

    while ( !tasks.empty())
    {
        main_cycle(quant, tasks, cpu_stat);
    }
    return 0;
}


