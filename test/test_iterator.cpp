#include "../iterator.h"
#include <iostream>

// 简单的测试容器类
template<typename T>
class SimpleArray {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    // 迭代器类
    class iterator : public sugar::iterator<sugar::random_access_iterator_tag, T> {
    private:
        T* ptr_;

    public:
        iterator(T* ptr = nullptr) : ptr_(ptr) {}

        reference operator*() { return *ptr_; }
        pointer operator->() { return ptr_; }
        
        iterator& operator++() { ++ptr_; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++ptr_; return tmp; }
        iterator& operator--() { --ptr_; return *this; }
        iterator operator--(int) { iterator tmp = *this; --ptr_; return tmp; }
        
        iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
        iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }
        iterator operator+(difference_type n) const { return iterator(ptr_ + n); }
        iterator operator-(difference_type n) const { return iterator(ptr_ - n); }
        difference_type operator-(const iterator& other) const { return ptr_ - other.ptr_; }
        
        reference operator[](difference_type n) { return ptr_[n]; }
        
        bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const iterator& other) const { return ptr_ < other.ptr_; }
        bool operator<=(const iterator& other) const { return ptr_ <= other.ptr_; }
        bool operator>(const iterator& other) const { return ptr_ > other.ptr_; }
        bool operator>=(const iterator& other) const { return ptr_ >= other.ptr_; }
    };

    using const_iterator = iterator; // 简化版本

    // 构造函数
    SimpleArray(size_t capacity = 10) : capacity_(capacity), size_(0) {
        data_ = new T[capacity_];
    }

    ~SimpleArray() {
        delete[] data_;
    }

    // 基本操作
    void push_back(const T& value) {
        if (size_ < capacity_) {
            data_[size_++] = value;
        }
    }

    void push_front(const T& value) {
        if (size_ < capacity_) {
            for (size_t i = size_; i > 0; --i) {
                data_[i] = data_[i-1];
            }
            data_[0] = value;
            ++size_;
        }
    }

    iterator insert(iterator pos, const T& value) {
        size_t index = pos - begin();
        if (size_ < capacity_) {
            for (size_t i = size_; i > index; --i) {
                data_[i] = data_[i-1];
            }
            data_[index] = value;
            ++size_;
        }
        return begin() + index;
    }

    iterator begin() { return iterator(data_); }
    iterator end() { return iterator(data_ + size_); }
    const_iterator begin() const { return iterator(data_); }
    const_iterator end() const { return iterator(data_ + size_); }

    size_t size() const { return size_; }
    size_t max_size() const { return capacity_; }
};

void test_iterator_traits() {
    std::cout << "\n=== 测试 iterator_traits ===" << std::endl;
    
    // 测试指针类型
    using int_ptr_traits = sugar::iterator_traits<int*>;
    std::cout << "int* 的迭代器分类: " 
              << (sugar::is_same<int_ptr_traits::iterator_category, 
                                sugar::random_access_iterator_tag>::value ? "random_access" : "other") 
              << std::endl;
    
    // 测试自定义迭代器
    SimpleArray<int> arr;
    using arr_iter_traits = sugar::iterator_traits<SimpleArray<int>::iterator>;
    std::cout << "SimpleArray<int>::iterator 的迭代器分类: " 
              << (sugar::is_same<arr_iter_traits::iterator_category, 
                                sugar::random_access_iterator_tag>::value ? "random_access" : "other") 
              << std::endl;
}

void test_distance_and_advance() {
    std::cout << "\n=== 测试 distance 和 advance ===" << std::endl;
    
    SimpleArray<int> arr;
    for (int i = 0; i < 5; ++i) {
        arr.push_back(i);
    }
    
    auto first = arr.begin();
    auto last = arr.end();
    
    // 测试 distance
    auto dist = sugar::distance(first, last);
    std::cout << "distance(first, last) = " << dist << std::endl;
    
    // 测试 advance
    auto it = arr.begin();
    sugar::advance(it, 2);
    std::cout << "advance(begin(), 2) 指向的值: " << *it << std::endl;
    
    // 测试 next 和 prev
    auto next_it = sugar::next(arr.begin(), 3);
    std::cout << "next(begin(), 3) 指向的值: " << *next_it << std::endl;
    
    auto prev_it = sugar::prev(arr.end(), 1);
    std::cout << "prev(end(), 1) 指向的值: " << *prev_it << std::endl;
}

void test_reverse_iterator() {
    std::cout << "\n=== 测试 reverse_iterator ===" << std::endl;
    
    SimpleArray<int> arr;
    for (int i = 0; i < 5; ++i) {
        arr.push_back(i);
    }
    
    // 创建反向迭代器
    auto rbegin = sugar::make_reverse_iterator(arr.end());
    auto rend = sugar::make_reverse_iterator(arr.begin());
    
    std::cout << "反向遍历: ";
    for (auto it = rbegin; it != rend; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试反向迭代器的算术运算
    auto rit = rbegin + 2;
    std::cout << "rbegin + 2 指向的值: " << *rit << std::endl;
    
    auto diff = rbegin - rend;
    std::cout << "rbegin - rend = " << diff << std::endl;
}

void test_insert_iterators() {
    std::cout << "\n=== 测试插入迭代器 ===" << std::endl;
    
    SimpleArray<int> source;
    for (int i = 0; i < 3; ++i) {
        source.push_back(i);
    }
    
    SimpleArray<int> target1, target2, target3;
    
    // 测试 back_insert_iterator
    auto back_inserter = sugar::back_inserter(target1);
    for (auto it = source.begin(); it != source.end(); ++it) {
        *back_inserter = *it;
    }
    
    std::cout << "back_inserter 结果: ";
    for (auto it = target1.begin(); it != target1.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试 front_insert_iterator
    auto front_inserter = sugar::front_inserter(target2);
    for (auto it = source.begin(); it != source.end(); ++it) {
        *front_inserter = *it;
    }
    
    std::cout << "front_inserter 结果: ";
    for (auto it = target2.begin(); it != target2.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试 insert_iterator
    auto inserter = sugar::inserter(target3, target3.begin());
    for (auto it = source.begin(); it != source.end(); ++it) {
        *inserter = *it;
    }
    
    std::cout << "inserter 结果: ";
    for (auto it = target3.begin(); it != target3.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void test_convenience_functions() {
    std::cout << "\n=== 测试便捷函数 ===" << std::endl;
    
    SimpleArray<int> arr;
    for (int i = 0; i < 3; ++i) {
        arr.push_back(i);
    }
    
    // 测试 begin/end
    auto first = sugar::begin(arr);
    auto last = sugar::end(arr);
    std::cout << "使用 sugar::begin/end 遍历: ";
    for (auto it = first; it != last; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试 rbegin/rend
    auto rfirst = sugar::rbegin(arr);
    auto rlast = sugar::rend(arr);
    std::cout << "使用 sugar::rbegin/rend 反向遍历: ";
    for (auto it = rfirst; it != rlast; ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试数组
    int array[] = {10, 20, 30};
    std::cout << "数组遍历: ";
    for (auto it = sugar::begin(array); it != sugar::end(array); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void test_pointer_iterators() {
    std::cout << "\n=== 测试指针迭代器 ===" << std::endl;
    
    int array[] = {1, 2, 3, 4, 5};
    int* first = array;
    int* last = array + 5;
    
    // 测试指针的迭代器特征
    using ptr_traits = sugar::iterator_traits<int*>;
    std::cout << "int* 的 value_type: " 
              << (sugar::is_same<ptr_traits::value_type, int>::value ? "int" : "other") 
              << std::endl;
    
    // 测试指针的 distance
    auto dist = sugar::distance(first, last);
    std::cout << "指针 distance: " << dist << std::endl;
    
    // 测试指针的 advance
    int* it = first;
    sugar::advance(it, 2);
    std::cout << "指针 advance(2) 指向的值: " << *it << std::endl;
}

int main() {
    std::cout << "=== MyMiniSTL Iterator 测试 ===" << std::endl;
    
    test_iterator_traits();
    test_distance_and_advance();
    test_reverse_iterator();
    test_insert_iterators();
    test_convenience_functions();
    test_pointer_iterators();
    
    std::cout << "\n=== 所有测试完成 ===" << std::endl;
    return 0;
} 