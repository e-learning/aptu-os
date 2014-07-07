#ifndef TASK_H
#define TASK_H

#include <iostream>
#include <sstream>
#include <vector>
#include <queue>

using std::string;
using std::priority_queue;

size_t timer = 0;
size_t quant;

struct IO {
    IO(size_t start_time, size_t exec_time)
        :start_time(start_time), exec_time(exec_time) {

    }

    size_t start_time;
    size_t exec_time;

    bool operator < (IO const & other) const {
        return start_time == other.start_time ? exec_time <= other.exec_time
                : start_time <= other.start_time;
    }
};

struct Task {

    string id;

    size_t start_time;
    size_t exec_time;
    size_t waiting_time;
    size_t start_io_time;

    priority_queue<IO> io;

    bool is_io_in_quant() const {
        return (start_time <= timer)
                && (io.size() == 0
                    ? false
                    : io.top().start_time - (exec_time - waiting_time) <= quant);
    }

    bool is_finish_in_quant() const {
        return start_time <= timer
                && waiting_time <= quant;
    }

    bool is_io() const {
        return io.size() == 0
                ? false
                : io.top().start_time == exec_time - waiting_time;
    }

    bool completed() const {
        return waiting_time == 0;
    }

    size_t io_finish_time() const {
        return start_io_time + io.top().exec_time;
    }

    void close_io() {
        waiting_time -= io.top().exec_time;
        io.pop();
    }

    void step(){
        --waiting_time;
    }
};


struct Task_Comparator {

    bool operator ()(const Task & that, const Task & other) const {
        if (that.is_io_in_quant()) {
            return false;
        } else if (other.is_io_in_quant()) {
            return true;
        }
        if (that.is_finish_in_quant()) {
            return false;
        } else if (other.is_finish_in_quant()) {
            return true;
        }
        return that.start_time > other.start_time;
    }

};

struct Task_IO_Comparator {

    bool operator ()(const Task & that, const Task & other) const {
        return that.io_finish_time() > other.io_finish_time();
    }

};



#endif // TASK_H