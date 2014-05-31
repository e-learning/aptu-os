#include <iostream>
// #include <string>
// #include <algorithm>
#include <set>
using namespace std;

unsigned char *memDataPtr;

int memSize;

struct MemoryBlock;

MemoryBlock *getBlockAt(int offset){
    return reinterpret_cast<MemoryBlock *>(memDataPtr + offset);
}

struct MemoryBlock {
    unsigned short offset;
    unsigned short previous;
    unsigned short size;
    bool isFree;

    MemoryBlock *getNext(){
        if( offset + size < memSize){
            return getBlockAt(offset + size);
        }
        return 0;
    }

    bool hasNext(){
        if( offset + size < memSize){
            return true;
        }
        return false;
    }

    bool hasPrevious(){
        if (offset != 0){
            return true;
        }
        return false;
    }

    MemoryBlock *getPrevious(){
        if (offset != 0){
            return getBlockAt(previous);
        }
        return 0;
    }

    int getUserMemSize(){
        return (size - sizeof(MemoryBlock));
    }

    int gerUserMemOffset(){
        return (sizeof(MemoryBlock)+offset);
    }
};

int alloc(int size){
    for(MemoryBlock * i = getBlockAt(0); i; i = i->getNext() ){
        if( i->isFree && (i->getUserMemSize() >= size) ){
            if(i->getUserMemSize() - size <  sizeof(MemoryBlock) ){
                i->isFree = false;
                return i->offset + sizeof(MemoryBlock);
            } else{
                MemoryBlock * next = getBlockAt(i->gerUserMemOffset() + size);
                next->previous = i->offset;
                next->offset = i->gerUserMemOffset() + size;
                next->size = i->size - (next->offset - i->offset);
                next->isFree = true;
                i->isFree = false;
                i->size = next->offset - i->offset;
                return i->offset + sizeof(MemoryBlock);
            }
        }
    }
    return -1;
}

int free(int offset){
    offset -= sizeof(MemoryBlock);
    MemoryBlock * block = getBlockAt(offset);
		if(block == nullptr) return -1;
    block->isFree = true;

    if(block->hasPrevious() && block->hasNext() &&
            block->getPrevious()->isFree && block->getNext()->isFree){
        if(block->getNext()->hasNext()){
            block->getNext()->getNext()->previous = block->getPrevious()->offset;
        }

        block->getPrevious()->size += ( block->size + block->getNext()->size);


    } else if(block->hasPrevious() && block->getPrevious()->isFree){
        if(block->hasNext()){
            block->getNext()->previous = block->getPrevious()->offset;
        }
        block->getPrevious()->size += block->size;


    } else if(block->hasNext() && block->getNext()->isFree){
        if(block->getNext()->hasNext()){
            block->getNext()->getNext()->previous = block->offset;
        }

        block->size += block->getNext()->size;

        block->isFree = true;
    }
		return 1;
}

void printInfo(){
    int maxSize = 0;
    int userBlocks = 0;
    int userMem = 0;


    for( MemoryBlock * i = getBlockAt(0); i; i = i->getNext() ){
        if(i->isFree){
            maxSize = std::max(maxSize, i->getUserMemSize() );
        } else{
            ++userBlocks;
            userMem += i->getUserMemSize();
        }
    }

    std::cout << userBlocks << userMem << maxSize << std::endl;
}

void printMap(){
    for( MemoryBlock * i = getBlockAt(0); i; i = i->getNext() ){
        for(unsigned int j = 0; j < sizeof(MemoryBlock); ++j){
            std::cout << 'm';
        }
        if(i->isFree){
            for(int j = 0; j < i->getUserMemSize(); ++j){
                std::cout << 'f';
            }
        } else{
            for(int j = 0; j < i->getUserMemSize(); ++j){
                std::cout << 'u';
            }
        }
    }
    std::cout << std::endl;
}

int main() {

    cin >> memSize;
    memDataPtr = new unsigned char[memSize];
    MemoryBlock * memory = getBlockAt(0);
    memory->size = memSize;
    memory->offset = 0;
    memory->isFree = true;
    std::string cmd = "";
		std::set<int> blocks;
    while(cmd!="QUIT"){

        std::cin >> cmd;

        if(cmd == "ALLOC"){

            int size;

            std::cin >> size;

            int position = alloc(size);

            if(position == -1){
                std::cout << '-' << std::endl;
            } else{
                std::cout << '+' << position << std::endl;
								blocks.insert(position);
            }

        } else if(cmd == "FREE"){
            int offset;
            std::cin >> offset;
            int res = free(offset);
						if(blocks.find(offset)==blocks.end()){
								std::cout << '-' << std::endl;
						} else{
								std::cout << '+' << std::endl;
						}
        } else if(cmd == "INFO"){
            printInfo();
        } else if(cmd == "MAP"){
            printMap();
        }
    }
    return 0;
}
