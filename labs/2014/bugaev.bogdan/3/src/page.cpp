#include "page.h"
#include "utils.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>


void Page::load(Config const &config, int number)
{
    unload();

    m_size = config.pageSize;

    m_fd = open(config.root.concat(getString(number)).c_str(), O_RDWR);
    //std::cerr << "Open:\t" << m_fd << "\t(" << number << ")" << std::endl;

    if (m_fd == -1) {
        throw PageException("Page " + getString(number) + " loading error");
    }

    m_data = reinterpret_cast<char *>(
        mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
        //mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, m_fd, 0));
    if (m_data == MAP_FAILED) {
        throw PageException("Page " + getString(number) + " loading error");
    }
}


void Page::unload()
{
    if (!m_size)
        return;

    //if (msync(m_data, m_size, MS_SYNC) == -1) {
    //    throw PageException("Page closing error");
    //}
    //std::cerr << "Close:\t" << m_fd << std::endl;

    if (munmap(m_data, m_size) == -1) {
        throw PageException("Page closing error");
    }
    m_data = 0;

    m_fd = close(m_fd);
    if (m_fd == -1) {
        throw PageException("Page closing error");
    }

    m_size = 0;
}
