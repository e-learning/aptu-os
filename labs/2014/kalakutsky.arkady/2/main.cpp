#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <cstdlib>
#include <queue>
#include <limits>

using std::cout;
using std::cin;
using std::endl;

using std::string;
using std::list;
using std::vector;
using std::priority_queue;

typedef unsigned int Time;

struct proc{
	string name;
	Time start_time;
	Time duration;
	list <int> ios;
	Time current_time;
	bool operator==(const proc& other)const{
		return name == other.name;
	}
};
struct event {
	enum eventType{
		t_start,
		t_finish,
		t_interrupt,
		t_interrupt_finish,
		t_timeout,
		t_eot
	};
	Time time;
	eventType type;
	proc* process;

	event(Time t, eventType et, proc* p):
		time(t),
		type(et),
		process(p){}
	bool operator<(const event& other) const{
		return this->time > other.time;
	}
	
};

namespace std{
	ostream& operator<<(ostream& out, proc & pr){
		out << "----------------------" << endl;
		out << pr.name << " " << pr.start_time << " " << pr.duration << endl;
		for (list<int>::iterator it = pr.ios.begin(); it != pr.ios.end(); ++it){
			out << *it << " ";
		}
		return out;
	}
};

int main(){

	Time q_size = 0;
	cin >> q_size;
	list<proc> procs;
	while(true){
		string tmp;
		cin >> tmp;
		if (tmp.empty()){
			break;
		}else if(tmp[0] <= '9' && tmp[0] >= '0'){
			procs.back().ios.push_back(atoi(tmp.c_str()));
			int d = 0;
			cin >> d;
			procs.back().ios.push_back(d);
		}else{
			procs.push_back(proc());
			procs.back().name = tmp;
			cin >> procs.back().start_time;
			cin >> procs.back().duration;
		}
	}
	priority_queue <event> events;	
	events.push(event(std::numeric_limits<Time>::max(), event::t_eot, 0));
	//init events
	for(list<proc>::iterator it = procs.begin(); it != procs.end(); ++it){
		events.push(event(it->start_time, event::t_start, &*it));
	}
	
	Time global_time = 0;
	list<proc*> low_p;
	list<proc*> high_p;
	list<proc*> waiting;
	proc* active = 0;
	events.size();
	while(!procs.empty()){
		if (events.top().time > global_time){
			if (active){
				global_time += 1;
				active->current_time += 1;
			}else{
				if (!high_p.empty() ){
					active = high_p.back();
					cout << global_time << " " << active->name <<endl;
					high_p.pop_back();
					if (!active->ios.empty()){
						events.push(event(global_time + active->ios.front() -
										  active->current_time,
										  event::t_interrupt,
										  active));
					}else{
						events.push(event(global_time + active->duration -
										  active->current_time,
										  event::t_finish,
										  active));
					}
				}else if(!low_p.empty()){
					active = low_p.back();
					cout << global_time << " " << active->name <<endl;
					low_p.pop_back();
					events.push(event(global_time + q_size,
									  event::t_timeout,
									  active));
				}else{
					cout << global_time <<" IDLE"<<endl;
					global_time = events.top().time > global_time + q_size?
						global_time + q_size:
						events.top().time;
				}
			}
		}else{
			
			switch (events.top().type){
			case event::t_start:{
				proc* cur = events.top().process;
				int t = cur->duration - cur->current_time;
				if (!cur->ios.empty()){
					t = cur->ios.front() - cur->current_time;
				}
				if (t <= q_size){
					high_p.push_back(cur);
				}else{
					low_p.push_back(cur);
				}
				
				events.pop();
				break;
			}
			case event::t_finish:{
				active = 0;
				procs.remove(*events.top().process);
				events.pop();
				break;
			}
			case event::t_interrupt:{
				active = 0;
				proc* cur = events.top().process;
				events.pop();
				cur->ios.pop_front();
				waiting.push_back(cur);
				events.push(event(cur->ios.front() + global_time, 
								  event::t_interrupt_finish,
								  cur));
				break;
			}
			case event::t_interrupt_finish:{
				proc* cur = events.top().process;
				events.pop();
				waiting.remove(cur);
				cur->current_time += cur->ios.front();
				cur->ios.pop_front();
				events.push(event(global_time, event::t_start, cur));
				break;
			}
			case event::t_timeout:{
				active = 0;
				proc* cur = events.top().process;
				events.pop();
				events.push(event(global_time, event::t_start, cur));
				break;
			}
			case event::t_eot:{
				//time overflow
				procs.erase(procs.begin(), procs.end());
				active = 0;
				break; 
			}
			default:
				break;
			}
		}
	}
	return 0;
}
