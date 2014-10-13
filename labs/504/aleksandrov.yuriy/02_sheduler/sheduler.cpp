#include <iostream>
#include <queue> // priority_queue
#include <vector>
#include <string>
#include <sstream>

using namespace std;

class Process {
public:
	string id;
	int start, end;
	queue< pair<int, int> > iopairs;
	int runtime;

	Process(string line) {
		runtime = 0;
		stringstream procInfo(line);
		
		procInfo >> id;
		procInfo >> start >> end;
		int ioStart, ioDuration;
		while(procInfo >> ioStart && procInfo >> ioDuration) {
			iopairs.push(pair<int,int>(ioStart, ioDuration));
		}
	}

	int requiredTime() const {
		if(iopairs.empty()) 
			return end-runtime;
		else
			return iopairs.front().first-runtime;
	}
};

class RequiredTimeCompare {
public:
	bool operator() (Process const & first, Process const & second) {
		return first.requiredTime() > second.requiredTime();
	}
};

class StartTimeCompare {
public:
	bool operator() (Process const & first, Process const & second) {
		return first.start > second.start;
	}
};

int main() 
{
	int quantDuration;
	int currentTime = 0;
	priority_queue<Process, vector<Process>, StartTimeCompare> waitingProcesses;
	priority_queue<Process, vector<Process>, RequiredTimeCompare> initiatedProcesses;
	string line;

	cin >> quantDuration;
	getline(cin, line);

	while(getline(cin, line) && !line.empty())
		waitingProcesses.push(Process(line));

	while(!waitingProcesses.empty() || !initiatedProcesses.empty())
	{
		while(!waitingProcesses.empty() && waitingProcesses.top().start <= currentTime)
		{
			initiatedProcesses.push(waitingProcesses.top());
			waitingProcesses.pop();
		}

		if(initiatedProcesses.empty())
		{
			cout << currentTime << " IDLE" << endl;
			currentTime = waitingProcesses.top().start;
			continue;
		}

		Process currentProcess = initiatedProcesses.top();
		initiatedProcesses.pop();

		cout << currentTime << " " << currentProcess.id << endl;

		int activeTime = (quantDuration < currentProcess.requiredTime()) ? quantDuration : currentProcess.requiredTime();
		currentProcess.runtime += activeTime;
		currentTime += activeTime;


		if(currentProcess.requiredTime() > 0)
		{
			initiatedProcesses.push(currentProcess);
			continue;
		}

		if(!currentProcess.iopairs.empty())
		{
			int ioDuration = currentProcess.iopairs.front().second;
			currentProcess.runtime += ioDuration;
			currentProcess.start = currentTime + ioDuration;

			currentProcess.iopairs.pop();

			waitingProcesses.push(currentProcess);
		}
	}
	
	return 0;
}
