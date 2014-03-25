#ifndef SHEDULER_H
#define SHEDULER_H

#include <deque>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include "task.h"

typedef std::shared_ptr<task> task_ptr;


class sheduler
{
public:
    sheduler(): timer(0), task_count(0),
        prev_task(""),prev_task_time(0) {}

    void add_task(task const & t) {
        unstarted.push_back(t);
        ++task_count;
    }

    void start();

private:

    void task_unstarted();
    void task_IO();
    void task_current();
    void task_waiting();
    void print_current_result();

    bool timer_compair(task const & t) {
        return t.start > timer;
    }

    task_ptr current_task;
    std::deque<task> unstarted;
    std::deque<task> finished;
    std::deque<task> waiting;
    std::vector<task> io_waiting;
    int timer;
    int task_count;
    string prev_task;
    int prev_task_time;


};

#endif // SHEDULER_H
