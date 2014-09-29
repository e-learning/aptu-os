#include <algorithm>

#include "process.h"
#include "scheduler.h"

int main() {
    Scheduler scheduler;
    scheduler.init();
    while(!(scheduler.waitingProcesses.empty() && scheduler.readyProcesses.empty())) {
        scheduler.update();
    }
    return 0;
}
