#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

struct Process {
    string name;
    long startTime, endTime;
    queue< pair<int, int> > IOs;
    long elapsedTime;
    Process(string line){
        elapsedTime = 0;
        stringstream ss(line);
        ss >> name;
        ss >> startTime >> endTime;
        int io_start;
        int io_len;
        while(ss >> io_start >> io_len) {
            IOs.push(make_pair(io_start, io_len));
        }
    }
    int timeToRun() const {
        if(!IOs.empty()) return IOs.front().first - elapsedTime;
        else return (endTime - elapsedTime);        
    }
};

struct startComparator {
    bool operator() (Process const & first, Process const & second) {
        return first.startTime > second.startTime;
    }
};

struct timeToRunComparator {
    bool operator() (Process const & first, Process const & second) {
        return first.timeToRun() > second.timeToRun();
    }
};

int quantumLength, currentTime;
priority_queue<Process, vector<Process>, timeToRunComparator> completed;
priority_queue<Process, vector<Process>, startComparator> pending;

void readInput() {
    cin >> quantumLength;
    string line;
    getline(cin, line);
    while(getline(cin, line)) pending.push(Process(line));
    currentTime = 0;
}

void executeProcess(Process & currentProcess) {
    if(currentProcess.timeToRun() > 0) {
        completed.push(currentProcess);
        return;
    }

    if(!currentProcess.IOs.empty()) {
        int ioLength = currentProcess.IOs.front().second;
        currentProcess.elapsedTime += ioLength;
        currentProcess.startTime = currentTime + ioLength;
        currentProcess.IOs.pop();
        pending.push(currentProcess);
    }
}

void makeMove() {
    while(!pending.empty() && pending.top().startTime <= currentTime)  {
        completed.push(pending.top());
        pending.pop();
    }

    if(completed.empty()) {
        cout << currentTime << " IDLE" << endl;
        currentTime = pending.top().startTime;
        return;
    }

    Process currentProcess = completed.top();
    completed.pop();
    cout << currentTime << " " << currentProcess.name << endl;
    int activeTime = min(quantumLength, currentProcess.timeToRun());
    currentProcess.elapsedTime += activeTime;
    currentTime += activeTime;
    executeProcess(currentProcess); 
}

int main() {
    readInput();
    while(!pending.empty() || !completed.empty()){
        makeMove();
    }
    return 0;
}
