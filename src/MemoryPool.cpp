#include "../include/MemoryPool.h"
#include <assert.h>

namespace Wjy_memoryPool {

/**
 * 使用成员初始化列表设置对象的初始状态
 * 初始化列表：在冒号 : 之后，用逗号分隔的初始化项，
 *            分别对成员变量进行初始化。
 */
MemoryPool::MemoryPool(size_t BlockSize) 
    : BlockSize_ (BlockSize) //每个内存块的大小（通常是操作系统申请的一大块内存）
    , SlotSize_ (0)         //每个槽（slot）大小，即每次分配出去的内存单元大小
    , firstBlock_ (nullptr) //指向第一个内存块的指针，用于 遍历/释放 块
    , curSlot_ (nullptr)    //指向当前可用的槽指针，用于快速分配内存
    , freeList_ (nullptr)   //空闲链表头，用于管理释放的槽
    , lastSlot_ (nullptr)   //指向当前块最后一个槽的指针，用于判断是否达到块末尾
{}

/**
 * 将内存池在运行器件所分配的内存释放掉
 */
MemoryPool::~MemoryPool() {
    //把连续的block删除
    Slot* cur = firstBlock_;
    while (cur) {
        Slot* next = cur->next;
        //operator delete:释放之前需要通过operator new分配内存
        //                注意：new和delete会调用构造和析构函数
        //                而operator new和operator delete则是直接分配和释放内存，不会调用构造和析构函数
        operator delete(reinterpret_cast<void*>(cur));
        cur = next;
    }
}

void MemoryPool::init(size_t size) {
    assert(size > 0);
    SlotSize_ = size;
    firstBlock_ = nullptr;
    curSlot_ = nullptr;
    freeList_ = nullptr;
    lastSlot_ = nullptr;
}

void* MemoryPool::allocate() {
    // 优先使用空闲链表的内存槽
    Slot* slot = popFreeList();
    if (slot != nullptr)
        return slot;
    
    Slot* temp;
    {
        std::lock_guard<std::mutex> lock(mutexForBlock_);
        if(curSlot_ >= lastSlot_) {
            allocateNewBlock();
        }
    }
}


void MemoryPool::allocateNewBlock() {
    //头插法插入新的内存块
    void* newBlock = operator new (BlockSize_);
}

}

