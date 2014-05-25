#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

struct Task {
    string name;
    int start, end;
    queue< pair<int, int> > iops;
    int time_runned;

    Task(string line) : time_runned(0) {
        stringstream ss(line);

        ss >> name;
        ss >> start >> end;

        int io_start, io_len;
        while(ss >> io_start >> io_len) {
            iops.push(pair<int,int>(io_start, io_len));
        }
    }

    int timeToRun() const {
        if(!iops.empty()) {
            return iops.front().first - time_runned;
        } else {
            return end - time_runned;
        }
    }
};

struct StartTimeCmp {
    bool operator() (Task const & first, Task const & second) {
        return first.start > second.start;
    }
};

struct TimeToRunCmp {
    bool operator() (Task const & first, Task const & second) {
        return first.timeToRun() > second.timeToRun();
    }
};

int quantLength, currentTime;
priority_queue<Task, vector<Task>, StartTimeCmp> waitingTasks;
priority_queue<Task, vector<Task>, TimeToRunCmp> readyTasks;

void readInput() {
    cin >> quantLength;

    string line;
    getline(cin, line);

    while(getline(cin, line)) {
        waitingTasks.push(Task(line));
    }

    currentTime = 0;
}

void processTask(Task & curTask) {
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

void makeMove() {
    while(!waitingTasks.empty() && waitingTasks.top().start <= currentTime) {
        readyTasks.push(waitingTasks.top());
        waitingTasks.pop();
    }

    if(readyTasks.empty()) {
        cout << currentTime << " IDLE" << endl;
        currentTime = waitingTasks.top().start;
        return;
    }

    Task curTask = readyTasks.top();
    readyTasks.pop();

    cout << currentTime << " " << curTask.name << endl;

    int activeTime = min(quantLength, curTask.timeToRun());
    curTask.time_runned += activeTime;
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
