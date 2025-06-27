#include "../utility.h"
#include "../exceptdef.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace sugar;

// 测试用的简单类
class TestClass {
public:
    int value;
    
    TestClass() : value(0) {}
    TestClass(int v) : value(v) {}
    TestClass(const TestClass& other) : value(other.value) {}
    TestClass(TestClass&& other) noexcept : value(other.value) {
        other.value = -1;
    }
    
    TestClass& operator=(const TestClass& other) {
        if (this != &other) {
            value = other.value;
        }
        return *this;
    }
    
    TestClass& operator=(TestClass&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = -1;
        }
        return *this;
    }
    
    bool operator==(const TestClass& other) const {
        return value == other.value;
    }
    
    bool operator<(const TestClass& other) const {
        return value < other.value;
    }
};

// 测试forward函数
void test_forward() {
    std::cout << "Testing forward function..." << std::endl;
    
    int x = 42;
    const int cx = 100;
    
    // 测试左值引用
    int& lref = sugar::forward<int&>(x);
    assert(lref == 42);
    
    // 测试右值引用
    int&& rref = sugar::forward<int>(42);
    assert(rref == 42);
    
    // 测试const左值引用
    const int& clref = sugar::forward<const int&>(cx);
    assert(clref == 100);
    
    std::cout << "  ✓ forward function tests passed" << std::endl;
}

// 测试move函数
void test_move() {
    std::cout << "Testing move function..." << std::endl;
    
    TestClass obj(42);
    TestClass moved = sugar::move(obj);
    
    assert(moved.value == 42);
    assert(obj.value == -1); // 移动后原对象被修改
    
    std::cout << "  ✓ move function tests passed" << std::endl;
}

// 测试swap函数
void test_swap() {
    std::cout << "Testing swap function..." << std::endl;
    
    int a = 10, b = 20;
    sugar::swap(a, b);
    assert(a == 20 && b == 10);
    
    TestClass obj1(100), obj2(200);
    sugar::swap(obj1, obj2);
    assert(obj1.value == 200 && obj2.value == 100);
    
    std::cout << "  ✓ swap function tests passed" << std::endl;
}

// 测试pair类型
void test_pair() {
    std::cout << "Testing pair type..." << std::endl;
    
    // 测试默认构造函数
    sugar::pair<int, std::string> p1;
    assert(p1.first == 0);
    assert(p1.second.empty());
    
    // 测试拷贝构造函数
    sugar::pair<int, std::string> p2(42, "hello");
    assert(p2.first == 42);
    assert(p2.second == "hello");
    
    // 测试移动构造函数
    TestClass tc1(100);
    TestClass tc2(200);
    sugar::pair<TestClass, TestClass> p3(sugar::move(tc1), sugar::move(tc2));
    assert(p3.first.value == 100);
    assert(p3.second.value == 200);
    assert(tc1.value == -1); // 被移动
    assert(tc2.value == -1); // 被移动
    
    // 测试混合构造函数
    int x = 300;
    TestClass tc3(400);
    sugar::pair<int, TestClass> p4(x, sugar::move(tc3));
    assert(p4.first == 300);
    assert(p4.second.value == 400);
    assert(tc3.value == -1); // 被移动
    
    // 测试模板拷贝构造函数
    sugar::pair<double, int> p5(3.14, 42);
    sugar::pair<int, double> p6(p5);
    assert(p6.first == 3);
    assert(p6.second == 42.0);
    
    // 测试赋值操作符
    sugar::pair<int, std::string> p7;
    p7 = p2;
    assert(p7.first == 42);
    assert(p7.second == "hello");
    
    // 测试移动赋值操作符
    TestClass tc4(500);
    TestClass tc5(600);
    sugar::pair<TestClass, TestClass> p8;
    p8 = sugar::pair<TestClass, TestClass>(sugar::move(tc4), sugar::move(tc5));
    assert(p8.first.value == 500);
    assert(p8.second.value == 600);
    assert(tc4.value == -1); // 被移动
    assert(tc5.value == -1); // 被移动
    
    // 测试swap方法
    sugar::pair<int, std::string> p9(1, "one");
    sugar::pair<int, std::string> p10(2, "two");
    p9.swap(p10);
    assert(p9.first == 2 && p9.second == "two");
    assert(p10.first == 1 && p10.second == "one");
    
    std::cout << "  ✓ pair type tests passed" << std::endl;
}

// 测试make_pair函数
void test_make_pair() {
    std::cout << "Testing make_pair function..." << std::endl;
    
    auto p1 = sugar::make_pair(42, std::string("hello"));
    assert(p1.first == 42);
    assert(p1.second == "hello");
    
    int x = 100;
    std::string s = "world";
    auto p2 = sugar::make_pair(x, s);
    assert(p2.first == 100);
    assert(p2.second == "world");
    
    // 测试右值
    auto p3 = sugar::make_pair(200, std::string("test"));
    assert(p3.first == 200);
    assert(p3.second == "test");
    
    std::cout << "  ✓ make_pair function tests passed" << std::endl;
}

// 测试pair比较操作符
void test_pair_comparison() {
    std::cout << "Testing pair comparison operators..." << std::endl;
    
    sugar::pair<int, std::string> p1(1, "a");
    sugar::pair<int, std::string> p2(1, "a");
    sugar::pair<int, std::string> p3(1, "b");
    sugar::pair<int, std::string> p4(2, "a");
    
    // 测试相等性
    assert(p1 == p2);
    assert(!(p1 == p3));
    assert(!(p1 == p4));
    
    // 测试不等性
    assert(!(p1 != p2));
    assert(p1 != p3);
    assert(p1 != p4);
    
    // 测试小于
    assert(p1 < p3);
    assert(p1 < p4);
    assert(!(p3 < p1));
    assert(!(p4 < p1));
    
    // 测试小于等于
    assert(p1 <= p2);
    assert(p1 <= p3);
    assert(p1 <= p4);
    assert(!(p3 <= p1));
    
    // 测试大于
    assert(p3 > p1);
    assert(p4 > p1);
    assert(!(p1 > p3));
    assert(!(p1 > p4));
    
    // 测试大于等于
    assert(p1 >= p2);
    assert(p3 >= p1);
    assert(p4 >= p1);
    assert(!(p1 >= p3));
    
    std::cout << "  ✓ pair comparison operators tests passed" << std::endl;
}

// 测试exchange函数
void test_exchange() {
    std::cout << "Testing exchange function..." << std::endl;
    
    int x = 10;
    int old_val = sugar::exchange(x, 20);
    assert(old_val == 10);
    assert(x == 20);
    
    std::string s = "hello";
    std::string old_str = sugar::exchange(s, std::string("world"));
    assert(old_str == "hello");
    assert(s == "world");
    
    TestClass tc(100);
    TestClass old_tc = sugar::exchange(tc, TestClass(200));
    assert(old_tc.value == 100);
    assert(tc.value == 200);
    
    std::cout << "  ✓ exchange function tests passed" << std::endl;
}

// 测试as_const函数
void test_as_const() {
    std::cout << "Testing as_const function..." << std::endl;
    
    int x = 42;
    const int& cx = sugar::as_const(x);
    assert(cx == 42);
    
    std::string s = "test";
    const std::string& cs = sugar::as_const(s);
    assert(cs == "test");
    
    std::cout << "  ✓ as_const function tests passed" << std::endl;
}

// 测试declval函数
void test_declval() {
    std::cout << "Testing declval function..." << std::endl;
    
    // declval主要用于decltype，这里测试类型推导
    using int_ref = decltype(sugar::declval<int>());
    using string_ref = decltype(sugar::declval<std::string>());
    
    static_assert(sugar::is_same<int_ref, int&&>::value, "declval<int> should return int&&");
    static_assert(sugar::is_same<string_ref, std::string&&>::value, "declval<string> should return string&&");
    
    std::cout << "  ✓ declval function tests passed" << std::endl;
}

// 测试异常安全性
void test_exception_safety() {
    std::cout << "Testing exception safety..." << std::endl;
    
    // 测试swap的异常安全性
    try {
        int a = 10, b = 20;
        sugar::swap(a, b);
        assert(a == 20 && b == 10);
    } catch (...) {
        assert(false && "swap should not throw");
    }
    
    // 测试pair的异常安全性
    try {
        sugar::pair<int, std::string> p1(42, "hello");
        sugar::pair<int, std::string> p2(100, "world");
        p1.swap(p2);
        assert(p1.first == 100 && p1.second == "world");
        assert(p2.first == 42 && p2.second == "hello");
    } catch (...) {
        assert(false && "pair::swap should not throw");
    }
    
    std::cout << "  ✓ exception safety tests passed" << std::endl;
}

int main() {
    std::cout << "Starting utility tests..." << std::endl;
    
    try {
        test_forward();
        test_move();
        test_swap();
        test_pair();
        test_make_pair();
        test_pair_comparison();
        test_exchange();
        test_as_const();
        test_declval();
        test_exception_safety();
        
        std::cout << "\n🎉 All utility tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
} 