#ifndef TASK_H
#define TASK_H

#include <deque>
#include <string>

using std::string;
using std::deque;
using std::pair;

enum TaskEvent {
    OK,
    QUANT_OVER,
    IO,
    FINISHED
};

class task
{
public:
    task(string & str);
    bool update_IO_time();
    TaskEvent update_working_time();
    bool compair_priority() const;

    bool operator<(task const & t) const {
        return start < t.start;
    }


    static int quant;
    int start;
    string name;


private:
    int lenght;
    int io_time;
    int total_time;
    int local_time;
    deque< pair<int, int> > io_blocks;

};

#endif // TASK_H
