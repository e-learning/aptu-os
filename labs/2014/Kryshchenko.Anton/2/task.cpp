#include "task.h"
#include <sstream>


task::task(string & str) :io_time(0),total_time(0),
    local_time(0) {
    std::stringstream stream(str);
    stream>> name>> start>> lenght;
    int start_io;
    int end_io;
    while(stream>> start_io>> end_io) {
        io_blocks.push_back(std::make_pair(start_io, end_io));
    }
}

bool task::compair_priority() const {
    if(total_time + quant >= lenght) {
        return true;
    }
    if(!io_blocks.empty() &&
            total_time + quant >= io_blocks.front().first) {
        return true;
    }
    return false;
}

bool task::update_IO_time() {
    ++io_time;
    ++total_time;
    bool fl = false;
    if(!io_blocks.empty() &&
            io_time >= io_blocks.front().second) {
        fl = true;
        io_blocks.pop_front();
        io_time = 0;
    }
    return fl;
}

TaskEvent task::update_working_time() {

    ++total_time;
    ++local_time;
    if(total_time == lenght) {
        local_time = 0;
        return TaskEvent::FINISHED;
    }
    if(!io_blocks.empty() &&
            total_time == io_blocks.front().first) {
        local_time = 0;
        return TaskEvent::IO;

    }

    if(local_time == quant) {
        local_time = 0;
        return TaskEvent::QUANT_OVER;

    }
    return TaskEvent::OK;

}
