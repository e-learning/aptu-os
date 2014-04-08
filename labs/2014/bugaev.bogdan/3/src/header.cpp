#include "header.h"
#include "page.h"
#include <ctime>


int popFreePageNumber(Config const &config)
{
    Page page;
    page.load(config, 0);
    PageReader reader(page);

    int cnt = reader.step<int>().read<int>();
    if (cnt == 0) return -1;

    int pos = cnt - 1;
    int here = (config.pageSize - 2 * sizeof(int)) / sizeof(int);

    int result = -1;
    if (pos < here) {
        result = reader.step<int>(pos).read<int>();
    } else {
        pos -= here;
        int capacity = config.pageSize / sizeof(int);
        int src = 1 + pos / capacity;
        pos %= capacity;

        Page page;
        page.load(config, src);
        PageReader reader(page);
        result = reader.step<int>(pos).read<int>();
        page.unload();
    }

    PageWriter writer(page);
    writer.step<int>().write(cnt - 1);

    return result;
}


int pushFreePageNumber(Config const &config, int pageNumber)
{
    Page page;
    page.load(config, 0);
    PageReader reader(page);

    int cnt = reader.step<int>().read<int>();
    if (cnt == 0) return -1;

    int pos = cnt + 2;
    int here = (config.pageSize - 2 * sizeof(int)) / sizeof(int);

    if (pos < here) {
        PageWriter writer(page);
        writer.step<int>(pos).write(pageNumber);
    } else {
        pos -= here;
        int capacity = config.pageSize / sizeof(int);
        int dst = 1 + pos / capacity;
        pos %= capacity;

        Page page;
        page.load(config, dst);
        PageWriter writer(page);
        writer.step<int>(pos).write(pageNumber);
        page.unload();
    }

    PageWriter writer(page);
    writer.step<int>().write(cnt + 1);

    return 0;
}


int getHeaderSize(Config const &config)
{
    Page page;
    page.load(config, 0);
    PageReader reader(page);
    int result = reader.read<int>();
    page.unload();
    return result;
}


int getFreePageCount(Config const &config)
{
    Page page;
    page.load(config, 0);
    PageReader reader(page);
    int result = reader.step<int>().read<int>();
    page.unload();
    return result;
}
