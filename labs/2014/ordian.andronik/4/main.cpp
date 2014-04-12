#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>

typedef std::uint16_t d_t;

struct Allocator
{
  explicit Allocator(size_t N)
    : N_(N)
    , memory_(N)
  {
    Chunk * first = chunk(0);
    first->free = true;
    first->offset = 0;
    first->size = N;
  }

  int allocate(int size)
  {
    for (Chunk * it = chunk(0); i; i = next(i))
      if (it->free && dataSize(it) >= size)
        if (dataSize(it) - size < CHUNK_SIZE)
          {
            it->free = false;
            return it->offset + CHUNK_SIZE;
          }
        else
          {
            Chunk * next = chunk(dataOffset(it) + size);
            next->free = true;
            next->offset = dataOffset(it) + size;
            next->size = it->size - (next->offset - it->offset);
            next->prev = it->offset;
            it->free = false;
            it->size = next->offset - it->offset;
            return it->offset + CHUNK_SIZE;
          }
    return -1;
  }

  void free(int offset)
  {
    offset -= CHUNK_SIZE;
    Chunk * chunk = chunk(offset);
    chunk->free = true;
    if (hasPrev(chunk) &&
        hasNext(chunk) &&
        prev(chunk)->free &&
        next(chunk)->free)
      {
        if(hasNext(next(chunk)))
          next(next(chunk))->prev = prev(chunk)->offset;
        prev(chunk)->size += chunk->size + next(chunk)->size;
      }
    else if (hasPrev(chunk) &&
             prev(chunk)->free)
      {
        if(hasNext(chunk))
            next(chunk)->prev = prev(chunk)->offset;
        prev(chunk)->size += chunk->size;
      }
    else if (hasNext(chunk) &&
             next(chunk)->free)
      {
        if (hasNext(next(chunk)))
            next(next(chunk))->prev = chunk->offset;
        chunk->free = true;
        chunk->size += next(chunk)->size;
      }
  }

  std::string info()
  {
    int dataChunks = 0;
    int dataMemory = 0;
    int maxSize = 0;

    for (Chunk * it = chunk(0); it; it = next(it))
        if (it->free)
          {
            int m = dataSize(it);
            maxSize = maxSize > m ? maxSize : m;
          }
        else
          {
            ++dataChunks;
            dataMemory += dataMemory(it);
          }

    std::stringstream ss;
    ss << dataChunks << std::endl;
    ss << dataMemory << std::endl;
    ss << maxSize;
    return ss.str();
  }

  std::string map()
  {
    std::stringstream ss;
    for (Chunk * it = chunk(0); it; it = next(it))
      {
        ss << std::string(CHUNK_SIZE, 'm');
        if(it->free)
          ss << std::string(dataSize(it), 'f');
        else
          ss << std::string(dataSize(it), 'u');
      }
    return ss.str();
  }

private:

  struct Chunk
  {
    bool free;
    d_t offset;
    d_t size;
    d_t prev;
  };


  Chunk * chunk(d_t offset)
  { return reinterpret_cast<Chunk *>(&memory_[0] + offset); }

  Chunk * next(Chunk const * chunk) const
  {
    if (chunk->offset + chunk->size >= N)
      return nullptr;
    return chunk(chunk->offset + chunk->size);
  }

  bool hasNext(Chunk const * chunk) const
  { return chunk->offset + chunk->size < N; }

  bool hasPrev(Chunk const * chunk) const
  { return chunk->offset != 0; }

  Chunk * prev(Chunk const * chunk) const
  {
    if (hasPrev(chunk))
      return chunk(chunk->prev);
    return nullptr;
  }

  int dataSize(Chunk * chunk)
  { return chunk->offset - CHUNK_SIZE; }

  int dataOffset(Chunk * chunk)
  { return chunk->offset + CHUNK_SIZE; }

private:
  size_t N_;
  std::vector<char> memory_;
  static const int CHUNK_SIZE = sizeof(Chunk);
};


int main()
{
  size_t N;
  std::cin >> N;
  Allocator allocator(N);

  while (1)
    {
      std::string input;
      std::cin >> input;
      if (input == "ALLOC")
        {
          int size;
          std::cin >> size;
          int position = allocator.allocate(size);
          if (position > 0)
            std::cout << "+" << position << std::endl;
          else
            std::cout << "-" << std::endl;
        }
      else if (input == "FREE")
        {
          int offset;
          std::cin >> offset;
          allocator.free(offset);
        }
      else if (input == "INFO")
        {
          std::cout << allocator.info() << std::endl;
        }
      else if (input == "MAP")
        {
          std::cout << allocator.map() << std::endl;
        }
      else
        break;
    }
  return 0;
}
