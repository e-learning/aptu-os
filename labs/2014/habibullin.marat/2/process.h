#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <queue>
#include <iostream>
#include <sstream>

struct IOOperation {
    size_t start_time;
    size_t duration;

    IOOperation & operator=(IOOperation const& other) {
        start_time = other.start_time;
        duration = other.duration;
        return *this;
    }
};

class Process {
public:
    std::string const& id() const {
        return p_id;
    }

    size_t start_time() const {
        return p_start_time;
    }

    void StartIO(size_t current_step) {
        p_start_time = current_step + TimeToBlocking() + TopIODuration();
        p_worked_time += (TimeToBlocking() + TopIODuration());
        PopIOOperation();
    }

    void WorkOffTimeSlot(size_t current_step, size_t time_slot) {
        p_start_time = current_step + time_slot;
        p_worked_time += time_slot;
    }

    size_t RemainingTime() const {
        return p_duration - p_worked_time;
    }

    bool HasIOWithinTimeSlot(size_t time_slot) const {
        if(!p_io_operations.empty()) {
            return TopIOStartTime() - p_worked_time <= time_slot;
        }
        return false;
    }

    size_t TimeToBlocking() const {
        return TopIOStartTime() - p_worked_time;
    }

    static Process FromString(std::string str) {
        std::stringstream sin(str);
        Process proc;
        sin >> proc.p_id;
        sin >> proc.p_start_time;
        sin >> proc.p_duration;
        proc.p_worked_time = 0;
        IOOperation io;
        while(sin >> io.start_time) {
            sin >> io.duration;
            proc.p_io_operations.push(io);
        }
        return proc;
    }

private:
    IOOperation PopIOOperation() {
        IOOperation tmp = p_io_operations.front();
        p_io_operations.pop();
        return tmp;
    }

    size_t const& TopIOStartTime() const {
        return p_io_operations.front().start_time;
    }

    size_t const& TopIODuration() const {
        return p_io_operations.front().duration;
    }

    std::string p_id;
    size_t p_start_time;
    size_t p_duration;
    size_t p_worked_time;
    std::queue<IOOperation> p_io_operations;
};

#endif // PROCESS_H
