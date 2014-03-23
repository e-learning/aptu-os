#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <iostream>
#include "process.h"

class ProcStartTimeLess {
public:
    bool operator() (Process const& p1, Process const& p2) {
        return p1.start_time() > p2.start_time();
    }
};

class ReadyProcLess {
public:
    ReadyProcLess(size_t time_slot)
        : p_time_slot(time_slot) {}

    bool operator() (Process const& p1, Process const& p2) {
        if((p2.RemainingTime() <= p_time_slot || p2.HasIOWithinTimeSlot(p_time_slot)) &&
           (p1.RemainingTime() <= p_time_slot || p1.HasIOWithinTimeSlot(p_time_slot))) {
            return p2.id() < p1.id();
        }
        if(p2.RemainingTime() <= p_time_slot || p2.HasIOWithinTimeSlot(p_time_slot)) {
            return true;
        }
        if(p1.RemainingTime() <= p_time_slot || p1.HasIOWithinTimeSlot(p_time_slot)) {
            return false;
        }
        if(p2.id() < p1.id()) {
            return true;
        }
        return false;
    }

private:
    size_t p_time_slot;
};

class Scheduler {
public:
    Scheduler(size_t time_slot, std::vector<Process> const& processes)
        : p_time_slot(time_slot)
        , p_all_procs(processes.begin(), processes.end())
        , p_ready_procs(ReadyProcLess(p_time_slot))
    {}

    void Start() {
        size_t step = 0;
        while(!p_all_procs.empty() || !p_ready_procs.empty() || !p_blocked_procs.empty()) {
            RefreshReadyProcQueue(step);
            PeekNextActive(step);
            ++step;
        }
    }

private:
    typedef std::priority_queue<Process, std::vector<Process>, ProcStartTimeLess> ProcQueue;
    typedef std::priority_queue<Process, std::vector<Process>, ReadyProcLess> ReadyProcQueue;

    size_t p_time_slot;
    ProcQueue p_all_procs;
    ReadyProcQueue p_ready_procs;
    ProcQueue p_blocked_procs;

    void RefreshReadyProcQueue(size_t current_step) {
        while(!p_all_procs.empty() && p_all_procs.top().start_time() <= current_step) {
            p_ready_procs.push(p_all_procs.top());
            p_all_procs.pop();
        }
        while (!p_blocked_procs.empty() && p_blocked_procs.top().start_time() <= current_step) {
            p_ready_procs.push(p_blocked_procs.top());
            p_blocked_procs.pop();
        }
    }

    void PeekNextActive(size_t& current_step) {
        if(p_ready_procs.empty()) {
            std::cout << current_step << ' ' << "IDLE" << std::endl;
            return;
        }
        Process proc = p_ready_procs.top();
        p_ready_procs.pop();
        std::cout << current_step << ' ' << proc.id() << std::endl;
        if(proc.HasIOWithinTimeSlot(p_time_slot)) {
            size_t time_to_blocking = proc.TimeToBlocking();
            proc.StartIO(current_step);
            current_step += (time_to_blocking - 1);
            p_blocked_procs.push(proc);
        }
        else if(proc.RemainingTime() <= p_time_slot) {
            current_step += proc.RemainingTime() - 1;
        }
        else {
            proc.WorkOffTimeSlot(current_step, p_time_slot);
            p_ready_procs.push(proc);
            current_step += (p_time_slot - 1);
        }
    }
};

#endif // SCHEDULER_H
