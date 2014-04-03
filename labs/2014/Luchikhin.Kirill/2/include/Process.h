#ifndef PROCESS_H
#define PROCESS_H

#include "IO.h"

#include <string>
#include <deque>
#include <cstddef>
#include <tr1/memory>

using std::string;
using std::deque;
using std::tr1::shared_ptr;

struct Process {
    Process(string name, size_t begin_time, size_t duration):
        name(name),
        begin_time(begin_time),
        duration(duration),
        real_duration(0)
    {}

    void addIO(io_ptr io);

    void start(size_t& globalTime, size_t c);

    bool stop();

    bool inQuantum(size_t c);

    bool hasIO(size_t c);

    deque<io_ptr> ios;
    string name;
    size_t begin_time;
    size_t duration;
    size_t real_duration;
};

typedef shared_ptr<Process> process_ptr;

#endif // PROCESS_H
