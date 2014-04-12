#ifndef MANAGER_H
#define MANAGER_H

#include <cstddef>
#include <string>
#include <list>

class Command;
class CommandAlloc;
class CommandFree;
class CommandInfo;
class CommandMap;

static size_t const SUCCESS = 0;
static size_t const ERROR = 1;
static size_t const SYSTEM_BLOCK_SIZE = 2;

class MemoryManager
{
    size_t memorySize_;
    char* memoryBuf_;
    size_t blocksCount_;
    size_t userSize_;
    std::list<size_t> userPtrs_;

    bool isBlockFit (size_t currentBlockPtr, size_t requiredSize);
    void allocate(size_t currentBlockPtr, size_t requiredSize);
    size_t get_nextBlockPtr (size_t currentBlockPtr);
    void set_freeBlock (size_t blockPtr, size_t blockSize);
    void set_busyBlock (size_t blockPtr, size_t blockSize);
    void try_to_merge_with_leftBlock (size_t currentBlockPtr);
    void try_to_merge_with_rightBlock (size_t currentBlockPtr);
    size_t maxFitBlock ();
    void print_info (size_t blockIterator);

public:
    MemoryManager (size_t memorySize);
    MemoryManager (MemoryManager const & mm);
    MemoryManager & operator= (MemoryManager const & mm);
    ~MemoryManager();

    void execute (Command* command);
    void execute (CommandAlloc* cmdAlloc);
    void execute (CommandFree* cmdFree);
    void execute (CommandInfo* cmdInfo);
    void execute (CommandMap* cmdMap);
};

#endif // MANAGER_H
