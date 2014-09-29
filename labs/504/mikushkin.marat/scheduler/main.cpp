#include <algorithm>
#include "process.h"

struct StartTimeCmp {
    bool operator() (Process & first, Process & second) {
        return first.start > second.start;
    }
};

struct TimeToRunCmp {
    bool operator() (Process & first, Process & second) {
        return first.timeToRun() > second.timeToRun();
    }
};

int quantLength, currentTime;
std::priority_queue<Process, std::vector<Process>, StartTimeCmp> waitingTasks;
std::priority_queue<Process, std::vector<Process>, TimeToRunCmp> readyTasks;

void readInput() {
    std::cin >> quantLength;

    std::string line;
    getline(std::cin, line);

    while(getline(std::cin, line)) {
        waitingTasks.push(Process(line));
    }

    currentTime = 0;
}

void processTask(Process & curTask) {
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

void makeMove() {
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
    curTask.remainingTime += activeTime;
    currentTime += activeTime;

    processTask(curTask);
}

int main() {
    readInput();
    while(!(waitingTasks.empty() && readyTasks.empty())) {
        makeMove();
    }
    return 0;
}
