#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <map>
#include <queue>

#include <cstddef>

struct io_op {
  long long start_time;
  long long total_time;
};

struct task {
  std::string id;
  long long start_time;
  long long total_time;
  std::deque<io_op> iops;
};

void read_tasks(std::multimap<long long, task>& tasks) {
  std::string line;
  std::stringstream ss;

  std::getline(std::cin, line);
  while (std::getline(std::cin, line)) {
    ss.clear();
    ss.str(line);

    task ts;
  
    ss >> ts.id >> ts.start_time >> ts.total_time;

    long long start;
    while (ss >> start) {
      io_op io;
      
      io.start_time = start;
      ss >> io.total_time;
      
      ts.iops.push_back(io);
    }

    tasks.insert(std::make_pair(ts.start_time, ts));
  }
}

void decrease_start(std::deque<io_op>& iops, long long delta) {
  for (size_t i = 0; i < iops.size(); ++i) {
    iops[i].start_time -= delta;
  }
}

int main(int argc, char* argv[]) {
  long long c;
  std::cin >> c;

  std::multimap<long long, task> tasks;
  read_tasks(tasks); 

  std::multimap<long long, task> ready;

  long long time = 0;
  while (!tasks.empty() || !ready.empty()) {
    while (!tasks.empty() && time >= tasks.begin()->first) {
      task t = tasks.begin()->second;
      
      long long priority = t.total_time;
      if (!t.iops.empty()) {
        priority = std::min(priority, t.iops.front().start_time);
      }

      ready.insert(std::make_pair(priority, t));
      tasks.erase(tasks.begin());
    }

    if (ready.empty()) {
      std::cout << time << " IDLE" << std::endl;
      time += std::min(c, tasks.begin()->first-time);
            
      continue;
    } 

    task ts = ready.begin()->second;
    ready.erase(ready.begin());

    long long cpu_time;
    if (!ts.iops.empty() && ts.iops.front().start_time == 0) {
      io_op io = ts.iops.front();
      ts.iops.pop_front();
      
      ts.total_time -= io.total_time;
      ts.start_time = time + io.total_time;
      decrease_start(ts.iops, io.total_time); 
      tasks.insert(std::make_pair(ts.start_time, ts));
      continue;     
    } 

    if (!ts.iops.empty() && ts.iops.front().start_time < c) {
      io_op io = ts.iops.front();
      cpu_time = io.start_time;
    } else {
      cpu_time = std::min(ts.total_time, c);
    }
    
    ts.total_time -= cpu_time;
    ts.start_time = time + cpu_time;
    decrease_start(ts.iops, cpu_time);

    if (ts.total_time > 0) {
      tasks.insert(std::make_pair(ts.start_time, ts));
    }

    std::cout << time << " " << ts.id << std::endl;
    time += cpu_time;
  }
  return 0;
}
