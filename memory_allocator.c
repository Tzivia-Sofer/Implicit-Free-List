#include <malloc.h>
#include <stdbool.h>
#include "memory_allocator.h"

static const size_t CAUGHT = 1;
static const size_t FREE = 0;
static const size_t MASK =  ~ (size_t)1;
static const unsigned char ALIGMENT = 16;


struct MemoryAllocator{
    void* Memory;
    size_t SizeOfMemory;
};

// ================== AUXILIARY FUNCTIONS ===========================

void InitMetaData(void *MetaData, size_t size, bool caught)
{
    *(size_t*)MetaData = size;

    if(caught)
    {
        *(size_t*)MetaData = *(size_t*)MetaData | caught;
    }

    else
    {
        *(size_t *) MetaData = *(size_t *) MetaData & MASK;
    }
}

void* GetPayload(void* ptr)
{
    return (void*)((char*)ptr + sizeof(size_t));
}

bool EndOfMemory(MemoryAllocator *memory, void* ptr)
{
    return (char*)memory->Memory + memory->SizeOfMemory > ptr;
}

bool IsCaught(void* ptr)
{
    return (*(size_t*)ptr) & CAUGHT;
}

size_t FindBlockSize(void* ptr)
{
    return *((size_t*)ptr) & MASK;
}

void split(void* ptr, size_t size)
{
    size_t CurrentSize = FindBlockSize(ptr);
    InitMetaData(ptr, size , CAUGHT);
    InitMetaData(ptr + size + sizeof(size_t), CurrentSize - size - sizeof(size_t), FREE);

}

size_t AlignedSize(size_t size)
{
    if (size % ALIGMENT != 0)
    {
        size = (size / ALIGMENT + 1) * ALIGMENT;
    }
    return size;
}

void MergeAndInit(void* ptr)
{
    void *NextBlock = ptr + FindBlockSize(ptr) + sizeof(size_t);
    size_t NewBlockSize = FindBlockSize(ptr) + FindBlockSize(NextBlock);
    InitMetaData(ptr, NewBlockSize , FREE);
}

// ==================END AUXILIARY FUNCTIONS========================

MemoryAllocator* MemoryInit(void* PtrPoolMemory, size_t size)
{
    MemoryAllocator* memoryAllocator = (MemoryAllocator*)malloc(sizeof(MemoryAllocator));
    memoryAllocator->Memory = PtrPoolMemory;
    memoryAllocator->SizeOfMemory = size;
    InitMetaData(memoryAllocator, size, FREE);

    return memoryAllocator;
}

void* MemoryAllocator_Release(MemoryAllocator* memory)
{
    void *ptr = memory->Memory;  
    free(memory);
    
	return ptr;
}

void* MemoryAllocator_Allocate(MemoryAllocator* memory, size_t size)
{
    void *ptr = memory;
    size  = AlignedSize(size);

    if (size > memory->SizeOfMemory)
    {
        return NULL;
    }

    while (!EndOfMemory(memory, ptr))
    {
        if (IsCaught(ptr))
        {
            ptr += FindBlockSize(ptr) + sizeof(size_t);
        }

        else if (FindBlockSize(ptr) < size)
        {
            // free small space
            void* NextPtr = ptr + FindBlockSize(ptr) + sizeof(size_t);

            // merge two free blocks and check again current ptr
            if(!IsCaught(NextPtr))
            {
                InitMetaData(ptr, FindBlockSize(ptr) + FindBlockSize(NextPtr) + sizeof(size_t), FREE);
            }

            else
            {
                ptr = NextPtr;
            }
        }

        // enough place to allocate
        else
        {
            split(ptr, size);
            return GetPayload(ptr);  
        }
    }
    return NULL;
}

void MemoryAllocatorFree(MemoryAllocator* memory, void* ptr)
{
    void *NextBlock = ptr + FindBlockSize(ptr) + sizeof(size_t);

    if (!IsCaught(NextBlock))
    {
        MergeAndInit(ptr);
    }

    else
    {
        InitMetaData(ptr, FindBlockSize(ptr), FREE);
    }
}
