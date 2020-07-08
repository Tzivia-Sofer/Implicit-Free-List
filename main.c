#include <malloc.h>
#include <stdio.h>
#include "memory_allocator.h"

int main()
{
    void* ptrMemory = malloc(1000);
    MemoryAllocator* memory;
    memory = MemoryInit(ptrMemory, 1000);
    void * t = MemoryAllocator_Allocate(memory, 90);
    printf("before %ld\n", *(size_t*)t);
    MemoryAllocatorFree(memory, t);
    printf("after %ld\n", *(size_t*)t);
    void *mem = MemoryAllocator_Release(memory);
    free(mem);
    return  0;
}