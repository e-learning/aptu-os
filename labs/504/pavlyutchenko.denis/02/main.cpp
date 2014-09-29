#include <iostream>
#include <vector>
#include <sstream>
#include <queue>
#include "Task.cpp"

using namespace std;

struct MoreThanByTi
{
    bool operator()(const Task& lhs, const Task& rhs) const
    {
        return lhs.get_Ti() > rhs.get_Ti();
    }
};

struct MoreThanByLeftTime
{
    bool operator()(const Task& lhs, const Task& rhs) const
    {
        return lhs.left_time() > rhs.left_time();
    }
};

int main() {
    const string IDLE_MESSAGE = "IDLE";

    int C;
    cin >> C;

    int current_time = 0;

    std::priority_queue<Task, std::vector<Task>, MoreThanByTi> tasks_with_start_time_priority;
    std::priority_queue<Task, std::vector<Task>, MoreThanByLeftTime> tasks_with_left_time_priority;

    string line;
    getline(cin, line);

    while (!getline(cin, line).eof())
    {
        if (line == "") {
            break;
        }

        Task task(line, C);
        tasks_with_start_time_priority.push(task);
    }

    while (!tasks_with_start_time_priority.empty() || !tasks_with_left_time_priority.empty()) {

        while(!tasks_with_start_time_priority.empty())  {
            int start_time = tasks_with_start_time_priority.top().get_Ti();

            if (start_time > current_time) {
                break;
            }

            Task t = tasks_with_start_time_priority.top();
            tasks_with_start_time_priority.pop();

            tasks_with_left_time_priority.push(t);
        }

        if(tasks_with_left_time_priority.empty()) {
            cout << current_time << " " << IDLE_MESSAGE << endl;

            int start_time = tasks_with_start_time_priority.top().get_Ti();
            current_time = start_time;

            continue;
        }

        Task task = tasks_with_left_time_priority.top();
        tasks_with_left_time_priority.pop();

        int worked_time = task.run(current_time);
        cout << task << endl;

        current_time += worked_time;

        if(task.left_time() > 0) {
            tasks_with_left_time_priority.push(task);
            continue;
        }

        if(!task.get_iops().empty()) {
            task.recalculate_Ti_and_worked_time(current_time);
            tasks_with_start_time_priority.push(task);
        }
    }

    return 0;
}