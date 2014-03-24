#ifndef IO_H
#define IO_H

#include <cstddef>
#include <tr1/memory>

using std::tr1::shared_ptr;

struct IO {
    IO(size_t begin_time, size_t duration):
        begin_time(begin_time),
        duration(duration)
    {}

    size_t begin_time;
    size_t duration;
};

typedef shared_ptr<IO> io_ptr;

bool inline operator<(io_ptr const io1, io_ptr const io2)
{
    return io1->begin_time < io2->begin_time;
}

#endif // IO_H
