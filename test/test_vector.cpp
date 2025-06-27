/*
 * @file test_vector.cpp
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL vector容器测试
 */

#include "vector.h"
#include "allocator.h"
#include <iostream>
#include <cassert>
#include <string>

// 测试用的类
class TestObject {
public:
    int value;
    static int construct_count;
    static int destruct_count;
    static int copy_count;
    static int move_count;

    TestObject() : value(0) {
        ++construct_count;
    }

    TestObject(int v) : value(v) {
        ++construct_count;
    }

    TestObject(const TestObject& other) : value(other.value) {
        ++copy_count;
    }

    TestObject(TestObject&& other) noexcept : value(other.value) {
        other.value = -1;
        ++move_count;
    }

    TestObject& operator=(const TestObject& other) {
        if (this != &other) {
            value = other.value;
            ++copy_count;
        }
        return *this;
    }

    TestObject& operator=(TestObject&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = -1;
            ++move_count;
        }
        return *this;
    }

    ~TestObject() {
        ++destruct_count;
    }

    bool operator==(const TestObject& other) const {
        return value == other.value;
    }

    bool operator<(const TestObject& other) const {
        return value < other.value;
    }

    static void reset_counters() {
        construct_count = 0;
        destruct_count = 0;
        copy_count = 0;
        move_count = 0;
    }
};

int TestObject::construct_count = 0;
int TestObject::destruct_count = 0;
int TestObject::copy_count = 0;
int TestObject::move_count = 0;

// 测试函数声明
void test_constructors();
void test_assignment();
void test_element_access();
void test_iterators();
void test_capacity();
void test_modifiers();
void test_allocator();
void test_exceptions();
void test_performance();

int main() {
    std::cout << "=== MyMiniSTL Vector 测试 ===" << std::endl;

    try {
        test_constructors();
        test_assignment();
        test_element_access();
        test_iterators();
        test_capacity();
        test_modifiers();
        test_allocator();
        test_exceptions();
        test_performance();

        std::cout << "\n🎉 All vector tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}

// 测试构造函数
void test_constructors() {
    std::cout << "\n=== 测试构造函数 ===" << std::endl;

    // 默认构造函数
    sugar::vector<int> v1;
    assert(v1.empty());
    assert(v1.size() == 0);
    assert(v1.capacity() == 0);
    std::cout << "✓ 默认构造函数" << std::endl;

    // 指定大小的构造函数
    sugar::vector<int> v2(5, 42);
    assert(v2.size() == 5);
    assert(v2.capacity() >= 5);
    for (size_t i = 0; i < v2.size(); ++i) {
        assert(v2[i] == 42);
    }
    std::cout << "✓ 指定大小的构造函数" << std::endl;

    // 拷贝构造函数
    sugar::vector<int> v3(v2);
    assert(v3.size() == v2.size());
    assert(v3 == v2);
    std::cout << "✓ 拷贝构造函数" << std::endl;

    // 移动构造函数
    sugar::vector<int> v4(sugar::move(v3));
    assert(v4.size() == 5);
    assert(v3.empty()); // 原对象被移动
    std::cout << "✓ 移动构造函数" << std::endl;

    // 迭代器范围构造函数
    int arr[] = {1, 2, 3, 4, 5};
    sugar::vector<int> v5(arr, arr + 5);
    assert(v5.size() == 5);
    for (size_t i = 0; i < v5.size(); ++i) {
        assert(v5[i] == arr[i]);
    }
    std::cout << "✓ 迭代器范围构造函数" << std::endl;

    // 初始化列表构造函数
    sugar::vector<int> v6 = {10, 20, 30, 40, 50};
    assert(v6.size() == 5);
    assert(v6[0] == 10);
    assert(v6[4] == 50);
    std::cout << "✓ 初始化列表构造函数" << std::endl;
}

// 测试赋值操作
void test_assignment() {
    std::cout << "\n=== 测试赋值操作 ===" << std::endl;

    sugar::vector<int> v1 = {1, 2, 3};
    sugar::vector<int> v2;

    // 拷贝赋值
    v2 = v1;
    assert(v2 == v1);
    std::cout << "✓ 拷贝赋值操作符" << std::endl;

    // 移动赋值
    sugar::vector<int> v3;
    v3 = sugar::move(v2);
    assert(v3 == v1);
    assert(v2.empty()); // 原对象被移动
    std::cout << "✓ 移动赋值操作符" << std::endl;

    // 初始化列表赋值
    sugar::vector<int> v4;
    v4 = {100, 200, 300};
    assert(v4.size() == 3);
    assert(v4[0] == 100);
    assert(v4[2] == 300);
    std::cout << "✓ 初始化列表赋值" << std::endl;

    // assign函数
    v4.assign(3, 999);
    assert(v4.size() == 3);
    for (size_t i = 0; i < v4.size(); ++i) {
        assert(v4[i] == 999);
    }
    std::cout << "✓ assign(count, value)" << std::endl;

    int arr[] = {1, 2, 3, 4, 5};
    v4.assign(arr, arr + 5);
    assert(v4.size() == 5);
    for (size_t i = 0; i < v4.size(); ++i) {
        assert(v4[i] == arr[i]);
    }
    std::cout << "✓ assign(iterators)" << std::endl;
}

// 测试元素访问
void test_element_access() {
    std::cout << "\n=== 测试元素访问 ===" << std::endl;

    sugar::vector<int> v = {10, 20, 30, 40, 50};

    // operator[]
    assert(v[0] == 10);
    assert(v[4] == 50);
    v[2] = 999;
    assert(v[2] == 999);
    std::cout << "✓ operator[]" << std::endl;

    // at()
    assert(v.at(0) == 10);
    assert(v.at(4) == 50);
    try {
        v.at(10); // 应该抛出异常
        assert(false);
    } catch (const std::out_of_range&) {
        // 期望的异常
    }
    std::cout << "✓ at()" << std::endl;

    // front() 和 back()
    assert(v.front() == 10);
    assert(v.back() == 50);
    std::cout << "✓ front() 和 back()" << std::endl;

    // data()
    int* data = v.data();
    assert(data[0] == 10);
    assert(data[4] == 50);
    std::cout << "✓ data()" << std::endl;
}

// 测试迭代器
void test_iterators() {
    std::cout << "\n=== 测试迭代器 ===" << std::endl;

    sugar::vector<int> v = {1, 2, 3, 4, 5};

    // 正向迭代器
    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it) {
        sum += *it;
    }
    assert(sum == 15);
    std::cout << "✓ 正向迭代器" << std::endl;

    // const迭代器
    const sugar::vector<int>& cv = v;
    sum = 0;
    for (auto it = cv.begin(); it != cv.end(); ++it) {
        sum += *it;
    }
    assert(sum == 15);
    std::cout << "✓ const迭代器" << std::endl;

    // 反向迭代器
    sum = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        sum += *it;
    }
    assert(sum == 15);
    std::cout << "✓ 反向迭代器" << std::endl;

    // 基于范围的for循环
    sum = 0;
    for (const auto& val : v) {
        sum += val;
    }
    assert(sum == 15);
    std::cout << "✓ 基于范围的for循环" << std::endl;
}

// 测试容量操作
void test_capacity() {
    std::cout << "\n=== 测试容量操作 ===" << std::endl;

    sugar::vector<int> v;

    // empty() 和 size()
    assert(v.empty());
    assert(v.size() == 0);
    std::cout << "✓ empty() 和 size()" << std::endl;

    // capacity() 和 reserve()
    assert(v.capacity() == 0);
    v.reserve(100);
    assert(v.capacity() >= 100);
    assert(v.empty()); // reserve不应该改变大小
    std::cout << "✓ capacity() 和 reserve()" << std::endl;

    // max_size()
    assert(v.max_size() > 0);
    std::cout << "✓ max_size()" << std::endl;

    // shrink_to_fit()
    v = {1, 2, 3, 4, 5};
    size_t old_capacity = v.capacity();
    v.shrink_to_fit();
    assert(v.capacity() <= old_capacity);
    assert(v.size() == 5);
    std::cout << "✓ shrink_to_fit()" << std::endl;
}

// 测试修改器
void test_modifiers() {
    std::cout << "\n=== 测试修改器 ===" << std::endl;

    sugar::vector<int> v;

    // push_back()
    v.push_back(10);
    assert(v.size() == 1);
    assert(v[0] == 10);
    v.push_back(20);
    assert(v.size() == 2);
    assert(v[1] == 20);
    std::cout << "✓ push_back()" << std::endl;

    // pop_back()
    v.pop_back();
    assert(v.size() == 1);
    assert(v[0] == 10);
    std::cout << "✓ pop_back()" << std::endl;

    // clear()
    v.clear();
    assert(v.empty());
    std::cout << "✓ clear()" << std::endl;

    // insert()
    v = {1, 2, 3};
    auto it = v.insert(v.begin() + 1, 999);
    assert(v.size() == 4);
    assert(v[1] == 999);
    assert(*it == 999);
    std::cout << "✓ insert()" << std::endl;

    // insert(count, value)
    it = v.insert(v.begin(), 3, 888);
    assert(v.size() == 7);
    assert(v[0] == 888);
    assert(v[1] == 888);
    assert(v[2] == 888);
    std::cout << "✓ insert(count, value)" << std::endl;

    // erase()
    it = v.erase(v.begin() + 1);
    assert(v.size() == 6);
    assert(*it == 888);
    std::cout << "✓ erase()" << std::endl;

    // erase(range)
    it = v.erase(v.begin(), v.begin() + 2);
    assert(v.size() == 4);
    assert(*it == 1);
    std::cout << "✓ erase(range)" << std::endl;

    // resize()
    v.resize(6, 999);
    assert(v.size() == 6);
    assert(v[4] == 999);
    assert(v[5] == 999);
    std::cout << "resize前: v.size()=" << v.size() << ", v.capacity()=" << v.capacity() << std::endl;
    v.resize(2);
    std::cout << "resize后: v.size()=" << v.size() << ", v.capacity()=" << v.capacity() << std::endl;
    assert(v.size() == 2);

    // swap()
    sugar::vector<int> v2 = {100, 200, 300};
    std::cout << "swap前: v.size()=" << v.size() << ", v2.size()=" << v2.size() << std::endl;
    v.swap(v2);
    std::cout << "swap后: v.size()=" << v.size() << ", v2.size()=" << v2.size() << std::endl;
    assert(v.size() == 3);
    assert(v2.size() == 2);
    std::cout << "✓ swap()" << std::endl;
}

// 测试分配器
void test_allocator() {
    std::cout << "\n=== 测试分配器 ===" << std::endl;

    // 使用自定义分配器
    sugar::vector<int, sugar::pool_allocator<int>> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    assert(v.size() == 3);
    assert(v[0] == 1);
    assert(v[2] == 3);
    std::cout << "✓ 自定义分配器" << std::endl;

    // get_allocator()
    const auto& alloc = v.get_allocator();
    assert(alloc.max_size() > 0);
    std::cout << "✓ get_allocator()" << std::endl;
}

// 测试异常安全
void test_exceptions() {
    std::cout << "\n=== 测试异常安全 ===" << std::endl;

    sugar::vector<int> v = {1, 2, 3, 4, 5};

    // at() 边界检查
    try {
        v.at(10);
        assert(false);
    } catch (const std::out_of_range& e) {
        std::cout << "✓ at() 边界检查异常: " << e.what() << std::endl;
    }

    // 空vector的front()和back()
    sugar::vector<int> empty_v;
    try {
        empty_v.front();
        assert(false);
    } catch (...) {
        // 期望的异常
    }
    try {
        empty_v.back();
        assert(false);
    } catch (...) {
        // 期望的异常
    }
    std::cout << "✓ 空vector访问异常" << std::endl;
}

// 测试性能
void test_performance() {
    std::cout << "\n=== 测试性能 ===" << std::endl;

    // 测试大量元素的插入
    sugar::vector<int> v;
    for (int i = 0; i < 10000; ++i) {
        v.push_back(i);
    }
    assert(v.size() == 10000);
    assert(v[0] == 0);
    assert(v[9999] == 9999);
    std::cout << "✓ 大量元素插入" << std::endl;

    // 测试内存效率
    size_t initial_capacity = v.capacity();
    v.shrink_to_fit();
    assert(v.capacity() == v.size());
    std::cout << "✓ 内存效率优化" << std::endl;

    // 测试移动语义
    TestObject::reset_counters();
    sugar::vector<TestObject> obj_v;
    obj_v.push_back(TestObject(42));
    obj_v.push_back(TestObject(84));
    
    // 移动构造
    sugar::vector<TestObject> moved_v = sugar::move(obj_v);
    assert(moved_v.size() == 2);
    assert(obj_v.empty());
    std::cout << "✓ 移动语义" << std::endl;
}

// 测试比较操作符
void test_comparison() {
    std::cout << "\n=== 测试比较操作符 ===" << std::endl;

    sugar::vector<int> v1 = {1, 2, 3};
    sugar::vector<int> v2 = {1, 2, 3};
    sugar::vector<int> v3 = {1, 2, 4};
    sugar::vector<int> v4 = {1, 2};

    // 相等比较
    assert(v1 == v2);
    assert(!(v1 == v3));
    assert(!(v1 == v4));
    std::cout << "✓ 相等比较" << std::endl;

    // 不等比较
    assert(!(v1 != v2));
    assert(v1 != v3);
    assert(v1 != v4);
    std::cout << "✓ 不等比较" << std::endl;

    // 大小比较
    assert(v4 < v1);
    assert(v1 < v3);
    assert(v3 > v1);
    assert(v1 > v4);
    std::cout << "✓ 大小比较" << std::endl;
} 