#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

int quantum;

struct process {
  std::string id;
  int time_input;
  int time_work;
  int time_completed;
  std::vector<std::pair<int, int> > io_blocks;
};

void input_next_process(process &process, std::string &string) {
  std::istringstream stream(string);
  stream >> process.id >> process.time_input >> process.time_work;
  int from, to;
  process.io_blocks.clear();
  while (stream >> from >> to) {
    process.io_blocks.push_back(std::make_pair(from, to));
  }
}

typedef std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, std::greater<std::pair<int, int> > > queue;

std::pair<int, int> get_next_process(int time, queue &queue, std::vector<process> &processes) {
  std::vector<std::pair<int, int> > candidates;

  //pop candidates
  while (!queue.empty() && queue.top().first <= time) {
    candidates.push_back(queue.top());
    queue.pop();
  }

  if (candidates.empty()) {
    std::pair<int, int> result(queue.top());
    queue.pop();
    return result;
  }

  size_t result_index = 0;
  for (size_t i = 0; i < candidates.size(); ++i) {
    process &process = processes[candidates[i].second];
    if (process.time_work <= process.time_completed + quantum || (!process.io_blocks.empty() && process.io_blocks.back().first <= process.time_completed + quantum)) {
      result_index = i;
    }
  }

  //push back
  for (size_t i = 0; i < candidates.size(); ++i) {
    if (i != result_index) {
      queue.push(candidates[i]);
    }
  }

  return candidates[result_index];
}


int main() {
  std::cin >> quantum;
  std::vector<process> processes;
  std::string string;
  std::getline(std::cin, string);
  while (std::getline(std::cin, string)) {
    process process;
    input_next_process(process, string);
    processes.push_back(process);
  }

  queue queue;
  for (size_t i = 0; i < processes.size(); ++i) {
    processes[i].time_completed = 0;
    queue.push(std::make_pair(processes[i].time_input, i));
  }

  int time = 0;
  while (!queue.empty()) {
    std::pair<int, int> next_process = get_next_process(time, queue, processes);
    process &process = processes[next_process.second];

    if (next_process.first > time) {
      std::cout << time << " IDLE" << std::endl;
    }

    time = std::max(time, next_process.first);
    std::cout << time << " " << process.id << std::endl;

    if (!process.io_blocks.empty() && process.io_blocks.back().first - process.time_completed <= quantum) {
      std::pair<int, int> io = process.io_blocks.back();
      time += io.first - process.time_completed;
      process.time_completed = io.first + io.second;
      queue.push(std::make_pair(time + io.second, next_process.second));
      process.io_blocks.pop_back();
    } else if (process.time_work - process.time_completed <= quantum) {
      time += process.time_work - process.time_completed;
    } else {
      time += quantum;
      process.time_completed += quantum;
      queue.push(std::pair<int, int>(time, next_process.second));
    }
  }

  return 0;
}
