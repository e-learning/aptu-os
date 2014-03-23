#include <iostream>
#include <sstream>
#include <vector>
#include <queue>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;
using std::vector;
using std::priority_queue;

size_t timer = 0;
size_t quantum_size;

struct IO {
    IO(size_t start_time, size_t run_time)
        :start_time(start_time), execution_time(run_time) {}

    size_t start_time;
    size_t execution_time;

    bool operator<(const IO & other) const {
        return start_time == other.start_time
                ? execution_time <= other.execution_time
                : start_time <= other.start_time;
    }
};

struct Process {

    string id;

    size_t start_time;
    size_t execution_time;
    size_t remaining_time;
    size_t io_start_time;

    priority_queue<IO> io;

    bool has_io_within_quant() const {
        return (start_time <= timer)
                && (io.size() == 0
                    ? false
                    : io.top().start_time <= quantum_size);
    }

    bool stops_within_quant() const {
        return start_time <= timer
                && remaining_time <= quantum_size;
    }

    bool needs_io() const {
        return io.size() == 0
                ? false
                : io.top().start_time == execution_time - remaining_time;
    }

    bool completed() const {
        return remaining_time == 0;
    }

    size_t io_end_time() const {
        return io_start_time + io.top().execution_time;
    }

    void close_io() {
        remaining_time -= io.top().execution_time;
        io.pop();
    }

};


struct ProcessComparator {

    bool operator ()(const Process & that, const Process & other) const {
        if (that.has_io_within_quant()) {
            return false;
        } else if (other.has_io_within_quant()) {
            return true;
        }
        if (that.stops_within_quant()) {
            return false;
        } else if (other.stops_within_quant()) {
            return true;
        }
        return that.start_time > other.start_time;
    }

};

struct ProcessIOComparator {

    bool operator ()(const Process & that, const Process & other) const {
        return that.io_end_time() > other.io_end_time();
    }

};

priority_queue<Process, vector<Process>, ProcessComparator> process_queue;
priority_queue<Process, vector<Process>, ProcessIOComparator> io_queue;

void flush_io_queue() {
    while(io_queue.size() && io_queue.top().io_end_time() <= timer){
        Process p = io_queue.top();
        io_queue.pop();
        p.close_io();
        if (!p.completed()) {
            process_queue.push(p);
        }
    }
}

int main() {

    cin >> quantum_size;
    cin.ignore(255,'\n');

    while (cin.good()) {

        string line;
        getline(cin, line);
        if (line.empty()) {
            continue;
        }
        stringstream ss(line);

        Process p;
        ss >> p.id
           >> p.start_time
           >> p.execution_time;
        p.remaining_time = p.execution_time;
        while (ss.good()) {
            size_t a, b;
            ss >> a >> b;
            if (!ss.fail()) {
                p.io.push(IO(a,b));
            }
        }
        process_queue.push(p);
    }

    while(!process_queue.empty() || !io_queue.empty()) {

        if (process_queue.empty()
                || process_queue.top().start_time > timer) {
            cout << timer << " IDLE" << endl;
            while (process_queue.empty()
                   || process_queue.top().start_time > timer){
                timer++;
                flush_io_queue();
            }
        }

        //run quantum

        Process p = process_queue.top();
        process_queue.pop();
        cout << timer << " " << p.id << endl;
        size_t quantum = quantum_size;
        while(quantum-- && !p.completed() && !p.needs_io() ) {
            p.remaining_time--;
            timer++;
        }

        if (p.needs_io()){
            p.io_start_time = timer;
            io_queue.push(p);
        } else if (!p.completed()) {
            process_queue.push(p);
        }
        flush_io_queue();
    }

    return 0;
}
