#include "../allocator.h"
#include <iostream>

// 测试用的类
class TestObject {
private:
    int value_;
    static int constructed_count_;
    static int destroyed_count_;

public:
    TestObject() : value_(0) { ++constructed_count_; }
    TestObject(int v) : value_(v) { ++constructed_count_; }
    TestObject(const TestObject& other) : value_(other.value_) { ++constructed_count_; }
    ~TestObject() { ++destroyed_count_; }

    int get_value() const { return value_; }
    void set_value(int v) { value_ = v; }

    static int get_constructed_count() { return constructed_count_; }
    static int get_destroyed_count() { return destroyed_count_; }
    static void reset_counters() { constructed_count_ = destroyed_count_ = 0; }
};

int TestObject::constructed_count_ = 0;
int TestObject::destroyed_count_ = 0;

void test_basic_allocation() {
    std::cout << "\n=== 测试基础内存分配 ===" << std::endl;
    
    // 测试标准分配器
    sugar::allocator<int> alloc;
    
    // 分配单个元素
    int* ptr1 = alloc.allocate(1);
    std::cout << "分配单个int: " << (ptr1 != nullptr ? "成功" : "失败") << std::endl;
    
    // 分配多个元素
    int* ptr2 = alloc.allocate(10);
    std::cout << "分配10个int: " << (ptr2 != nullptr ? "成功" : "失败") << std::endl;
    
    // 释放内存
    alloc.deallocate(ptr1, 1);
    alloc.deallocate(ptr2, 10);
    std::cout << "内存释放完成" << std::endl;
    
    // 测试零分配
    int* ptr3 = alloc.allocate(0);
    std::cout << "零分配: " << (ptr3 == nullptr ? "正确" : "错误") << std::endl;
}

void test_object_construction() {
    std::cout << "\n=== 测试对象构造和析构 ===" << std::endl;
    
    TestObject::reset_counters();
    
    sugar::allocator<TestObject> alloc;
    
    // 分配内存
    TestObject* ptr = alloc.allocate(3);
    std::cout << "分配3个TestObject内存: " << (ptr != nullptr ? "成功" : "失败") << std::endl;
    
    // 构造对象
    alloc.construct(ptr, 10);
    alloc.construct(ptr + 1, 20);
    alloc.construct(ptr + 2, 30);
    
    std::cout << "构造对象数量: " << TestObject::get_constructed_count() << std::endl;
    std::cout << "对象值: " << ptr[0].get_value() << ", " << ptr[1].get_value() << ", " << ptr[2].get_value() << std::endl;
    
    // 析构对象
    alloc.destroy(ptr);
    alloc.destroy(ptr + 1);
    alloc.destroy(ptr + 2);
    
    std::cout << "析构对象数量: " << TestObject::get_destroyed_count() << std::endl;
    
    // 释放内存
    alloc.deallocate(ptr, 3);
}

void test_uninitialized_operations() {
    std::cout << "\n=== 测试未初始化内存操作 ===" << std::endl;
    
    TestObject::reset_counters();
    
    sugar::allocator<TestObject> alloc;
    
    // 分配内存
    TestObject* ptr = alloc.allocate(5);
    
    // 测试 uninitialized_fill_n
    sugar::uninitialized_fill_n(ptr, 3, TestObject(100));
    std::cout << "uninitialized_fill_n 构造对象数量: " << TestObject::get_constructed_count() << std::endl;
    
    // 测试 uninitialized_fill
    sugar::uninitialized_fill(ptr + 3, ptr + 5, TestObject(200));
    std::cout << "uninitialized_fill 构造对象数量: " << TestObject::get_constructed_count() << std::endl;
    
    // 验证对象值
    for (int i = 0; i < 5; ++i) {
        std::cout << "ptr[" << i << "] = " << ptr[i].get_value() << " ";
    }
    std::cout << std::endl;
    
    // 测试 uninitialized_copy
    TestObject* dest = alloc.allocate(5);
    sugar::uninitialized_copy(ptr, ptr + 5, dest);
    std::cout << "uninitialized_copy 构造对象数量: " << TestObject::get_constructed_count() << std::endl;
    
    // 清理
    sugar::destroy(ptr, ptr + 5);
    sugar::destroy(dest, dest + 5);
    alloc.deallocate(ptr, 5);
    alloc.deallocate(dest, 5);
    
    std::cout << "最终析构对象数量: " << TestObject::get_destroyed_count() << std::endl;
}

void test_pool_allocator() {
    std::cout << "\n=== 测试内存池分配器 ===" << std::endl;
    
    sugar::pool_allocator<int> pool_alloc;
    
    // 分配小对象（使用内存池）
    int* small1 = pool_alloc.allocate(1);
    int* small2 = pool_alloc.allocate(1);
    int* small3 = pool_alloc.allocate(1);
    
    std::cout << "分配3个小对象: " << (small1 && small2 && small3 ? "成功" : "失败") << std::endl;
    
    // 释放小对象
    pool_alloc.deallocate(small1, 1);
    pool_alloc.deallocate(small2, 1);
    pool_alloc.deallocate(small3, 1);
    std::cout << "释放小对象完成" << std::endl;
    
    // 分配大对象（直接分配）
    int* large = pool_alloc.allocate(1000);
    std::cout << "分配大对象: " << (large != nullptr ? "成功" : "失败") << std::endl;
    pool_alloc.deallocate(large, 1000);
    
    // 测试对象构造
    TestObject::reset_counters();
    sugar::pool_allocator<TestObject> obj_pool_alloc;
    
    TestObject* obj_ptr = obj_pool_alloc.allocate(2);
    obj_pool_alloc.construct(obj_ptr, 42);
    obj_pool_alloc.construct(obj_ptr + 1, 84);
    
    std::cout << "内存池构造对象: " << TestObject::get_constructed_count() << std::endl;
    std::cout << "对象值: " << obj_ptr[0].get_value() << ", " << obj_ptr[1].get_value() << std::endl;
    
    obj_pool_alloc.destroy(obj_ptr);
    obj_pool_alloc.destroy(obj_ptr + 1);
    obj_pool_alloc.deallocate(obj_ptr, 2);
    
    std::cout << "内存池析构对象: " << TestObject::get_destroyed_count() << std::endl;
}

void test_allocator_traits() {
    std::cout << "\n=== 测试 allocator_traits ===" << std::endl;
    
    sugar::allocator<int> alloc;
    using traits = sugar::allocator_traits<sugar::allocator<int>>;
    
    // 测试类型别名
    std::cout << "value_type 是 int: " 
              << (sugar::is_same<traits::value_type, int>::value ? "正确" : "错误") << std::endl;
    
    std::cout << "pointer 是 int*: " 
              << (sugar::is_same<traits::pointer, int*>::value ? "正确" : "错误") << std::endl;
    
    // 测试分配
    int* ptr = traits::allocate(alloc, 5);
    std::cout << "traits::allocate: " << (ptr != nullptr ? "成功" : "失败") << std::endl;
    
    // 测试构造
    traits::construct(alloc, ptr, 123);
    std::cout << "traits::construct 值: " << *ptr << std::endl;
    
    // 测试析构
    traits::destroy(alloc, ptr);
    
    // 测试释放
    traits::deallocate(alloc, ptr, 5);
    std::cout << "traits::deallocate 完成" << std::endl;
    
    // 测试最大大小
    size_t max_size = traits::max_size(alloc);
    std::cout << "最大分配大小: " << max_size << std::endl;
}

void test_convenience_functions() {
    std::cout << "\n=== 测试便捷函数 ===" << std::endl;
    
    TestObject::reset_counters();
    
    sugar::allocator<TestObject> alloc;
    
    // 测试 create_object
    TestObject* obj1 = sugar::create_object<TestObject>(alloc, 100);
    std::cout << "create_object 构造对象: " << TestObject::get_constructed_count() << std::endl;
    std::cout << "create_object 值: " << obj1->get_value() << std::endl;
    
    // 测试 destroy_object
    sugar::destroy_object(alloc, obj1);
    std::cout << "destroy_object 析构对象: " << TestObject::get_destroyed_count() << std::endl;
}

void test_exception_safety() {
    std::cout << "\n=== 测试异常安全 ===" << std::endl;
    
    sugar::allocator<int> alloc;
    
    try {
        // 测试分配失败（请求过大的内存）
        size_t huge_size = size_t(-1) / sizeof(int) + 1;
        int* ptr = alloc.allocate(huge_size);
        std::cout << "大内存分配: " << (ptr != nullptr ? "成功" : "失败") << std::endl;
    } catch (const std::length_error& e) {
        std::cout << "捕获 length_error: " << e.what() << std::endl;
    }
    
    try {
        // 测试内存分配失败
        void* ptr = sugar::allocate(size_t(-1));
        std::cout << "超大内存分配: " << (ptr != nullptr ? "成功" : "失败") << std::endl;
    } catch (const std::bad_alloc& e) {
        std::cout << "捕获 bad_alloc: " << e.what() << std::endl;
    }
}

void test_allocator_comparison() {
    std::cout << "\n=== 测试分配器比较 ===" << std::endl;
    
    sugar::allocator<int> alloc1;
    sugar::allocator<int> alloc2;
    sugar::allocator<double> alloc3;
    
    // 同类型分配器比较
    bool equal = (alloc1 == alloc2);
    bool not_equal = (alloc1 != alloc2);
    
    std::cout << "同类型分配器相等: " << (equal ? "是" : "否") << std::endl;
    std::cout << "同类型分配器不等: " << (not_equal ? "是" : "否") << std::endl;
    
    // 不同类型分配器比较
    bool different_equal = (alloc1 == alloc3);
    std::cout << "不同类型分配器相等: " << (different_equal ? "是" : "否") << std::endl;
}

int main() {
    std::cout << "=== MyMiniSTL Allocator 测试 ===" << std::endl;
    
    test_basic_allocation();
    test_object_construction();
    test_uninitialized_operations();
    test_pool_allocator();
    test_allocator_traits();
    test_convenience_functions();
    test_exception_safety();
    test_allocator_comparison();
    
    std::cout << "\n=== 所有测试完成 ===" << std::endl;
    return 0;
} 