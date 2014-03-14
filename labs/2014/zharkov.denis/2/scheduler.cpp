#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <deque>
#include <iterator>

using namespace std;

struct ExecutionResult {
    bool interrupted;
    int used_time;
    int last_io_details;
};

struct Process {
    bool operator<(Process const & other) const {
        return activates_at < other.activates_at || (activates_at == other.activates_at && pid < other.pid);
    }

    bool will_freeze_within_time(int time) {
        int will_be = executed + time;

        if (will_be >= duration) {
            return true;
        }
        
        return !ios.empty() && will_be >= ios.front().first;
    }

    ExecutionResult execute(int quantum_length) {
        int will_be = executed + quantum_length;
        
        ExecutionResult result { false, quantum_length, -1 };

        if (!ios.empty() && will_be >= ios.front().first) {
            result.interrupted = true;
            result.used_time = ios.front().first - executed;     
            result.last_io_details = ios.front().second;

            executed = ios.front().first;
        } else if (will_be >= duration) {
            result.used_time = quantum_length - (will_be - duration);
            result.interrupted = true;
            
            executed = duration;
        } else {
            executed = will_be;
        }

        return result;
    }

    void accept_io_result() {
        executed += ios.front().second;
        ios.pop();
    }

    bool done() {
        return executed == duration;
    }

    void acquire_pid() {
        pid = pid_counter++;
    }

    string name;
    int activates_at;
    int duration;

    queue<pair<int,int>> ios;
    
    int pid;
    
    int executed = 0;
    static int pid_counter;
};

int Process::pid_counter = 0;

istream & operator >> (istream & in, Process & result) {
    
    if (!(in >> result.name >> result.activates_at >> result.duration)) {
        return in;
    }
    
    result.acquire_pid();

    int from,to;
    
    string line;
    if (!getline(in, line)) {
        return in; 
    }

    istringstream ss(line);
    while (!result.ios.empty()) {
        result.ios.pop();
    }

    while (ss >> from >> to) {
        result.ios.push(make_pair(from, to));
    }

    return in;
}


struct Event {
    enum Type {
        IO_RESULT,
        ACTIVATION
    };
    

    Type type;
    int pid;
    int when;

    bool operator>(Event const & other) const {
        return when > other.when || (when == other.when && pid > other.pid);
    }
};


int main() {
    freopen("input.txt", "r", stdin);
    int quantum_length;
    cin >> quantum_length;

    vector<Process> processes {
        istream_iterator<Process>{cin},
        istream_iterator<Process>{}
    };

    sort(processes.begin(), processes.end());

    queue<int> io_level;
    queue<int> second_level;

    priority_queue<Event, vector<Event>, greater<Event>> events;
    
    for (auto & process : processes) {
        events.push({Event::ACTIVATION, process.pid, process.activates_at});        
    }
    
    int current_time = 0;

    while (!events.empty() || !io_level.empty() || !second_level.empty()) {
        while (!events.empty() && current_time >= events.top().when) {
            Event evnt = events.top();
            events.pop();
            
            Process & process = processes[evnt.pid];
            
            if (evnt.type == Event::IO_RESULT) {
                process.accept_io_result(); 
            }

            if (process.will_freeze_within_time(quantum_length)) {
                io_level.push(evnt.pid);
            } else {
                second_level.push(evnt.pid);
            }
        }
        
        queue<int> & current_queue = !io_level.empty() ? io_level : second_level;
        
        cout<< current_time << " ";

        if (current_queue.empty()) {
            cout<< "IDLE" << endl; 
            current_time = events.top().when;
            continue;
        } 

        int pid = current_queue.front();
        Process & process = processes[pid];
        current_queue.pop();
        
        cout << process.name << endl;
       
        ExecutionResult last_execution = process.execute(quantum_length);
        current_time += last_execution.used_time;

        if (!last_execution.interrupted) {
            if (process.will_freeze_within_time(quantum_length)) {
                io_level.push(pid);
            } else {
                second_level.push(pid);
            }
        } else {
            if (!process.done()) {
                //io
                events.push({Event::IO_RESULT, pid, current_time + last_execution.last_io_details});
            }
        }
    }

    return 0;
}
