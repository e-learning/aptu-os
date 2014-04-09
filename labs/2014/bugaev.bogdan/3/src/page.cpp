#include "page.h"
#include "utils.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


void Page::load(Config const &config, int number)
{
    unload();

    m_size = config.pageSize;

    m_fd = open(config.root.concat(getString(number)).c_str(), O_RDWR);
    if (m_fd == -1) {
        throw PageException("Page loading error");
    }

    m_data = reinterpret_cast<char *>(
        mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));
        //mmap(0, m_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, m_fd, 0));
    if (m_data == MAP_FAILED) {
        throw PageException("Page loading error");
    }
}


void Page::unload()
{
    if (!m_size)
        return;

    //if (msync(m_data, m_size, MS_SYNC) == -1) {
    //    throw PageException("Page closing error");
    //}

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
