#include "scheduler.h"

Scheduler::Scheduler() {
    ;
}

void Scheduler::sendToProcessor(Process & currentProcess) {
    if(currentProcess.timeToRun() > 0) {
        readyProcesses.push(currentProcess);
        return;
    }

    if(!currentProcess.ioOperations.empty()) {
        int ioLength = currentProcess.ioOperations.front().second;
        currentProcess.remainingTime += ioLength;
        currentProcess.start = currentTime + ioLength;

        currentProcess.ioOperations.pop();

        waitingProcesses.push(currentProcess);
    }
}

void Scheduler::update() {
    while(!waitingProcesses.empty() && waitingProcesses.top().start <= currentTime) {
        readyProcesses.push(waitingProcesses.top());
        waitingProcesses.pop();
    }

    if(readyProcesses.empty()) {
        std::cout << currentTime << " IDLE" << std::endl;
        currentTime = waitingProcesses.top().start;
        return;
    }

    Process currentProcess = readyProcesses.top();
    readyProcesses.pop();

    std::cout << currentTime << " " << currentProcess.name << std::endl;

    int activeTime = std::min(processorUnit, currentProcess.timeToRun());
    currentProcess.remainingTime += activeTime;
    currentTime += activeTime;

    sendToProcessor(currentProcess);
}

void Scheduler::init() {
    std::cin >> processorUnit;

    std::string line;
    getline(std::cin, line);

    while(getline(std::cin, line)) {
        waitingProcesses.push(Process(line));
    }

    currentTime = 0;
}
