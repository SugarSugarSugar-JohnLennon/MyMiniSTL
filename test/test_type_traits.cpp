#include "../type_traits.h"
#include <iostream>
#include <string>
#include <vector>

// 测试用的类
class Base {};
class Derived : public Base {};
class NonDerived {};

// 测试函数
void test_function() {}

int main() {
    std::cout << "=== MyMiniSTL Type Traits 测试 ===" << std::endl;
    
    // 测试基础类型判断
    std::cout << "\n1. 基础类型判断:" << std::endl;
    std::cout << "is_void<void>::value: " << sugar::is_void<void>::value << std::endl;
    std::cout << "is_void<int>::value: " << sugar::is_void<int>::value << std::endl;
    
    std::cout << "is_integer<int>::value: " << sugar::is_integer<int>::value << std::endl;
    std::cout << "is_integer<const int>::value: " << sugar::is_integer<const int>::value << std::endl;
    std::cout << "is_integer<int&>::value: " << sugar::is_integer<int&>::value << std::endl;
    std::cout << "is_integer<double>::value: " << sugar::is_integer<double>::value << std::endl;
    
    std::cout << "is_float<double>::value: " << sugar::is_float<double>::value << std::endl;
    std::cout << "is_float<int>::value: " << sugar::is_float<int>::value << std::endl;
    
    std::cout << "is_char<char>::value: " << sugar::is_char<char>::value << std::endl;
    std::cout << "is_char<int>::value: " << sugar::is_char<int>::value << std::endl;
    
    // 测试指针和数组判断
    std::cout << "\n2. 指针和数组判断:" << std::endl;
    std::cout << "is_pointer<int*>::value: " << sugar::is_pointer<int*>::value << std::endl;
    std::cout << "is_pointer<int>::value: " << sugar::is_pointer<int>::value << std::endl;
    std::cout << "is_pointer<const int*>::value: " << sugar::is_pointer<const int*>::value << std::endl;
    
    std::cout << "is_array<int[5]>::value: " << sugar::is_array<int[5]>::value << std::endl;
    std::cout << "is_array<int[]>::value: " << sugar::is_array<int[]>::value << std::endl;
    std::cout << "is_array<int>::value: " << sugar::is_array<int>::value << std::endl;
    
    // 测试函数判断
    std::cout << "\n3. 函数判断:" << std::endl;
    std::cout << "is_function<void()>::value: " << sugar::is_function<void()>::value << std::endl;
    std::cout << "is_function<int(int, double)>::value: " << sugar::is_function<int(int, double)>::value << std::endl;
    std::cout << "is_function<int>::value: " << sugar::is_function<int>::value << std::endl;
    
    // 测试类型转换
    std::cout << "\n4. 类型转换:" << std::endl;
    std::cout << "is_same<int, int>::value: " << sugar::is_same<int, int>::value << std::endl;
    std::cout << "is_same<int, double>::value: " << sugar::is_same<int, double>::value << std::endl;
    std::cout << "is_same<int, const int>::value: " << sugar::is_same<int, const int>::value << std::endl;
    
    // 测试继承关系
    std::cout << "\n5. 继承关系判断:" << std::endl;
    std::cout << "is_base_of<Base, Derived>::value: " << sugar::is_base_of<Base, Derived>::value << std::endl;
    std::cout << "is_base_of<Base, NonDerived>::value: " << sugar::is_base_of<Base, NonDerived>::value << std::endl;
    std::cout << "is_base_of<Derived, Base>::value: " << sugar::is_base_of<Derived, Base>::value << std::endl;
    
    // 测试复合类型判断
    std::cout << "\n6. 复合类型判断:" << std::endl;
    std::cout << "is_arithmetic<int>::value: " << sugar::is_arithmetic<int>::value << std::endl;
    std::cout << "is_arithmetic<double>::value: " << sugar::is_arithmetic<double>::value << std::endl;
    std::cout << "is_arithmetic<std::string>::value: " << sugar::is_arithmetic<std::string>::value << std::endl;
    
    std::cout << "is_scalar<int>::value: " << sugar::is_scalar<int>::value << std::endl;
    std::cout << "is_scalar<int*>::value: " << sugar::is_scalar<int*>::value << std::endl;
    std::cout << "is_scalar<std::string>::value: " << sugar::is_scalar<std::string>::value << std::endl;
    
    std::cout << "is_object<int>::value: " << sugar::is_object<int>::value << std::endl;
    std::cout << "is_object<void()>::value: " << sugar::is_object<void()>::value << std::endl;
    std::cout << "is_object<void>::value: " << sugar::is_object<void>::value << std::endl;
    
    // 测试类型别名
    std::cout << "\n7. 类型别名测试:" << std::endl;
    std::cout << "is_same<sugar::remove_const_t<const int>, int>::value: " 
              << sugar::is_same<sugar::remove_const_t<const int>, int>::value << std::endl;
    std::cout << "is_same<sugar::remove_reference_t<int&>, int>::value: " 
              << sugar::is_same<sugar::remove_reference_t<int&>, int>::value << std::endl;
    std::cout << "is_same<sugar::decay_t<int[5]>, int*>::value: " 
              << sugar::is_same<sugar::decay_t<int[5]>, int*>::value << std::endl;
    
    // 测试conditional
    std::cout << "\n8. Conditional测试:" << std::endl;
    using type1 = sugar::conditional_t<true, int, double>;
    using type2 = sugar::conditional_t<false, int, double>;
    std::cout << "conditional<true, int, double>::type is int: " 
              << sugar::is_same<type1, int>::value << std::endl;
    std::cout << "conditional<false, int, double>::type is double: " 
              << sugar::is_same<type2, double>::value << std::endl;
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
} 