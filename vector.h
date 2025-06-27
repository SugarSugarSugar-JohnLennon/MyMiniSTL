/*
 * @file vector.h
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL vector容器，完全独立实现，不依赖std
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include "allocator.h"
#include "iterator.h"
#include "type_traits.h"
#include "utility.h"
#include "exceptdef.h"
#include "algorithm.h"
#include <cstddef>
#include <initializer_list>

namespace sugar {

// ============================ vector 类模板 ============================

/**
 * @brief vector 类模板，动态数组容器
 * @tparam T 元素类型
 * @tparam Alloc 分配器类型，默认为sugar::allocator<T>
 */
template<typename T, typename Alloc = allocator<T>>
class vector {
public:
    // ============================ 类型定义 ============================
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = typename allocator_traits<Alloc>::size_type;
    using difference_type = typename allocator_traits<Alloc>::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename allocator_traits<Alloc>::pointer;
    using const_pointer = typename allocator_traits<Alloc>::const_pointer;

    // 迭代器类型
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = sugar::reverse_iterator<iterator>;
    using const_reverse_iterator = sugar::reverse_iterator<const_iterator>;

private:
    // ============================ 私有成员 ============================
    pointer begin_;      // 指向第一个元素
    pointer end_;        // 指向最后一个元素的下一个位置
    pointer end_cap_;    // 指向分配内存的末尾
    allocator_type allocator_; // 分配器

    // ============================ 私有辅助函数 ============================
    
    /**
     * @brief 计算新的容量大小
     * @param new_size 需要的新大小
     * @return 新的容量大小
     */
    size_type calculate_new_capacity(size_type new_size) const {
        size_type old_capacity = capacity();
        if (new_size > old_capacity) {
            // 使用2倍增长策略
            size_type new_capacity = old_capacity == 0 ? 1 : old_capacity * 2;
            if (new_capacity < new_size) {
                new_capacity = new_size;
            }
            return new_capacity;
        }
        return old_capacity;
    }

    /**
     * @brief 重新分配内存
     * @param new_capacity 新的容量
     */
    void reallocate(size_type new_capacity) {
        if (new_capacity == 0) {
            deallocate_all();
            return;
        }

        pointer new_begin = allocator_traits<Alloc>::allocate(allocator_, new_capacity);
        pointer new_end = new_begin;

        try {
            // 移动现有元素到新内存
            for (pointer p = begin_; p != end_; ++p, ++new_end) {
                allocator_traits<Alloc>::construct(allocator_, new_end, sugar::move(*p));
            }
        } catch (...) {
            // 如果构造失败，清理已构造的元素
            for (pointer p = new_begin; p != new_end; ++p) {
                allocator_traits<Alloc>::destroy(allocator_, p);
            }
            allocator_traits<Alloc>::deallocate(allocator_, new_begin, new_capacity);
            throw;
        }

        // 销毁旧元素并释放旧内存
        destroy_all();
        deallocate_all();

        // 更新指针
        begin_ = new_begin;
        end_ = new_end;
        end_cap_ = new_begin + new_capacity;
    }

    /**
     * @brief 销毁所有元素
     */
    void destroy_all() {
        if (begin_) {
            for (pointer p = begin_; p != end_; ++p) {
                allocator_traits<Alloc>::destroy(allocator_, p);
            }
        }
    }

    /**
     * @brief 释放所有内存
     */
    void deallocate_all() {
        if (begin_) {
            allocator_traits<Alloc>::deallocate(allocator_, begin_, capacity());
            begin_ = nullptr;
            end_ = nullptr;
            end_cap_ = nullptr;
        }
    }

    /**
     * @brief 确保有足够容量
     * @param new_size 需要的新大小
     */
    void ensure_capacity(size_type new_size) {
        if (new_size > capacity()) {
            reallocate(calculate_new_capacity(new_size));
        }
    }

public:
    // ============================ 构造函数 ============================
    
    /**
     * @brief 默认构造函数
     */
    vector() noexcept : begin_(nullptr), end_(nullptr), end_cap_(nullptr) {}

    /**
     * @brief 指定分配器的构造函数
     * @param alloc 分配器
     */
    explicit vector(const allocator_type& alloc) noexcept 
        : begin_(nullptr), end_(nullptr), end_cap_(nullptr), allocator_(alloc) {}

    /**
     * @brief 指定大小的构造函数
     * @param count 元素数量
     * @param value 元素值
     * @param alloc 分配器
     */
    explicit vector(size_type count, const T& value = T(), const allocator_type& alloc = allocator_type())
        : allocator_(alloc) {
        if (count > 0) {
            begin_ = allocator_traits<Alloc>::allocate(allocator_, count);
            end_ = begin_;
            end_cap_ = begin_ + count;
            
            try {
                for (; end_ != end_cap_; ++end_) {
                    allocator_traits<Alloc>::construct(allocator_, end_, value);
                }
            } catch (...) {
                destroy_all();
                deallocate_all();
                throw;
            }
        } else {
            begin_ = nullptr;
            end_ = nullptr;
            end_cap_ = nullptr;
        }
    }

    /**
     * @brief 从迭代器范围构造
     * @param first 起始迭代器
     * @param last 结束迭代器
     * @param alloc 分配器
     */
    template<typename InputIt>
    vector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
        : allocator_(alloc) {
        begin_ = nullptr;
        end_ = nullptr;
        end_cap_ = nullptr;
        
        assign(first, last);
    }

    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的vector
     */
    vector(const vector& other) : allocator_(other.allocator_) {
        begin_ = nullptr;
        end_ = nullptr;
        end_cap_ = nullptr;
        
        if (!other.empty()) {
            size_type count = other.size();
            begin_ = allocator_traits<Alloc>::allocate(allocator_, count);
            end_ = begin_;
            end_cap_ = begin_ + count;
            
            try {
                for (const_iterator it = other.begin(); it != other.end(); ++it, ++end_) {
                    allocator_traits<Alloc>::construct(allocator_, end_, *it);
                }
            } catch (...) {
                destroy_all();
                deallocate_all();
                throw;
            }
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 要移动的vector
     */
    vector(vector&& other) noexcept 
        : begin_(other.begin_), end_(other.end_), end_cap_(other.end_cap_), allocator_(sugar::move(other.allocator_)) {
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.end_cap_ = nullptr;
    }

    /**
     * @brief 初始化列表构造函数
     * @param init 初始化列表
     * @param alloc 分配器
     */
    vector(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : allocator_(alloc) {
        begin_ = nullptr;
        end_ = nullptr;
        end_cap_ = nullptr;
        
        assign(init.begin(), init.end());
    }

    // ============================ 析构函数 ============================
    
    /**
     * @brief 析构函数
     */
    ~vector() {
        destroy_all();
        deallocate_all();
    }

    // ============================ 赋值操作符 ============================
    
    /**
     * @brief 拷贝赋值操作符
     * @param other 要拷贝的vector
     * @return 当前vector的引用
     */
    vector& operator=(const vector& other) {
        if (this != &other) {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    /**
     * @brief 移动赋值操作符
     * @param other 要移动的vector
     * @return 当前vector的引用
     */
    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            destroy_all();
            deallocate_all();
            
            begin_ = other.begin_;
            end_ = other.end_;
            end_cap_ = other.end_cap_;
            allocator_ = sugar::move(other.allocator_);
            
            other.begin_ = nullptr;
            other.end_ = nullptr;
            other.end_cap_ = nullptr;
        }
        return *this;
    }

    /**
     * @brief 初始化列表赋值操作符
     * @param init 初始化列表
     * @return 当前vector的引用
     */
    vector& operator=(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
        return *this;
    }

    // ============================ 赋值函数 ============================
    
    /**
     * @brief 赋值指定数量的元素
     * @param count 元素数量
     * @param value 元素值
     */
    void assign(size_type count, const T& value) {
        clear();
        if (count > 0) {
            ensure_capacity(count);
            for (size_type i = 0; i < count; ++i) {
                allocator_traits<Alloc>::construct(allocator_, end_, value);
                ++end_;
            }
        }
    }

    /**
     * @brief 从迭代器范围赋值
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template<typename InputIt, typename = typename sugar::enable_if<!sugar::is_integral<InputIt>::value>::type>
    void assign(InputIt first, InputIt last) {
        clear();
        for (; first != last; ++first) {
            push_back(*first);
        }
    }

    /**
     * @brief 从初始化列表赋值
     * @param init 初始化列表
     */
    void assign(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
    }

    // ============================ 分配器访问 ============================
    
    /**
     * @brief 获取分配器
     * @return 分配器的引用
     */
    const allocator_type& get_allocator() const noexcept {
        return allocator_;
    }

    // ============================ 元素访问 ============================
    
    /**
     * @brief 访问指定位置的元素
     * @param pos 位置索引
     * @return 元素的引用
     */
    reference at(size_type pos) {
        SUGAR_THROW_OUT_OF_RANGE_IF(pos >= size(), "vector::at - index out of range");
        return begin_[pos];
    }

    /**
     * @brief 访问指定位置的元素（const版本）
     * @param pos 位置索引
     * @return 元素的const引用
     */
    const_reference at(size_type pos) const {
        SUGAR_THROW_OUT_OF_RANGE_IF(pos >= size(), "vector::at - index out of range");
        return begin_[pos];
    }

    /**
     * @brief 访问指定位置的元素（无边界检查）
     * @param pos 位置索引
     * @return 元素的引用
     */
    reference operator[](size_type pos) {
        return begin_[pos];
    }

    /**
     * @brief 访问指定位置的元素（无边界检查，const版本）
     * @param pos 位置索引
     * @return 元素的const引用
     */
    const_reference operator[](size_type pos) const {
        return begin_[pos];
    }

    /**
     * @brief 访问第一个元素
     * @return 第一个元素的引用
     */
    reference front() {
        SUGAR_THROW_OUT_OF_RANGE_IF(empty(), "vector::front - vector is empty");
        return *begin_;
    }

    /**
     * @brief 访问第一个元素（const版本）
     * @return 第一个元素的const引用
     */
    const_reference front() const {
        SUGAR_THROW_OUT_OF_RANGE_IF(empty(), "vector::front - vector is empty");
        return *begin_;
    }

    /**
     * @brief 访问最后一个元素
     * @return 最后一个元素的引用
     */
    reference back() {
        SUGAR_THROW_OUT_OF_RANGE_IF(empty(), "vector::back - vector is empty");
        return *(end_ - 1);
    }

    /**
     * @brief 访问最后一个元素（const版本）
     * @return 最后一个元素的const引用
     */
    const_reference back() const {
        SUGAR_THROW_OUT_OF_RANGE_IF(empty(), "vector::back - vector is empty");
        return *(end_ - 1);
    }

    /**
     * @brief 获取指向数据的指针
     * @return 指向数据的指针
     */
    pointer data() noexcept {
        return begin_;
    }

    /**
     * @brief 获取指向数据的指针（const版本）
     * @return 指向数据的const指针
     */
    const_pointer data() const noexcept {
        return begin_;
    }

    // ============================ 迭代器 ============================
    
    /**
     * @brief 获取起始迭代器
     * @return 起始迭代器
     */
    iterator begin() noexcept {
        return begin_;
    }

    /**
     * @brief 获取起始迭代器（const版本）
     * @return 起始const迭代器
     */
    const_iterator begin() const noexcept {
        return begin_;
    }

    /**
     * @brief 获取起始迭代器（const版本）
     * @return 起始const迭代器
     */
    const_iterator cbegin() const noexcept {
        return begin_;
    }

    /**
     * @brief 获取结束迭代器
     * @return 结束迭代器
     */
    iterator end() noexcept {
        return end_;
    }

    /**
     * @brief 获取结束迭代器（const版本）
     * @return 结束const迭代器
     */
    const_iterator end() const noexcept {
        return end_;
    }

    /**
     * @brief 获取结束迭代器（const版本）
     * @return 结束const迭代器
     */
    const_iterator cend() const noexcept {
        return end_;
    }

    /**
     * @brief 获取反向起始迭代器
     * @return 反向起始迭代器
     */
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    /**
     * @brief 获取反向起始迭代器（const版本）
     * @return 反向起始const迭代器
     */
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    /**
     * @brief 获取反向起始迭代器（const版本）
     * @return 反向起始const迭代器
     */
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    /**
     * @brief 获取反向结束迭代器
     * @return 反向结束迭代器
     */
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    /**
     * @brief 获取反向结束迭代器（const版本）
     * @return 反向结束const迭代器
     */
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    /**
     * @brief 获取反向结束迭代器（const版本）
     * @return 反向结束const迭代器
     */
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // ============================ 容量 ============================
    
    /**
     * @brief 检查容器是否为空
     * @return 如果容器为空返回true，否则返回false
     */
    bool empty() const noexcept {
        return begin_ == end_;
    }

    /**
     * @brief 获取容器大小
     * @return 容器中元素的数量
     */
    size_type size() const noexcept {
        return static_cast<size_type>(end_ - begin_);
    }

    /**
     * @brief 获取最大大小
     * @return 容器可以容纳的最大元素数量
     */
    size_type max_size() const noexcept {
        return allocator_traits<Alloc>::max_size(allocator_);
    }

    /**
     * @brief 预留容量
     * @param new_cap 新的容量
     */
    void reserve(size_type new_cap) {
        if (new_cap > capacity()) {
            reallocate(new_cap);
        }
    }

    /**
     * @brief 获取当前容量
     * @return 当前分配的容量
     */
    size_type capacity() const noexcept {
        return static_cast<size_type>(end_cap_ - begin_);
    }

    /**
     * @brief 收缩容量到大小
     */
    void shrink_to_fit() {
        if (size() < capacity()) {
            reallocate(size());
        }
    }

    // ============================ 修改器 ============================
    
    /**
     * @brief 清空容器
     */
    void clear() noexcept {
        destroy_all();
        end_ = begin_;
    }

    /**
     * @brief 在指定位置插入元素
     * @param pos 插入位置
     * @param value 要插入的值
     * @return 指向插入元素的迭代器
     */
    iterator insert(const_iterator pos, const T& value) {
        return insert(pos, 1, value);
    }

    /**
     * @brief 在指定位置插入元素（移动版本）
     * @param pos 插入位置
     * @param value 要插入的值
     * @return 指向插入元素的迭代器
     */
    iterator insert(const_iterator pos, T&& value) {
        size_type offset = pos - begin();
        ensure_capacity(size() + 1);
        
        iterator insert_pos = begin() + offset;
        
        // 移动后面的元素
        for (iterator it = end(); it != insert_pos; --it) {
            *it = sugar::move(*(it - 1));
        }
        
        // 构造新元素
        allocator_traits<Alloc>::construct(allocator_, insert_pos, sugar::move(value));
        ++end_;
        
        return insert_pos;
    }

    /**
     * @brief 在指定位置插入指定数量的元素
     * @param pos 插入位置
     * @param count 要插入的元素数量
     * @param value 要插入的值
     * @return 指向第一个插入元素的迭代器
     */
    iterator insert(const_iterator pos, size_type count, const T& value) {
        if (count == 0) {
            return const_cast<iterator>(pos);
        }
        
        size_type offset = pos - begin();
        ensure_capacity(size() + count);
        
        iterator insert_pos = begin() + offset;
        
        // 移动后面的元素
        for (iterator it = end(); it != insert_pos; --it) {
            *(it + count - 1) = sugar::move(*(it - 1));
        }
        
        // 构造新元素
        for (size_type i = 0; i < count; ++i) {
            allocator_traits<Alloc>::construct(allocator_, insert_pos + i, value);
        }
        
        end_ += count;
        return insert_pos;
    }

    /**
     * @brief 在指定位置插入迭代器范围的元素
     * @param pos 插入位置
     * @param first 起始迭代器
     * @param last 结束迭代器
     * @return 指向第一个插入元素的迭代器
     */
    template<typename InputIt, typename = typename sugar::enable_if<!sugar::is_integral<InputIt>::value>::type>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        size_type offset = pos - begin();
        size_type count = 0;
        
        // 计算插入的元素数量
        for (InputIt it = first; it != last; ++it) {
            ++count;
        }
        
        if (count == 0) {
            return const_cast<iterator>(pos);
        }
        
        ensure_capacity(size() + count);
        
        iterator insert_pos = begin() + offset;
        
        // 移动后面的元素
        for (iterator it = end(); it != insert_pos; --it) {
            *(it + count - 1) = sugar::move(*(it - 1));
        }
        
        // 构造新元素
        iterator current = insert_pos;
        for (InputIt it = first; it != last; ++it, ++current) {
            allocator_traits<Alloc>::construct(allocator_, current, *it);
        }
        
        end_ += count;
        return insert_pos;
    }

    /**
     * @brief 在指定位置插入初始化列表
     * @param pos 插入位置
     * @param ilist 初始化列表
     * @return 指向第一个插入元素的迭代器
     */
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    /**
     * @brief 在末尾添加元素
     * @param value 要添加的值
     */
    void push_back(const T& value) {
        ensure_capacity(size() + 1);
        allocator_traits<Alloc>::construct(allocator_, end_, value);
        ++end_;
    }

    /**
     * @brief 在末尾添加元素（移动版本）
     * @param value 要添加的值
     */
    void push_back(T&& value) {
        ensure_capacity(size() + 1);
        allocator_traits<Alloc>::construct(allocator_, end_, sugar::move(value));
        ++end_;
    }

    /**
     * @brief 移除末尾元素
     */
    void pop_back() {
        if (!empty()) {
            --end_;
            allocator_traits<Alloc>::destroy(allocator_, end_);
        }
    }

    /**
     * @brief 调整容器大小
     * @param count 新的大小
     * @param value 用于填充的值
     */
    void resize(size_type count, const T& value = T()) {
        if (count > size()) {
            ensure_capacity(count);
            for (size_type i = size(); i < count; ++i) {
                allocator_traits<Alloc>::construct(allocator_, end_, value);
                ++end_;
            }
        } else if (count < size()) {
            size_type old_size = size();  // Cache the old size
            for (size_type i = count; i < old_size; ++i) {
                --end_;
                allocator_traits<Alloc>::destroy(allocator_, end_);
            }
        }
    }

    /**
     * @brief 交换两个vector
     * @param other 要交换的vector
     */
    void swap(vector& other) noexcept {
        sugar::swap(begin_, other.begin_);
        sugar::swap(end_, other.end_);
        sugar::swap(end_cap_, other.end_cap_);
        sugar::swap(allocator_, other.allocator_);
    }

    /**
     * @brief 擦除指定位置的元素
     * @param pos 要擦除的位置
     * @return 指向下一个元素的迭代器
     */
    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    /**
     * @brief 擦除指定范围的元素
     * @param first 起始位置
     * @param last 结束位置
     * @return 指向下一个元素的迭代器
     */
    iterator erase(const_iterator first, const_iterator last) {
        if (first == last) {
            return const_cast<iterator>(first);
        }
        
        iterator erase_first = const_cast<iterator>(first);
        iterator erase_last = const_cast<iterator>(last);
        
        // 移动后面的元素
        for (iterator it = erase_last; it != end(); ++it) {
            *(it - (erase_last - erase_first)) = sugar::move(*it);
        }
        
        // 销毁被擦除的元素
        for (iterator it = erase_last - (erase_last - erase_first); it != end(); ++it) {
            allocator_traits<Alloc>::destroy(allocator_, it);
        }
        
        end_ -= (erase_last - erase_first);
        return erase_first;
    }
};

// ============================ 非成员函数 ============================

/**
 * @brief 比较两个vector是否相等
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果相等返回true，否则返回false
 */
template<typename T, typename Alloc>
bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    
    for (typename vector<T, Alloc>::size_type i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief 比较两个vector是否不相等
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果不相等返回true，否则返回false
 */
template<typename T, typename Alloc>
bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief 比较两个vector的大小关系
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果lhs小于rhs返回true，否则返回false
 */
template<typename T, typename Alloc>
bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return sugar::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/**
 * @brief 比较两个vector的大小关系
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果lhs小于等于rhs返回true，否则返回false
 */
template<typename T, typename Alloc>
bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief 比较两个vector的大小关系
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果lhs大于rhs返回true，否则返回false
 */
template<typename T, typename Alloc>
bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return rhs < lhs;
}

/**
 * @brief 比较两个vector的大小关系
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return 如果lhs大于等于rhs返回true，否则返回false
 */
template<typename T, typename Alloc>
bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) {
    return !(lhs < rhs);
}

/**
 * @brief 交换两个vector
 * @param lhs 左操作数
 * @param rhs 右操作数
 */
template<typename T, typename Alloc>
void swap(vector<T, Alloc>& lhs, vector<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace sugar

#endif // VECTOR_H_ 