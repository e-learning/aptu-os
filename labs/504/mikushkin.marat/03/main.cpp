#include <string>
#include "Allocator.h"
#include "MemoryBlock.h"
#include "Processor.h"

using namespace std;

int main() {
	size_t blockSize = 0;
	cin >> blockSize;

	Allocator allocator(blockSize);
	MemoryBlock memoryBlock(blockSize);
	Processor processor(&allocator, &memoryBlock);

	for (string request = ""; cin >> request;) {
		processor.perform(request);
	}

	processor.perform("free memory");

	return 0;
}

