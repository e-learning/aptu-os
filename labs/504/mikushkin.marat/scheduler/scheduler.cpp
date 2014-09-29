#include "scheduler.h"

Scheduler::Scheduler() {
    ;
}

void Scheduler::sendToProcessor(Process & currentProcess) {
    if(currentProcess.timeToRun() > 0) {
        readyTasks.push(currentProcess);
        return;
    }

    if(!currentProcess.ioOperations.empty()) {
        int ioLength = currentProcess.ioOperations.front().second;
        currentProcess.remainingTime += ioLength;
        currentProcess.start = currentTime + ioLength;

        currentProcess.ioOperations.pop();

        waitingTasks.push(currentProcess);
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

    sendToProcessor(curTask);
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
