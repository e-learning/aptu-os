#include "../include/Scheduler.h"

#include <deque>
#include <algorithm>
#include <iostream>

using std::sort;
using std::deque;
using std::cout;
using std::endl;

void Scheduler::startScheduler(deque<process_ptr>& processes, size_t c)
{
    size_t globalTime = 0;
    deque<process_ptr> fp;
    deque<process_ptr> sp;
    bool prevIDLE = false;

    for(deque<process_ptr>::iterator it = processes.begin(); it != processes.end(); ++it)
        sort((*it)->ios.begin(), (*it)->ios.end());

    while(!fp.empty() || !sp.empty() || !processes.empty()) {


        for(int i = sp.size(); i > 0 && !sp.empty(); --i) {
            if (sp.front()->inQuantum(c) || sp.front()->hasIO(c))
                fp.push_back(sp.front());
            else
                sp.push_back(sp.front());

            sp.pop_front();
        }

        for(int i = processes.size(); i > 0 && !processes.empty(); --i) {
            if (processes.front()->begin_time <= globalTime) {
                if (processes.front()->inQuantum(c) || processes.front()->hasIO(c))
                    fp.push_back(processes.front());
                else
                    sp.push_back(processes.front());
            } else {
                processes.push_back(processes.front());
            }

            processes.pop_front();
        }

        if (!fp.empty()) {
            fp.front()->start(globalTime, c);

            if(!fp.front()->stop()) processes.push_back(fp.front());

            fp.pop_front();
            prevIDLE = false;
            continue;
        } else if (!sp.empty()) {
            sp.front()->start(globalTime, c);

            if(!sp.front()->stop()) processes.push_back(sp.front());

            sp.pop_front();
            prevIDLE = false;
            continue;
        }

        if (fp.empty() && sp.empty()) {
            if (!prevIDLE) {
                if (!processes.empty()) {
                    cout << globalTime << " " << "IDLE" << endl;
                    prevIDLE = true;
                }
            }

            ++globalTime;
        }
    }
}
