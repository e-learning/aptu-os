#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>


struct Task
{
  int start;
  int end;

  std::string name;
  std::queue< std::pair<int, int> > io;
  int duration;

  Task(std::string const& line)
    : duration(0)
  {
    std::stringstream s(line);
    s >> name;
    s >> start >> end;
    int IOT, IOL;
    while (s >> IOT >> IOL)
      {
	io.push(std::make_pair(IOT, IOL));
      }
  }

  int remainingTime() const
  {
    if (io.empty()) return end - duration;
    return io.front().first - duration;
  }
};

struct startCmp
{
  bool operator() (Task const & first, Task const & second)
  {
    return first.start > second.start;
  }
};

struct durationCmp
{
  bool operator() (Task const & first, Task const & second)
  {
    return first.remainingTime() > second.remainingTime();
  }
};

int main(void)
{
  std::priority_queue<Task, std::vector<Task>, durationCmp> ready;
  std::priority_queue<Task, std::vector<Task>, startCmp> waiting;
  
  int quant, curr_time = 0;

  std::cin >> quant;
  std::string line;
  std::getline(std::cin, line);
  while(std::getline(std::cin, line))
    {
      waiting.push(Task(line));
    }

  while (!ready.empty() || !waiting.empty())
    {
      while (!waiting.empty() && waiting.top().start <= curr_time)
	{
	  ready.push(waiting.top());
	  waiting.pop();
	}
      
      if (ready.empty())
	{
	  std::cout << curr_time << " IDLE\n";
	  curr_time = waiting.top().start;
	  continue;
	}
      
      Task curr = ready.top();
      ready.pop();
      
      std::cout << curr_time << " " << curr.name << std::endl;
      
      int active = std::min(quant, curr.remainingTime());
      curr.duration += active;
      curr_time += active;
            
      if (curr.remainingTime() > 0)
	{
	  ready.push(curr);
	  continue;
	}
    
      if (!curr.io.empty())
	{
	  int IOL = curr.io.front().second;
	  curr.duration += IOL;
	  curr.start = curr_time + IOL;
	  
	  curr.io.pop();
	  
	  waiting.push(curr);
	}
    }
  
  return 0;
}
