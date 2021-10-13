#include <cassert>

#include "pool/chunk_list.hpp"

ChunkList::ChunkList(Arena *arena, ChunkList *nextList, int minUsage, int maxUsage, int chunkSize)
{
}

ChunkList::~ChunkList()
{
}

void ChunkList::prevList(ChunkList *prevList)
{
    assert(_prev == nullptr);
    _prev = prevList;
}