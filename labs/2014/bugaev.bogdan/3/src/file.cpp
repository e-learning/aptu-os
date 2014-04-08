#include "file.h"
#include "defs.h"


void createDir(Config const &config, int pageNumber,
               std::string const &name)
{
    Page page;
    page.load(config, pageNumber);
    PageWriter writer(page);
    writer.write(-1).write(-1).write(-1);
    writer.write(config.pageSize).write(time(0));
    writer.write<int>(LEN/*name.size()*/).writeString(name);
    page.unload();
}


void createFile(Config const &config, int pageNumber,
                std::string const &name)
{
    Page page;
    page.load(config, pageNumber);
    PageWriter writer(page);
    writer.write(0).write(-1).write(-1);
    writer.step<int>().write(time(0));
    writer.write<int>(LEN/*name.size()*/).writeString(name);
    page.unload();
}


std::string const getFileName(Page &page)
{
    PageReader reader(page);
    int len = reader.step<int>(4).step<time_t>().read<int>();
    std::string result;
    for (int i = 0; i < len; ++i) {
        char tmp = reader.read<char>();
        if (!tmp) break;
        result += tmp;
    }
    return result;
}


int getFileSize(Page &page)
{
    return PageReader(page).step<int>(3).read<int>();
}


time_t getFileDate(Page &page)
{
    return PageReader(page).step<int>(4).read<time_t>();
}


int getFirstFileNumber(Page &page)
{
    return PageReader(page).read<int>();
}


void setFirstFileNumber(Page &page, int fileNumber)
{
    PageWriter(page).write(fileNumber);
}


int getNextFileNumber(Page &page)
{
    return PageReader(page).step<int>().read<int>();
}


int getFirstDataBlockNumber(Page &page)
{
    return PageReader(page).step<int>(2).read<int>();
}

void setFirstDataBlockNumber(Page &page, int number)
{
    PageWriter(page).step<int>(2).write<int>(number);
}


int getNextDataBlockNumber(Page &page)
{
    return PageReader(page).read<int>();
}


void setNextFileNumber(Page &page, int fileNumber)
{
    PageWriter(page).step<int>().write(fileNumber);
}


bool isDir(Page &page)
{
    return PageReader(page).read<int>() != 0;
}


bool isEmptyDir(Page &page)
{
    return PageReader(page).read<int>() == -1;
}


int findFileNumber(Config const &config, int dirNumber,
                   std::string const &name)
{
    Page page;
    page.load(config, dirNumber);
    int file = getFirstFileNumber(page);
    page.unload();

    while (file > -1) {
        Page page;
        page.load(config, file);
        if (getFileName(page) == name)
            return file;
        file = getNextFileNumber(page);
        page.unload();
    }

    return -1;
}
