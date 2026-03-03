#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__

//这个头文件<atomic>包含了C++11引入的原子操作相关的类和函数，可以用于实现线程安全的内存池。通过使用std::atomic，可以确保在多线程环境下对内存池的访问是安全的，避免了数据竞争和其他并发问题。
#include <atomic>
//这个头文件<cassert>包含了C++标准库中的断言宏，可以用于在代码中进行调试和错误检查。通过使用assert宏，可以在运行时检查某些条件是否为真，如果条件不满足，程序会抛出一个错误并终止执行。这对于确保内存池的正确性和稳定性非常有帮助。
#include <cassert>
//这个头文件<cstdint>包含了C++标准库中的固定宽度整数类型定义，如int32_t、uint64_t等。这些类型提供了明确的位数和符号，可以用于在内存池中存储和管理数据，确保数据的一致性和可移植性。
#include <cstdint>
//这个头文件<iostream>包含了C++标准库中的输入输出流类，如std::cout和std::cerr等。通过使用这些类，可以在内存池的实现中输出调试信息、错误消息或其他相关信息，帮助开发者更好地理解和调试内存池的行为。
#include <iostream>
//这个头文件<memory>包含了C++标准库中的智能指针类，如std::shared_ptr和std::unique_ptr等。通过使用智能指针，可以更安全地管理内存池中的资源，避免内存泄漏和悬空指针等问题。智能指针会自动释放所管理的资源，确保内存池的稳定性和可靠性。
#include <memory>
//这个头文件<mutex>包含了C++标准库中的互斥锁类，如std::mutex和std::lock_guard等。通过使用互斥锁，可以在多线程环境下保护内存池的共享资源，确保线程安全。互斥锁可以防止多个线程同时访问内存池，避免数据竞争和其他并发问题，从而提高内存池的稳定性和可靠性。
#include <mutex>

/**
 * @param MEMORY_POOL_NUM 内存池的数量，表示内存池中可以管理的不同大小的内存块的数量。
 * @param SLOT_BASE_SIZE 内存块的基本大小，表示内存池中最小的内存块的大小。内存池将根据这个基本大小来划分内存块，以满足不同大小的内存分配需求。
 * @param MAX_SLOT_SIZE 内存块的最大大小，表示内存池中最大的内存块的大小。内存池将根据这个最大大小来限制内存块的分配，以确保内存池的效率和性能。
 */


namespace Wjy_memoryPool {

#define MEMORY_POOL_NUM 64
#define SLOT_BASE_SIZE 8
#define MAX_SLOT_SIZE 512

struct Slot {
    std::atomic<Slot*> next;
};

class MemoryPool {
public:
    MemoryPool(size_t BlockSize = 4096);
    ~MemoryPool();

    void init(size_t);
    
    void* allocate();
    void deallocate(void*);

private:
    void allocateNewBlock();
    size_t padPointer(int* p, size_t align);

    //使用CAS操作进行无锁入队和出队
    bool pushFreeList(Slot* slot);
    Slot* popFreeList();

private:
    int             BlockSize_;         //内存块大小
    int             SlotSize_;          //槽大小
    Slot*           firstBlock_;        //指向内存池管理的首个实际内存块
    Slot*           curSlot_;           //指向当前未被使用过的槽
    std::atomic<Slot*>    freeList_;    //指向空闲的槽（使用过后被释放的槽）
    Slot*           lastSlot_;          //作为当前内存块中最后能够存放元素的位置表示（超过该位置需要申请新内存块）

    std::mutex      mutexForBlock_;     // 保证多线程情况下避免不必要的重复开销内存导致的浪费行为

};



}


#endif // __MEMORYPOOL_H__