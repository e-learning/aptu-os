#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <deque>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class Process
{
public:
    static int quant;
    int start;
    string name;
    Process(string const &input);
    bool operator<(Process const &p) const;
    bool compare() const;
    bool updateIO();
    int updateWorking();
private:
    int length;
    int ioTime;
    int totalTime;
    int localTime;
    deque< pair<int, int> > ios;
};

class Scheduler
{
private:
    int quant;
    int timer;
    size_t processCount;
    std::string prevPrintedProcess;
    int prevPrintedTime;

    deque<Process> nonstarted;
    deque<Process> waiting;
    vector<Process> finished;
    vector<Process> ioWaiting;
    Process *current_process;

    void processNonStarted();
    void processIO();
    void processCurrent();
    void processWaiting();
    void printChanges();

public:
    Scheduler(int quant) : timer(0), processCount(0), prevPrintedProcess(""), prevPrintedTime(0)
    {
        Process::quant = quant;
        current_process = NULL;
    }

    void add(string input)
    {
        Process p(input);
        nonstarted.push_back(p);
        processCount++;
    }

    void proceed()
    {
        sort(nonstarted.begin(), nonstarted.end());

        while (finished.size() < processCount) {
            processNonStarted();
            processIO();
            processCurrent();
            processWaiting();
            printChanges();

            timer++;
        }
    }
};

#endif // SCHEDULER_H
