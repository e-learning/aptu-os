#include "scheduler.h"

Scheduler::Scheduler() {
    ;
}

void Scheduler::processTask(Process & curTask) {
    if(curTask.timeToRun() > 0) {
        readyTasks.push(curTask);
        return;
    }

    if(!curTask.iops.empty()) {
        int ioLength = curTask.iops.front().second;
        curTask.time_runned += ioLength;
        curTask.start = currentTime + ioLength;

        curTask.iops.pop();

        waitingTasks.push(curTask);
    }
}

void Scheduler::makeMove() {
    while(!waitingTasks.empty() && waitingTasks.top().start <= currentTime) {
        readyTasks.push(waitingTasks.top());
        waitingTasks.pop();
    }

    if(readyTasks.empty()) {
        std::cout << currentTime << " IDLE" << std::endl;
        currentTime = waitingTasks.top().start;
        return;
    }

    Process curTask = readyTasks.top();
    readyTasks.pop();

    std::cout << currentTime << " " << curTask.name << std::endl;

    int activeTime = std::min(quantLength, curTask.timeToRun());
    curTask.time_runned += activeTime;
    currentTime += activeTime;

    processTask(curTask);
}

void Scheduler::readInput() {
    std::cin >> quantLength;

    std::string line;
    getline(std::cin, line);

    while(getline(std::cin, line)) {
        waitingTasks.push(Process(line));
    }

    currentTime = 0;
}
