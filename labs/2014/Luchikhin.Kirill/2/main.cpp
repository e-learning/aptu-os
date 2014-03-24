#include "include/OS.h"
#include "include/Process.h"
#include "include/IO.h"

#include <iostream>
#include <deque>
#include <string>
#include <algorithm>
#include <sstream>

using std::istream;
using std::string;
using std::istringstream;
using std::cin;

void addNewProcesses(istream &in, OS& os)
{
    string process_declaration = "";
    getline(in, process_declaration);

    while (getline(in, process_declaration)) {
        istringstream iss(process_declaration);
        string process_name = "";
        size_t begin_time = 0;
        size_t duration = 0;
        size_t io_begin = 0;
        size_t io_duration = 0;
        iss >> process_name >> begin_time >> duration;
        process_ptr p(new Process(process_name, begin_time, duration));

        while (iss >> io_begin >> io_duration)
            p->addIO(io_ptr(new IO(io_begin, io_duration)));

        os.addProcess(p);
    }
}

int main()
{
    size_t c = 0;
    cin >> c;
    OS os(c);
    addNewProcesses(cin, os);
    os.run();
    return 0;
}
