#include "scheduler.h"

Scheduler::Scheduler() {
    ;
}

void Scheduler::processTask(Process & curTask) {
    if(curTask.timeToRun() > 0) {
        readyTasks.push(curTask);
        return;
    }

    if(!curTask.ioOperations.empty()) {
        int ioLength = curTask.ioOperations.front().second;
        curTask.remainingTime += ioLength;
        curTask.start = currentTime + ioLength;

        curTask.ioOperations.pop();

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

    int activeTime = std::min(processorUnit, curTask.timeToRun());
    curTask.remainingTime += activeTime;
    currentTime += activeTime;

    processTask(curTask);
}

void Scheduler::init() {
    std::cin >> processorUnit;

    std::string line;
    getline(std::cin, line);

    while(getline(std::cin, line)) {
        waitingTasks.push(Process(line));
    }

    currentTime = 0;
}
