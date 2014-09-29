#include "scheduler.h"
#include <sstream>

Process::Process(string const &input) : ioTime(0), totalTime(0), localTime(0)
{
    stringstream ss(input);
    ss >> name >> start >> length;
    int ioBeg, ioEnd;
    while (ss >> ioBeg >> ioEnd) {
        ios.push_back(std::make_pair(ioBeg, ioEnd));
    }
}

bool Process::operator<(Process const &p) const
{
    return start < p.start;
}

bool Process::compare() const
{
    if (totalTime + quant >= length)
        return true;
    if (!ios.empty() && totalTime + quant >= ios.front().first)
        return true;
    return false;
}

bool Process::updateIO()
{
    ioTime++;
    totalTime++;
    bool flag = false;
    if (!ios.empty() && ioTime >= ios.front().second) {
        flag = true;
        ios.pop_front();
        ioTime = 0;
    }
    return flag;
}

int Process::updateWorking()
{
    totalTime++;
    localTime++;
    if (totalTime == length) {
        localTime = 0;
        return 3;
    }
    if (!ios.empty() && totalTime == ios.front().first) {
        localTime = 0;
        return 2;
    }
    if (localTime == quant) {
        localTime = 0;
        return 1;
    }
    return 0;
}

void Scheduler::processNonStarted()
{
    deque <Process>::iterator it = nonstarted.begin();
    while (it != nonstarted.end())
    {
        if (it->start > timer)
            break;
        it++;
    }
    std::copy(nonstarted.begin(), it, back_inserter(waiting));
    nonstarted.erase(nonstarted.begin(), it);
}

void Scheduler::processIO()
{
    vector <Process>::iterator it = remove_if(
        ioWaiting.begin(), ioWaiting.end(), std::mem_fun_ref(&Process::updateIO));
    std::copy(it, ioWaiting.end(), back_inserter(waiting));
    ioWaiting.erase(it, ioWaiting.end());
}

void Scheduler::processCurrent()
{
    if (current_process == NULL)
        return;

    switch (current_process->updateWorking()) {
        case 0:
            break;
        case 1:
            waiting.push_front(*current_process);
            delete current_process;
            current_process = NULL;
            break;
        case 2:
            ioWaiting.push_back(*current_process);
            delete current_process;
            current_process = NULL;
            break;
        case 3:
            finished.push_back(*current_process);
            delete current_process;
            current_process = NULL;
            break;
    }
}

void Scheduler::processWaiting()
{
    if (current_process != NULL || waiting.empty())
        return;

    stable_partition(
        waiting.begin(), waiting.end(), std::mem_fun_ref(&Process::compare));
    current_process = new Process(waiting.front());
    waiting.pop_front();
}

void Scheduler::printChanges()
{
    if (finished.size() == processCount)
        return;
    string cur = current_process == NULL ? "IDLE" : current_process->name;
    if (cur != prevPrintedProcess || prevPrintedTime + Process::quant == timer) {
        cout << timer << " " << cur << endl;
        prevPrintedProcess = cur;
        prevPrintedTime = timer;
    }
}
