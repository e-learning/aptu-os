#include <iostream>
#include <string>

#include "sheduler.h"
#include "task.h"


int task::quant = 0;

int main()
{

    sheduler sch;
    std::cin >> task::quant;

    std::string current_line;
    while(getline(std::cin, current_line)) {
        if(current_line.length() != 0) {
            sch.add_task(task(current_line));
        }
    }

    sch.start();

    return 0;
}
