/*
 * @file allocator.h
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL 内存分配器系统，完全独立实现，不依赖std
 */

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include "type_traits.h"
#include "exceptdef.h"
#include "utility.h"
#include <cstddef>
#include <new>

namespace sugar {

// ============================ 基础内存操作 ============================

/**
 * @brief 内存分配函数
 */
inline void* allocate(size_t size) {
    void* ptr = ::operator new(size);
    if (!ptr) {
        SUGAR_THROW_BAD_ALLOC_IF(true, "Memory allocation failed");
    }
    return ptr;
}

/**
 * @brief 内存释放函数
 */
inline void deallocate(void* ptr, size_t /* size */) {
    ::operator delete(ptr);
}

/**
 * @brief 内存对齐分配（简化版本，C++11兼容）
 */
inline void* allocate_aligned(size_t size, size_t alignment) {
    // 简化实现，只处理基本对齐
    if (alignment <= sizeof(void*)) {
        return allocate(size);
    }
    // 对于大对齐，使用普通分配（实际项目中可以使用posix_memalign等）
    return allocate(size);
}

/**
 * @brief 内存对齐释放（简化版本，C++11兼容）
 */
inline void deallocate_aligned(void* ptr, size_t /* size */, size_t alignment) {
    if (alignment <= sizeof(void*)) {
        deallocate(ptr, 0);
    } else {
        deallocate(ptr, 0);
    }
}

// ============================ 对象构造和析构 ============================

/**
 * @brief 构造对象
 */
template<typename T, typename... Args>
void construct(T* ptr, Args&&... args) {
    ::new(static_cast<void*>(ptr)) T(sugar::forward<Args>(args)...);
}

/**
 * @brief 析构对象
 */
template<typename T>
void destroy(T* ptr) {
    ptr->~T();
}

/**
 * @brief 批量析构对象
 */
template<typename ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last) {
    for (; first != last; ++first) {
        destroy(&*first);
    }
}

/**
 * @brief 批量析构对象（特化版本）
 */
template<typename T>
void destroy(T* first, T* last) {
    for (; first != last; ++first) {
        destroy(first);
    }
}

// ============================ 未初始化内存操作 ============================

/**
 * @brief 未初始化内存复制
 */
template<typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    ForwardIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            construct(&*cur, *first);
        }
    } catch (...) {
        destroy(result, cur);
        throw;
    }
    return cur;
}

/**
 * @brief 未初始化内存填充
 */
template<typename ForwardIterator, typename T>
ForwardIterator uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value) {
    ForwardIterator cur = first;
    try {
        for (; cur != last; ++cur) {
            construct(&*cur, value);
        }
    } catch (...) {
        destroy(first, cur);
        throw;
    }
    return cur;
}

/**
 * @brief 未初始化内存填充n个元素
 */
template<typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& value) {
    ForwardIterator cur = first;
    try {
        for (; n > 0; --n, ++cur) {
            construct(&*cur, value);
        }
    } catch (...) {
        destroy(first, cur);
        throw;
    }
    return cur;
}

// ============================ 标准分配器 ============================

/**
 * @brief 标准分配器
 */
template<typename T>
class allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal = true_type;

    /**
     * @brief 构造函数
     */
    allocator() noexcept {}
    template<typename U>
    allocator(const allocator<U>&) noexcept {}
    ~allocator() noexcept {}

    /**
     * @brief 分配内存
     */
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        if (n > max_size()) {
            SUGAR_THROW_LENGTH_ERROR_IF(true, "Requested allocation size exceeds maximum");
        }
        return static_cast<pointer>(sugar::allocate(n * sizeof(T)));
    }

    /**
     * @brief 释放内存
     */
    void deallocate(pointer ptr, size_type /* n */) noexcept {
        if (ptr) {
            sugar::deallocate(ptr, 0);
        }
    }

    /**
     * @brief 最大分配大小
     */
    size_type max_size() const noexcept {
        return size_type(-1) / sizeof(T);
    }

    /**
     * @brief 构造对象
     */
    template<typename U, typename... Args>
    void construct(U* ptr, Args&&... args) {
        sugar::construct(ptr, sugar::forward<Args>(args)...);
    }

    /**
     * @brief 析构对象
     */
    template<typename U>
    void destroy(U* ptr) {
        sugar::destroy(ptr);
    }
};

// 分配器比较操作（在类外定义，避免重复定义）
template<typename T1, typename T2>
bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept {
    return true;
}

template<typename T1, typename T2>
bool operator!=(const allocator<T1>& lhs, const allocator<T2>& rhs) noexcept {
    return !(lhs == rhs);
}

// ============================ allocator_traits ============================

/**
 * allocator_traits - 分配器特征
 */
template<typename Alloc>
struct allocator_traits {
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;
    using pointer = typename Alloc::pointer;
    using const_pointer = typename Alloc::const_pointer;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using difference_type = typename Alloc::difference_type;
    using size_type = typename Alloc::size_type;
    using propagate_on_container_copy_assignment = false_type;
    using propagate_on_container_move_assignment = typename Alloc::propagate_on_container_move_assignment;
    using propagate_on_container_swap = false_type;
    using is_always_equal = typename Alloc::is_always_equal;

    // 分配内存
    static pointer allocate(Alloc& a, size_type n) {
        return a.allocate(n);
    }

    // 释放内存
    static void deallocate(Alloc& a, pointer p, size_type n) noexcept {
        a.deallocate(p, n);
    }

    // 构造对象
    template<typename T, typename... Args>
    static void construct(Alloc& a, T* p, Args&&... args) {
        a.construct(p, sugar::forward<Args>(args)...);
    }

    // 析构对象
    template<typename T>
    static void destroy(Alloc& a, T* p) {
        a.destroy(p);
    }

    // 最大分配大小
    static size_type max_size(const Alloc& a) noexcept {
        return a.max_size();
    }

    // 选择分配器
    template<typename T>
    using rebind_alloc = allocator<T>;
};

// ============================ 内存池分配器 ============================

/**
 * 简单的内存池分配器
 * 用于小对象的快速分配
 */
template<typename T, size_t BlockSize = 4096>
class pool_allocator {
private:
    struct Block {
        Block* next;
        char data[BlockSize];
    };

    struct FreeNode {
        FreeNode* next;
    };

    Block* current_block_;
    FreeNode* free_list_;
    size_t block_offset_;

    void allocate_block() {
        Block* new_block = static_cast<Block*>(sugar::allocate(sizeof(Block)));
        new_block->next = current_block_;
        current_block_ = new_block;
        block_offset_ = 0;
    }

public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;
    using is_always_equal = false_type;

    pool_allocator() : current_block_(nullptr), free_list_(nullptr), block_offset_(0) {}
    ~pool_allocator() {
        while (current_block_) {
            Block* next = current_block_->next;
            sugar::deallocate(current_block_, sizeof(Block));
            current_block_ = next;
        }
    }
    // 禁止拷贝和移动
    pool_allocator(const pool_allocator&) = delete;
    pool_allocator& operator=(const pool_allocator&) = delete;
    pool_allocator(pool_allocator&&) = delete;
    pool_allocator& operator=(pool_allocator&&) = delete;

    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        if (n > max_size()) {
            SUGAR_THROW_LENGTH_ERROR_IF(true, "Requested allocation size exceeds maximum");
        }

        size_t size = n * sizeof(T);
        
        // 对于小对象，使用内存池
        if (size <= BlockSize / 4) {
            // 首先检查空闲列表
            if (free_list_) {
                pointer result = reinterpret_cast<pointer>(free_list_);
                free_list_ = free_list_->next;
                return result;
            }

            // 检查当前块是否有足够空间
            if (!current_block_ || block_offset_ + size > BlockSize) {
                allocate_block();
            }

            // 确保内存对齐
            size_t alignment = alignof(T);
            size_t aligned_offset = (block_offset_ + alignment - 1) & ~(alignment - 1);
            
            if (aligned_offset + size > BlockSize) {
                allocate_block();
                aligned_offset = 0;
            }

            pointer result = reinterpret_cast<pointer>(current_block_->data + aligned_offset);
            block_offset_ = aligned_offset + size;
            return result;
        }

        // 对于大对象，直接分配
        return static_cast<pointer>(sugar::allocate(size));
    }

    void deallocate(pointer ptr, size_type n) noexcept {
        if (!ptr) return;
        size_t size = n * sizeof(T);
        // 判断 ptr 是否属于 Block 链表
        bool in_block = false;
        for (Block* blk = current_block_; blk != nullptr; blk = blk->next) {
            char* blk_start = reinterpret_cast<char*>(blk->data);
            char* blk_end = blk_start + BlockSize;
            if (reinterpret_cast<char*>(ptr) >= blk_start && reinterpret_cast<char*>(ptr) < blk_end) {
                in_block = true;
                break;
            }
        }
        if (!in_block && size > BlockSize / 4) {
            sugar::deallocate(ptr, size);
        }
        // 属于 Block 的小对象什么都不做
    }

    size_type max_size() const noexcept {
        return size_type(-1) / sizeof(T);
    }

    template<typename U, typename... Args>
    void construct(U* ptr, Args&&... args) {
        sugar::construct(ptr, sugar::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* ptr) {
        sugar::destroy(ptr);
    }
};

// ============================ 类型别名 ============================

template<typename T>
using allocator_t = allocator<T>;

template<typename T, size_t BlockSize = 4096>
using pool_allocator_t = pool_allocator<T, BlockSize>;

// ============================ 便捷函数 ============================

/**
 * 创建对象
 */
template<typename T, typename Alloc, typename... Args>
T* create_object(Alloc& alloc, Args&&... args) {
    T* ptr = allocator_traits<Alloc>::allocate(alloc, 1);
    try {
        allocator_traits<Alloc>::construct(alloc, ptr, sugar::forward<Args>(args)...);
    } catch (...) {
        allocator_traits<Alloc>::deallocate(alloc, ptr, 1);
        throw;
    }
    return ptr;
}

/**
 * 销毁对象
 */
template<typename T, typename Alloc>
void destroy_object(Alloc& alloc, T* ptr) {
    if (ptr) {
        allocator_traits<Alloc>::destroy(alloc, ptr);
        allocator_traits<Alloc>::deallocate(alloc, ptr, 1);
    }
}

} // namespace sugar

#endif // ALLOCATOR_H_ 