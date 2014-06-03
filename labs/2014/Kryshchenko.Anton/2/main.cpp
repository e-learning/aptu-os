#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include "task.h"

using std::string;
using std::vector;
using std::priority_queue;
using std::stringstream;
using std::cout;
using std::cin;
using std::endl;

extern size_t timer;
extern size_t quant;

priority_queue<Task, vector<Task>, Task_Comparator> task_queue;
priority_queue<Task, vector<Task>, Task_IO_Comparator> io_queue;

void clear_finished_io() {
    while(io_queue.size() && io_queue.top().io_finish_time() <= timer){
        Task t = io_queue.top();
        io_queue.pop();
        t.close_io();
        if (!t.completed()) {
            task_queue.push(t);
        }
    }
}

int main() {

    cin >> quant;
    //cin.ignore(255,'\n');

    while (cin.good()) {
        string cur_line;

        getline(cin, cur_line);
        if (!cur_line.empty()) {
            stringstream ss(cur_line);
            Task t;
            ss >> t.id >> t.start_time >> t.exec_time;
            t.waiting_time = t.exec_time;
            while (ss.good()) {
                size_t start;
                size_t end;
                ss >> start >> end;
                if (!ss.fail()) {
                    t.io.push(IO(start,end));
                }
            }
            task_queue.push(t);
        }

    }

    while(!task_queue.empty() || !io_queue.empty()) {
        if (task_queue.empty() || task_queue.top().start_time > timer) {
            cout << timer << " IDLE" << endl;
            while (task_queue.empty() || task_queue.top().start_time > timer){
                ++timer;
                clear_finished_io();
            }
        }

        Task t = task_queue.top();
        task_queue.pop();
        cout << timer << " " << t.id << endl;
        size_t quantum = quant;
        while(quantum-- && !t.completed() && !t.is_io() ) {
            t.step();
            ++timer;
        }

        if (t.is_io()){
            t.start_io_time = timer;
            io_queue.push(t);
        } else if (!t.completed()) {
            task_queue.push(t);
        }
        clear_finished_io();
    }

    return 0;
}