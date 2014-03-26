#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>

using std::string;
using std::istream;
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;
using std::vector;
using std::priority_queue;
using std::pair;
using std::queue;

typedef pair<size_t, size_t> IO;

int timer = 0;
int procQuant;

class Process {

public:

	bool hasIOwithinQuant() const{
		if( _startTime <= timer && _io.size() != 0){
			return _io.top().first - ( _duration - _remainingTime ) <= procQuant;
		} else{
			return false;
		}
	}

	bool stopsWithinQuant() const{
		return _startTime < timer && _remainingTime < procQuant;
	}

	bool completed() const {
		return _remainingTime == 0;
	}

    bool hasIO() const {
        if(_io.size() == 0){
        	return false;
        } else{
        	return _io.top().first == _duration - _remainingTime;
        }
    }

	int ioEnds() const {
		return _ioStartTime + _io.top().second;
	}

	void closeIO() {
		_remainingTime -= _io.top().second;
		_io.pop();
	}

	void step(){
		_remainingTime--;
	}
	bool operator <(const Process & other) const {
		if (this->hasIOwithinQuant()) {
			return false;
	    } else if (other.hasIOwithinQuant()) {
	        return true;
	    }
	    if (this->stopsWithinQuant()) {
	        return false;
	    } else if (other.stopsWithinQuant()) {
	    	return true;
	    }
	    return this->_startTime > other._startTime;
	}

	int getStartTime() const{
		return _startTime;
	}

	string getID() const{
		return id;
	}

	void startIO(int ioStartTime){
		_ioStartTime = ioStartTime;
	}
    friend istream & operator >> (istream & in, Process & p);
private:
    string id;
    int _startTime;
    int _duration;
    int _remainingTime;
    int _ioStartTime;
    priority_queue<IO> _io;
};

istream & operator >> (istream & in, Process & p){
	if(!(in >> p.id >> p._startTime >> p._duration)){
		return in;
	}
	p._remainingTime = p._duration;
	string line;
	if(!getline(cin, line)){
		return in;
	}
	stringstream ss(line);
	while (ss.good()) {
		size_t a, b;
	    ss >> a >> b;
	    if (!ss.fail()) {
	    	p._io.push(IO(a,b));
	    }
	}
	return in;
}

struct ProcessIOComparator {

    bool operator ()(const Process & that, const Process & other) const {
        return that.ioEnds() > other.ioEnds();
    }

};


priority_queue<Process, vector<Process> > processes;
priority_queue<Process, vector<Process>, ProcessIOComparator> ioProcesses;

void flushIOqueue() {
    while(ioProcesses.size() && ioProcesses.top().ioEnds() <= timer){
        Process p = ioProcesses.top();
        ioProcesses.pop();
        p.closeIO();
        if (!p.completed()) {
            processes.push(p);
        }
    }
}

int main() {

    cin >> procQuant;
    cin.ignore(255,'\n');
	Process p;
    while (cin >> p) {
    	processes.push(p);
    }

    while(!processes.empty() || !ioProcesses.empty()) {

           if (processes.empty() || processes.top().getStartTime() > timer) {
               cout << timer << " IDLE" << endl;
               while (processes.empty() || processes.top().getStartTime() > timer){
                   timer++;
                   flushIOqueue();
               }
           }
           Process p = processes.top();
           processes.pop();
           cout << timer << " " << p.getID() << endl;
           int quantum = procQuant;
           while(quantum-- && !p.completed() && !p.hasIO() ) {
               p.step();
               timer++;
           }

           if (p.hasIO()){
               p.startIO(timer);
               ioProcesses.push(p);
           } else if (!p.completed()) {
        	   processes.push(p);
           }
           flushIOqueue();
       }

    return 0;
}
