#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <list>

using namespace std;

struct interruption{
    int start;
    int duration;
    interruption(int s, int d) : start(s), duration(d) {}
};

struct process{
    string name;
    int time_to_start;
    int duration;
    int count;
    bool flag_erase;

    queue<interruption> interr;


    process() {}
    process(string n, int t, int d): name(n), time_to_start(t), duration(d), count(0), flag_erase(0) {}

  //  ~process() {
//      // delete interr ... ?
//  }

};

int quantum;
list<process> prior1;  // waiting process
list<process> prior2;  // less prior
list<process> prior3;  // big prior


int total_time = 0;
int step_time = 0;


bool flag_idle = 1;


bool foo (const process& value) {
    return value.flag_erase;
}

int main(){

    // input
    string id;
    int ti, tw, oit, iol;

    string line;

    stringstream ss;
    getline(cin, line);
    ss << line;
    ss >> quantum;

    while(getline(cin, line)) {

        stringstream ss;

        ss << line;

        ss >> id;
        ss >> ti >> tw;
        process proc(id, ti, tw);
        while(ss >> oit) {
            ss >> iol;
//            if(oit == 0) {                      //??  i/o at first
//                proc.time_to_start += iol;
//                continue;
//            }
            interruption inter(oit, iol);
            proc.interr.push(inter);
        }

        prior1.push_back(proc);
    }


    // start

    while( !prior1.empty() || !prior2.empty() || !prior3.empty()) {

        list<process>::iterator it;

        for(it = prior1.begin(); it != prior1.end(); ++it ) {

            if(it->time_to_start > step_time)
                it->time_to_start -= step_time;
            else {
                it->time_to_start = 0;
                if(it->duration <= quantum || ( !it->interr.empty() && (it->interr.front().start - it->count) <= quantum) ) {
                    prior3.push_back( *it );
                    it->flag_erase = 1;
                }
                else {
                    prior2.push_back( *it );
                    it->flag_erase = 1;
                }
            }

        }
        prior1.remove_if(foo);


        process cur_proc;

        if( !prior3.empty() ) {

            flag_idle = 1;

            cur_proc =  prior3.front();
            prior3.erase( prior3.begin() );

            cout << total_time <<" "<< cur_proc.name <<"\n";

            step_time = 0;
            while(step_time < quantum) {

                cur_proc.count+=1;
                cur_proc.duration -=1;
                total_time += 1;
                step_time += 1;

                if( !cur_proc.interr.empty() && (cur_proc.interr.front().start - cur_proc.count) == 0 ) {
                    cur_proc.time_to_start = cur_proc.interr.front().duration + step_time;
                    cur_proc.duration -= cur_proc.interr.front().duration;
                    cur_proc.interr.pop();

                    break;
                }

                if(cur_proc.duration == 0) break;

            }

            if( cur_proc.duration != 0 ) {
                cur_proc.flag_erase = 0;
                prior1.push_back(cur_proc);
            }

        }
        else if( !prior2.empty() ) {


            flag_idle = 1;

            cur_proc = prior2.front();
            prior2.erase( prior2.begin() );

            cout << total_time <<" "<< cur_proc.name <<"\n";
            cur_proc.duration -= quantum;
            cur_proc.count += quantum;

            cur_proc.flag_erase = 0;
            prior1.push_back(cur_proc);

            total_time += quantum;
            step_time = quantum;
        }
        else {

            if(flag_idle) cout << total_time <<" IDLE\n";
            flag_idle = 0;
            total_time += 1;
            step_time = 1;
        }


    }

    return 0;
}

