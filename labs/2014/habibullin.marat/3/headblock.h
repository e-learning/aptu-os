#ifndef HEADBLOCK_H
#define HEADBLOCK_H

#include <string>

#include "chainableblock.h"
#include "utils.h"

using std::string;

class HeadBlock : public ChainableBlock {
public:
    HeadBlock(size_t s, char magic)
        : ChainableBlock(s, magic)
    {}

    bool SetName(string const& name) { return SetString(Utils::ToRawString(name, NAME_MAX_SIZE), NAME_OFFS); }
    bool SetSize(size_t sz) { return SetSizeT(sz, SIZE_OFFS); }
    bool SetLastModTime(string const& datetime) { return SetString(datetime, DATETIME_OFFS); }
    string GetName() {
        string raw_name = GetString(NAME_OFFS, NAME_MAX_SIZE);
        return Utils::FromRawString(raw_name);
    }
    size_t GetSize() { return GetSizeT(SIZE_OFFS); }
    string GetLastModTime() { return GetString(DATETIME_OFFS, DATETIME_SIZE); }
    const static size_t NAME_MAX_SIZE = 10;
private:
//    const size_t NAME_OFFS = CHHEADER_SIZE;
    const size_t NAME_OFFS = 5;

    const size_t SIZE_OFFS = NAME_OFFS + NAME_MAX_SIZE;
    const size_t DATETIME_OFFS = SIZE_OFFS + SIZET_SIZE;
    const size_t DATETIME_SIZE = 19;
    const size_t BEGPTR_OFFS = DATETIME_OFFS + DATETIME_SIZE;
};

#endif // HEADBLOCK_H
