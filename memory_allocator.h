#ifndef __MEMORY__
#define __MEMORY__


typedef struct MemoryAllocator MemoryAllocator;

MemoryAllocator*        MemoryInit(void* PtrPoolMemory, size_t size);
void*                   MemoryAllocator_Release(MemoryAllocator* memory);
void*                   MemoryAllocator_Allocate(MemoryAllocator* memory, size_t size);
void                    MemoryAllocatorFree(MemoryAllocator* memory, void* ptr);

#endif  //__MEMORY__
