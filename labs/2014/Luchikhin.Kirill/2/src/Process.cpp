#include "../include/Process.h"
#include <iostream>

using std::cout;
using std::endl;

void Process::addIO(io_ptr io)
{
    ios.push_back(io);
}

bool Process::hasIO(size_t c)
{
    if (!ios.empty()) return ios.front()->begin_time - real_duration <= c;

    return false;
}

bool Process::inQuantum(size_t c)
{
    return duration - real_duration <= c;
}

void Process::start(size_t& globalTime, size_t c)
{
    cout << globalTime << " " << name << endl;
    c = (duration - real_duration < c) ? duration - real_duration : c;

    if (!ios.empty()) {
        size_t timeBeforeIO = ios.front()->begin_time - real_duration;

        if (timeBeforeIO <= c) {
            globalTime += timeBeforeIO;
            begin_time = globalTime + ios.front()->duration + 1;
            real_duration += timeBeforeIO + ios.front()->duration;
            ios.pop_front();
        } else {
            globalTime += c;
            begin_time = globalTime + 1;
            real_duration += c;
        }
    } else {
        globalTime += c;
        begin_time = globalTime + 1;
        real_duration += c;
    }
}

bool Process::stop()
{
    return real_duration == duration;
}


