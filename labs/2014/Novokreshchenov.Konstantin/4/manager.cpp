#include "manager.h"
#include "command.h"
#include "utility.h"
#include <iostream>
#include <algorithm>
#include <list>

MemoryManager::MemoryManager (size_t memorySize):
    memorySize_(memorySize), memoryBuf_(new char[memorySize]),
    blocksCount_(0), userSize_(0)
{
    size_t systemInfo = (memorySize - 2 * SYSTEM_BLOCK_SIZE) * 2 + 1;
    number_to_bytes (memoryBuf_, SYSTEM_BLOCK_SIZE, systemInfo);
    number_to_bytes (memoryBuf_ + memorySize - SYSTEM_BLOCK_SIZE, SYSTEM_BLOCK_SIZE, systemInfo);
}

MemoryManager::MemoryManager (MemoryManager const & mm):
    memorySize_(mm.memorySize_), memoryBuf_(NULL), blocksCount_(mm.blocksCount_), userSize_(mm.userSize_), userPtrs_(mm.userPtrs_)
{
    if (mm.memoryBuf_ != NULL) {
        memoryBuf_ = new char[memorySize_];
        std::copy(mm.memoryBuf_, mm.memoryBuf_ + mm.memorySize_, memoryBuf_);
    }
}

MemoryManager & MemoryManager::operator= (MemoryManager const & mm)
{
    memorySize_ = mm.memorySize_;
    memoryBuf_ = NULL;
    if (mm.memoryBuf_ != NULL) {
        memoryBuf_ = new char[memorySize_];
        std::copy (mm.memoryBuf_, mm.memoryBuf_ + mm.memorySize_, memoryBuf_);
    }
    blocksCount_ = mm.blocksCount_;
    userSize_ = mm.userSize_;
    userPtrs_ = mm.userPtrs_;

    return *this;
}

MemoryManager::~MemoryManager()
{ delete memoryBuf_; }

void MemoryManager::execute (Command* command)
{
    std::cout << "Incorrect command. Try again.." << std::endl;
}

void MemoryManager::execute (CommandAlloc* cmdAlloc)
{
    size_t requiredSize = cmdAlloc->get_size();
    size_t currentBlockPtr = 0;
    while (currentBlockPtr != memorySize_) {
        if (isBlockFit(currentBlockPtr, requiredSize)) {
            allocate(currentBlockPtr, requiredSize);
            std::cout << "+ " << currentBlockPtr + SYSTEM_BLOCK_SIZE << std::endl;
            userPtrs_.push_back(currentBlockPtr + SYSTEM_BLOCK_SIZE);
            return;
        }
        currentBlockPtr = get_nextBlockPtr(currentBlockPtr);
    }
    std::cout << "-" << std::endl;
}

void MemoryManager::execute (CommandFree* cmdFree)
{
    size_t userPtr = cmdFree->get_ptr();
    std::list<size_t>::iterator elemIt = std::find(userPtrs_.begin(), userPtrs_.end(), userPtr);
    if (elemIt == userPtrs_.end()) {
        std::cout << "-" << std::endl;
        return;
    }
    userPtrs_.erase(elemIt);
    size_t currentBlockPtr = userPtr - SYSTEM_BLOCK_SIZE;
    size_t currentBlockSize = number_from_bytes (memoryBuf_ + currentBlockPtr, SYSTEM_BLOCK_SIZE) >> 1;
    set_freeBlock (currentBlockPtr, currentBlockSize);
    blocksCount_ -= 1;
    userSize_ -= currentBlockSize;
    try_to_merge_with_rightBlock (currentBlockPtr);
    try_to_merge_with_leftBlock (currentBlockPtr);

    std::cout << "+" << std::endl;
}

void MemoryManager::execute (CommandInfo* cmdInfo)
{
    std::cout << blocksCount_ << " " << userSize_ << " " << maxFitBlock() << std::endl;
}

void MemoryManager::execute (CommandMap* cmdMap)
{
    size_t currentBlockPtr= 0;
    while (currentBlockPtr != memorySize_) {
        print_info(currentBlockPtr);
        currentBlockPtr = get_nextBlockPtr(currentBlockPtr);
    }
    std::cout << std::endl;
}

bool MemoryManager::isBlockFit (size_t currentBlockPtr, size_t requiredSize)
{
    size_t systemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr, SYSTEM_BLOCK_SIZE);
    size_t blockSize = systemInfo >> 1;
    size_t isFree = systemInfo % 2;
    if (isFree && (requiredSize <= blockSize)) {
        return true;
    }
    return false;
}

void MemoryManager::allocate(size_t currentBlockPtr, size_t requiredSize)
{
    size_t systemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr, SYSTEM_BLOCK_SIZE);
    size_t blockSize = systemInfo >> 1;
    size_t allocateSize = 0;
    if (blockSize - requiredSize < 2 * SYSTEM_BLOCK_SIZE + 1) {
        allocateSize = blockSize;
    }
    else {
        allocateSize = requiredSize;
        set_freeBlock (currentBlockPtr + allocateSize + 2 * SYSTEM_BLOCK_SIZE, blockSize - requiredSize - 2 * SYSTEM_BLOCK_SIZE);
    }
    set_busyBlock (currentBlockPtr, allocateSize);
    blocksCount_ += 1;
    userSize_ += allocateSize;
}

size_t MemoryManager::get_nextBlockPtr (size_t currentBlockPtr)
{
    size_t systemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr, SYSTEM_BLOCK_SIZE);
    size_t blockSize = systemInfo >> 1;
    return currentBlockPtr + blockSize + 2 * SYSTEM_BLOCK_SIZE;
}

void MemoryManager::set_freeBlock (size_t blockPtr, size_t blockSize)
{
    size_t systemInfo = blockSize * 2 + 1;
    number_to_bytes (memoryBuf_ + blockPtr, SYSTEM_BLOCK_SIZE, systemInfo);
    number_to_bytes (memoryBuf_ + blockPtr + SYSTEM_BLOCK_SIZE + blockSize, SYSTEM_BLOCK_SIZE, systemInfo);
}

void MemoryManager::set_busyBlock (size_t blockPtr, size_t blockSize)
{
    size_t systemInfo = blockSize * 2 + 0;
    number_to_bytes (memoryBuf_ + blockPtr, SYSTEM_BLOCK_SIZE, systemInfo);
    number_to_bytes (memoryBuf_ + blockPtr + SYSTEM_BLOCK_SIZE + blockSize, SYSTEM_BLOCK_SIZE, systemInfo);
}

void MemoryManager::try_to_merge_with_leftBlock (size_t currentBlockPtr)
{
    if (currentBlockPtr == 0) {
        return;
    }
    size_t leftSystemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr - SYSTEM_BLOCK_SIZE, SYSTEM_BLOCK_SIZE);
    size_t leftBlockSize = leftSystemInfo >> 1;
    size_t leftIsFree = leftSystemInfo % 2;
    if (! leftIsFree) {
        return;
    }
    size_t currentSystemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr, SYSTEM_BLOCK_SIZE);
    size_t currentBlockSize = currentSystemInfo >> 1;
    set_freeBlock (currentBlockPtr - 2 * SYSTEM_BLOCK_SIZE - leftBlockSize, leftBlockSize + currentBlockSize + 2 * SYSTEM_BLOCK_SIZE);
}

void MemoryManager::try_to_merge_with_rightBlock (size_t currentBlockPtr)
{
    size_t currentSystemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr, SYSTEM_BLOCK_SIZE);
    size_t currentBlockSize = currentSystemInfo >> 1;
    if (currentBlockPtr + currentSystemInfo + 2 * SYSTEM_BLOCK_SIZE == memorySize_) {
        return;
    }
    size_t rightSystemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr + 2 * SYSTEM_BLOCK_SIZE + currentBlockSize, SYSTEM_BLOCK_SIZE);
    size_t rightBlockSize = rightSystemInfo >> 1;
    size_t rightIsFree = rightSystemInfo % 2;
    if (! rightIsFree) {
        return;
    }
    set_freeBlock (currentBlockPtr, currentBlockSize + rightBlockSize + 2 * SYSTEM_BLOCK_SIZE);
}

size_t MemoryManager::maxFitBlock ()
{
    size_t currentBlockPtr = 0;
    size_t maxFreeBlockSize = 0;
    while (currentBlockPtr != memorySize_) {
        size_t currentSystemInfo = number_from_bytes (memoryBuf_ + currentBlockPtr, SYSTEM_BLOCK_SIZE);
        size_t currentBlockSize = currentSystemInfo >> 1;
        size_t currentIsFree = currentSystemInfo % 2;
        if (currentIsFree && (currentBlockSize > maxFreeBlockSize)) {
            maxFreeBlockSize = currentBlockSize;
        }
        currentBlockPtr = get_nextBlockPtr (currentBlockPtr);
    }
    return maxFreeBlockSize;
}

void MemoryManager::print_info (size_t blockPtr)
{
    size_t systemInfo = number_from_bytes(memoryBuf_ + blockPtr, SYSTEM_BLOCK_SIZE);
    size_t blockSize = systemInfo >> 1;
    char state = systemInfo % 2 ? 'f' : 'u';
    print_symbol(SYSTEM_BLOCK_SIZE, 'm');
    print_symbol(blockSize, state);
    print_symbol(SYSTEM_BLOCK_SIZE, 'm');
}













