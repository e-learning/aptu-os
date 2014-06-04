#ifndef BLOCK
#define BLOCK

const int INT_SIZE = 4;
const int HEADER_SIZE = 3 * INT_SIZE;
const int MIN_BLOCK_SIZE = HEADER_SIZE + 1;

class Block
{
public:
    Block(int size) : mySize(size), nextBlock(NULL), prevBlock(NULL) {}

    int available()
    {
        if(isFree()) {
            return mySize + HEADER_SIZE;
        } else {
            return 0;
        }
    }

    bool isFree()
    {
        return mySize > 0;
    }

    void allocate() 
    {
        if(mySize > 0)
        {
            mySize = -mySize;
        }
    }

    void free() 
    {
        if(mySize < 0) {
            mySize = -mySize;
        }
    }

    int getRealSize() {return abs(mySize);}

    void mergeWithNext()
    {
        if (!isFree() || nextBlock == NULL || !nextBlock->isFree())
            return;

        Block* nextBlockPtr = nextBlock;
        mySize += (HEADER_SIZE + nextBlock->mySize);
        if (nextBlock->nextBlock != NULL)
            nextBlock->nextBlock->prevBlock = this;
        nextBlock = nextBlock->nextBlock;
        
        delete nextBlockPtr;
    }

    void mergeWithPrev()
    {
        if (prevBlock != NULL) {
            prevBlock->mergeWithNext();
        }
    }

    Block* next() {return nextBlock;}
    Block* prev() {return prevBlock;}

    void setPrevBlock(Block* prev) {prevBlock = prev;}
    void setNextBlock(Block* next) {nextBlock = next;}

    int size() {return mySize;}

    void setSize(int size) {mySize = size;}

private:
    int mySize;
    Block* nextBlock;
    Block* prevBlock;
    bool myIsFree;
};


#endif