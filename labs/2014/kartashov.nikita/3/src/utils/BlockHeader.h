#ifndef BLOCK_HEADER_H_
#define BLOCK_HEADER_H_

struct BlockHeader
{
  int id;
  int next;
  size_t occupied;
};

#endif /* end of include guard */
