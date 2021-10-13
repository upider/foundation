#ifndef __CHUNK_LIST_HPP__
#define __CHUNK_LIST_HPP__

class Arena;

class ChunkList
{
private:
    ChunkList* _prev;
    ChunkList* _next;

public:
    /**
     * @brief ChunkList 构造函数
     * @param Arena
     * @param ChunkList
     * @param minUsage
     * @param maxUsage
     * @param chunkSize
     */
    ChunkList(Arena* arena, ChunkList* nextList, int minUsage, int maxUsage, int chunkSize);
    ~ChunkList();

    void prevList(ChunkList* prevList);
};

#endif // __CHUNK_LIST_HPP__