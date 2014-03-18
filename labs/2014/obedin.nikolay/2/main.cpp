#include <climits>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

typedef unsigned int uint;

uint quantum;

struct io_operation {
    uint start_time;
    uint duration;

    io_operation()
        : start_time(UINT_MAX), duration(0) {}

    io_operation(uint s, uint d)
        : start_time(s), duration(d) {}

    friend bool operator<(const io_operation &fst, const io_operation &snd)
        { return fst.start_time > snd.start_time; }
};

struct process {
    string id;
    vector<io_operation> io_operations;

    uint time_passed = 0;
    uint activation_time;
    uint duration;

    bool used_full_quantum = false;

    process()
        : id(""), activation_time(UINT_MAX), duration(0) {}

    process(const string &id, uint s, uint d)
        : id(id), activation_time(s), duration(d) {}

    bool is_done() const
        { return duration <= time_passed; }

    bool is_available_at(uint time) const
        { return time >= activation_time; }

    uint priority() const
        { return (2 * has_io_operation()) + used_full_quantum; }

    const io_operation &current_io() const
        { return io_operations.back(); }

    bool has_io_operation() const
        { return !io_operations.empty()
            && current_io().start_time <= time_passed + quantum; }

    io_operation pop_current_io()
    {
        io_operation r = current_io();
        io_operations.pop_back();
        return r;
    }

    friend bool operator<(const process &fst, const process &snd)
        { return fst.activation_time >= snd.activation_time; }

    struct priority_comparator {
        bool operator()(const process &fst, const process &snd)
            { return fst.priority() < snd.priority(); }
    };
};

istream &operator>>(istream &in, io_operation &op)
{
    return in >> op.start_time >> op.duration;
}

istream &operator>>(istream &in, process &proc)
{
    in >> proc.id >> proc.activation_time >> proc.duration;
    string line;
    getline(in, line);
    istringstream isn(line);
    proc.io_operations.clear();
    proc.io_operations.assign
        (istream_iterator<io_operation>(isn)
        ,istream_iterator<io_operation>());
    sort(proc.io_operations.begin(), proc.io_operations.end());
    return in;
}

int main(int argc, const char *argv[])
{
    cin >> quantum;

    priority_queue<process> process_queue
        ((istream_iterator<process>(cin))
        ,istream_iterator<process>());

    uint time = 0;

    while (!process_queue.empty()) {
        vector<process> matches;
        while (!process_queue.empty()
                    && process_queue.top().is_available_at(time)) {
            matches.push_back(process_queue.top());
            process_queue.pop();
        }

        if (matches.empty()) {
            cout << time << " IDLE" << endl;
            time = process_queue.top().activation_time;
            continue;
        }

        stable_sort(matches.begin(), matches.end(),
                process::priority_comparator());
        process match = matches.back();
        matches.pop_back();
        for (const process &p: matches)
            process_queue.push(p);

        cout << time << " " << match.id << endl;

        if (match.has_io_operation()) {
            io_operation op = match.pop_current_io();
            uint dt = op.start_time - match.time_passed;

            time += dt;
            match.activation_time = time + op.duration;
            match.time_passed += (dt + op.duration);
            match.used_full_quantum = dt < quantum;

            process_queue.push(match);
        } else {
            uint dt = min(quantum, match.duration - match.time_passed);
            time += dt;
            match.time_passed += dt;

            if (!match.is_done()) {
                match.activation_time = time;
                match.used_full_quantum = true;
                process_queue.push(match);
            }
        }
    }

    return 0;
}

