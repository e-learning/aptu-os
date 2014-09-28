#include <iostream>
#include <vector>
#include <sstream>
#include <queue>

using namespace std;

class Task {
public:
    Task(string init_string, int c)
    {
        if (c < 1) {
            return;
        };

        C = c;

        start_time = -1;

        worked_time = 0;

        stringstream ss(init_string);

        ss >> ID;
        ss >> Ti >> Tw;

        int io_start, io_len;
        while(ss >> io_start >> io_len) {
            iops.push(make_pair(io_start, io_len));
        }
    };

    string get_ID() const {
        return ID;
    }

    int get_Ti() const {
        return Ti;
    }

    int get_Tw() const {
        return Tw;
    }

    int get_start_time() const {
        return start_time;
    }

    queue<pair<int, int> > get_iops() const {
        return iops;
    }

    bool is_finish() const {
//        return Tw == worked_time;
        return left_time() == 0;
    }

    int left_time() const {
        int finished_time = 0;

        if(!iops.empty()) {
            finished_time = iops.front().first;
        }
        else {
            finished_time = Tw;
        }

        return finished_time - worked_time;
    }

    void recalculate_Ti_and_worked_time(int current_time) {
        if(!get_iops().empty()) {
            int length_of_io = get_iops().front().second;
            worked_time += length_of_io;
            Ti = current_time + length_of_io;

            get_iops().pop();
        }
    }

    int run(int current_time) {
        start_time = current_time;

        int wt = min(C, left_time());

        worked_time += wt;

        return wt;
    }

private:
    //время, в течение которого процессор работал
    int worked_time;

    string ID;

    //время поступления задачи на вход планировщика
    int Ti;

    //продолжительность работы данной задачи
    int Tw;

    //время старта задачи
    int start_time;

    //IOT IOL - описание операции ввода/вывода
    queue<pair<int, int> > iops;

    //квант процессорного времени
    int C;
};

inline ostream& operator<< (ostream& os, const Task& obj) {
    os << obj.get_start_time() << " " << obj.get_ID();
    return os;
}