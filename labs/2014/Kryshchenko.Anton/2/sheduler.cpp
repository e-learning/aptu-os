#include "sheduler.h"
#include <algorithm>
#include <iostream>
#include <functional>


void sheduler::start() {
    sort(unstarted.begin(),unstarted.end());

    while(finished.size() < task_count) {
        task_unstarted();
        task_IO();
        task_current();
        task_waiting();
        print_current_result();
        ++timer;
    }
}

void sheduler::task_unstarted() {

    std::deque<task>::iterator it = std::find_if(unstarted.begin(), unstarted.end(),
                           [&](task const & t) {return t.start > timer; });
    std::copy(unstarted.begin(), it, back_inserter(waiting));
    unstarted.erase(unstarted.begin(), it);
}

void sheduler::task_IO() {

    std::vector<task>::iterator it = std::remove_if(io_waiting.begin(), io_waiting.end(),
                                                    std::mem_fun_ref(&task::update_IO_time));
    std::copy(it, io_waiting.end(), back_inserter(waiting));
    io_waiting.erase(it, io_waiting.end());

}

void sheduler::task_waiting() {

    if(current_task != NULL || waiting.empty()) {
        return;
    }

    std::stable_partition(waiting.begin(), waiting.end(),
                          std::mem_fun_ref(&task::compair_priority));
    current_task = task_ptr(new task(waiting.front()));
    waiting.pop_front();
}

void sheduler::task_current() {

    if(current_task == NULL) {
        return;
    }

    switch(current_task->update_working_time()) {
        case OK:
            break;
        case QUANT_OVER:
            waiting.push_front(*current_task);
            current_task == NULL;
            break;
        case IO:
            io_waiting.push_back(*current_task);
            current_task = NULL;
            break;
        case FINISHED:
        finished.push_back(*current_task);
        current_task = NULL;
        break;


    }
}

void sheduler::print_current_result() {

    if(finished.size() == task_count) {
        return;
    }

    std::string current_task_name = "";
    if(current_task == NULL){
        current_task_name = "IDLE";

    } else {
        current_task_name = current_task->name;
    }

    if(current_task_name != prev_task ||
            prev_task_time + task::quant == timer) {
        std::cout<< timer << " " << current_task_name << std::endl;
        prev_task = current_task_name;
        prev_task_time = timer;
    }
}
