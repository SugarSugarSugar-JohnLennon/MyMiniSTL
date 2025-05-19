/**
 * type_traits.h
 * 利用模板、模板特例、类型转换等特性
 * 来实现 编译期 的一系列判断和类型转换
 */

#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

namespace sugar{
    /**
     * 给出true 和 false 这两个常量 ，后面操作的基础
     * 用一个模板类型来表示 yes or no
     * 后面的各种判断就是这个模版（为什么是模版？ 我猜测是因为编译期就能确定
     */
    struct true_type  {  static constexpr bool value = true;   };
    struct false_type {  static constexpr bool value = false;  };

    /**
     * 下文中，如果是 const int & 会影响判断是否为int
     * 于是我们需要 remove_const 和 remove_reference
     */
    template <typename T>
    struct remove_const {  using type = T;  };
    
    template <typename T>
    struct remove_const<const T> {  using type = T;  };

    template <typename T>
    struct remove_reference {  using type = T;  };

    template <typename T>
    struct remove_reference<T &> {  using type = T;  };

    template <typename T>
    struct remove_reference<T &&> {  using type = T;  };


    /**
     * 基础类型判断， 判断传入函数的返回值
     * void func ();
     * 如 ： is_void<decltype(func())>::value = ture_type
     * 就能支持模版编译期的类型判断
     */
    template <typename>
    struct is_void : false_type {};
    struct is_void<void> : true_type {};

    /* 以下判断都要考虑const 和 &  */
    /* 浮点数判断helper */
    template <typename>
    struct is_float_helper : false_type {};
    struct is_float_helper<float> : true_type {};
    struct is_float_helper<double> : true_type {};
    struct is_float_helper<long double> : true_type {};

    /* 字符判断helper */
    template <typename>
    struct is_char_helper : false_type {};
    struct is_char_helper<char> : true_type {};
    struct is_char_helper<unsigned char> : true_type {};
    struct is_char_helper<signed char> : true_type {};

    /* 整型判断helper */
    template <typename>
    struct is_integer_helper : false_type {};
    struct is_integer_helper<short> : true_type {};
    struct is_integer_helper<unsigned short> : true_type {};
    struct is_integer_helper<int> : true_type {};
    struct is_integer_helper<unsigned int> : true_type {};
    struct is_integer_helper<long> : true_type {};
    struct is_integer_helper<unsigned long> : true_type {};
    struct is_integer_helper<long long> : true_type {};
    struct is_integer_helper<unsigned long long> : true_type {};

    /**
     * 判断类型的具体方法 
     */
    template <typename T>
    struct is_float 
    : is_float_helper<typename remove_const<remove_reference<T>::type>::type> {};

    template <typename T>
    struct is_char 
    : is_char_helper<typename remove_const<remove_reference<T>::type>::type> {};

    template <typename T>
    struct is_integer 
    : is_integer_helper<typename remove_const<remove_reference<T>::type>::type> {};

   /**
    * 上面的类型封装最主要的原因是为了在编译器进行逻辑判断
    * 下面实现选择逻辑，也就是当条件成立时，选择某一个类型，不成立时选择另一个类型
    * 当第一个参数为true时，type就与T1相同，否则就与T2相同。
    */
    template <bool judge, typename T1, typename T2>
    struct conditional {  using type = T1  };

    template <typename T1, typename T2>
    struct conditional<false, T1, T2> {  using type = T2  };

    /**
     * 判断类型是否相同
     */
    template <typename, typename>
    struct is_same : false_type {};

    template <typename T>
    struct is_same<T, T> : true_type {};

    /**
     * is_base_of用于判断两个类型是否是继承关系，
     * 在C++中已经存在了对应的关键字用于判断：__is_base_of(Base, Derive)
     */
    template <typename base, typename derive>
    struct is_base_of : conditional<__is_base_of(base, derive), true_type, false_type> {};

    /**
     * 判断一个类是否为一个类的父类
     * 其实就看指针能否转换（多态
     * 如果D是B的子类，那么就会调用第一个函数，从而推断出返回值是true_type，
     * 否则调用第二个函数，推断出返回值是false_type。
     * 
     * 就是B和D必须都是类才行，而且需要去掉const等因素，所以这是   ====半成品====
     */
    template <typename base, typename derive>
    true_type test(base*);

    template <typename base, typename derive>
    false_type test(void *);

    template <typename base, typename derive>
    struct is_base_of : decltype(test<base, derive>(static_cast<derive*>(nullptr))) {};


    // enable_if

    // is_pointer

    // decay

    // is_array

    // is_function

}

#endif // TYPE_TRAITS_H_