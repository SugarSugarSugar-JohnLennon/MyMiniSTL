/*
 * @file type_traits.h
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL 类型萃取与类型判断工具，完全独立实现，不依赖std
 */

#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

#include <cstddef>

namespace sugar{
    /**
     * @brief true/false 常量类型，类型判断基础
     */
    struct true_type  {  static constexpr bool value = true;   };
    struct false_type {  static constexpr bool value = false;  };

    /**
     * @brief 移除const修饰符
     */
    template <typename T>
    struct remove_const {  using type = T;  };
    
    template <typename T>
    struct remove_const<const T> {  using type = T;  };

    /**
     * @brief 移除引用修饰符
     */
    template <typename T>
    struct remove_reference {  using type = T;  };

    template <typename T>
    struct remove_reference<T &> {  using type = T;  };

    template <typename T>
    struct remove_reference<T &&> {  using type = T;  };

    /**
     * @brief 移除volatile修饰符
     */
    template <typename T>
    struct remove_volatile { using type = T; };

    template <typename T>
    struct remove_volatile<volatile T> { using type = T; };

    /**
     * @brief 移除cv修饰符（const和volatile）
     */
    template <typename T>
    struct remove_cv { 
        using type = typename remove_volatile<typename remove_const<T>::type>::type; 
    };

    /**
     * @brief 判断是否为void类型
     */
    template <typename>
    struct is_void : false_type {};
    template <>
    struct is_void<void> : true_type {};

    /* 以下判断都要考虑const 和 &  */
    /* 浮点数判断helper */
    template <typename>
    struct is_float_helper : false_type {};
    template <>
    struct is_float_helper<float> : true_type {};
    template <>
    struct is_float_helper<double> : true_type {};
    template <>
    struct is_float_helper<long double> : true_type {};

    /* 字符判断helper */
    template <typename>
    struct is_char_helper : false_type {};
    template <>
    struct is_char_helper<char> : true_type {};
    template <>
    struct is_char_helper<unsigned char> : true_type {};
    template <>
    struct is_char_helper<signed char> : true_type {};

    /* 整型判断helper */
    template <typename>
    struct is_integer_helper : false_type {};
    template <>
    struct is_integer_helper<short> : true_type {};
    template <>
    struct is_integer_helper<unsigned short> : true_type {};
    template <>
    struct is_integer_helper<int> : true_type {};
    template <>
    struct is_integer_helper<unsigned int> : true_type {};
    template <>
    struct is_integer_helper<long> : true_type {};
    template <>
    struct is_integer_helper<unsigned long> : true_type {};
    template <>
    struct is_integer_helper<long long> : true_type {};
    template <>
    struct is_integer_helper<unsigned long long> : true_type {};

    /**
     * 判断类型的具体方法 
     */
    template <typename T>
    struct is_float 
    : is_float_helper<typename remove_cv<typename remove_reference<T>::type>::type> {};

    template <typename T>
    struct is_char 
    : is_char_helper<typename remove_cv<typename remove_reference<T>::type>::type> {};

    template <typename T>
    struct is_integer 
    : is_integer_helper<typename remove_cv<typename remove_reference<T>::type>::type> {};

    /**
     * 判断是否为指针类型
     */
    template <typename>
    struct is_pointer_helper : false_type {};

    template <typename T>
    struct is_pointer_helper<T*> : true_type {};

    template <typename T>
    struct is_pointer 
    : is_pointer_helper<typename remove_cv<typename remove_reference<T>::type>::type> {};

    /**
     * 判断是否为数组类型
     */
    template <typename>
    struct is_array : false_type {};

    template <typename T>
    struct is_array<T[]> : true_type {};

    template <typename T, std::size_t N>
    struct is_array<T[N]> : true_type {};

    /**
     * 判断是否为函数类型
     */
    template <typename>
    struct is_function_helper : false_type {};

    template <typename Ret, typename... Args>
    struct is_function_helper<Ret(Args...)> : true_type {};

    template <typename Ret, typename... Args>
    struct is_function_helper<Ret(Args...) const> : true_type {};

    template <typename Ret, typename... Args>
    struct is_function_helper<Ret(Args...) volatile> : true_type {};

    template <typename Ret, typename... Args>
    struct is_function_helper<Ret(Args...) const volatile> : true_type {};

    template <typename T>
    struct is_function 
    : is_function_helper<typename remove_cv<typename remove_reference<T>::type>::type> {};

   /**
    * 上面的类型封装最主要的原因是为了在编译器进行逻辑判断
    * 下面实现选择逻辑，也就是当条件成立时，选择某一个类型，不成立时选择另一个类型
    * 当第一个参数为true时，type就与T1相同，否则就与T2相同。
    */
    template <bool judge, typename T1, typename T2>
    struct conditional {  using type = T1;  };

    template <typename T1, typename T2>
    struct conditional<false, T1, T2> {  using type = T2;  };

    /**
     * 判断类型是否相同
     */
    template <typename, typename>
    struct is_same : false_type {};

    template <typename T>
    struct is_same<T, T> : true_type {};

    /**
     * enable_if - SFINAE的核心工具
     * 当条件为true时，type为T；当条件为false时，没有type成员
     */
    template <bool B, typename T = void>
    struct enable_if {};

    template <typename T>
    struct enable_if<true, T> { using type = T; };

    /**
     * 添加引用
     */
    template <typename T>
    struct add_lvalue_reference { using type = T&; };

    template <typename T>
    struct add_lvalue_reference<T&> { using type = T&; };

    template <typename T>
    struct add_lvalue_reference<T&&> { using type = T&; };

    template <>
    struct add_lvalue_reference<void> { using type = void; };

    template <>
    struct add_lvalue_reference<const void> { using type = const void; };

    template <>
    struct add_lvalue_reference<volatile void> { using type = volatile void; };

    template <>
    struct add_lvalue_reference<const volatile void> { using type = const volatile void; };

    template <typename T>
    struct add_rvalue_reference { using type = T&&; };

    template <typename T>
    struct add_rvalue_reference<T&> { using type = T&; };

    template <typename T>
    struct add_rvalue_reference<T&&> { using type = T&&; };

    template <>
    struct add_rvalue_reference<void> { using type = void; };

    template <>
    struct add_rvalue_reference<const void> { using type = const void; };

    template <>
    struct add_rvalue_reference<volatile void> { using type = volatile void; };

    template <>
    struct add_rvalue_reference<const volatile void> { using type = const volatile void; };

    /**
     * 添加const
     */
    template <typename T>
    struct add_const { using type = const T; };

    /**
     * 移除extent（数组维度）
     */
    template <typename T>
    struct remove_extent { using type = T; };

    template <typename T>
    struct remove_extent<T[]> { using type = T; };

    template <typename T, std::size_t N>
    struct remove_extent<T[N]> { using type = T; };

    /**
     * 添加指针
     */
    template <typename T>
    struct add_pointer { using type = typename remove_reference<T>::type*; };

    /**
     * decay - 类型衰减
     */
    template <typename T>
    struct decay {
    private:
        using U = typename remove_reference<T>::type;
    public:
        using type = typename conditional<
            is_array<U>::value,
            typename remove_cv<typename remove_extent<U>::type>::type*,
            typename conditional<
                is_function<U>::value,
                typename add_pointer<U>::type,
                typename remove_cv<U>::type
            >::type
        >::type;
    };

    /**
     * 判断是否为左值引用
     */
    template <typename T>
    struct is_lvalue_reference : false_type {};

    template <typename T>
    struct is_lvalue_reference<T&> : true_type {};

    /**
     * 判断是否为右值引用
     */
    template <typename T>
    struct is_rvalue_reference : false_type {};

    template <typename T>
    struct is_rvalue_reference<T&&> : true_type {};

    /**
     * 判断是否为引用
     */
    template <typename T>
    struct is_reference : conditional<
        is_lvalue_reference<T>::value || is_rvalue_reference<T>::value,
        true_type,
        false_type
    >::type {};

    /**
     * 判断是否为nullptr_t
     */
    template <typename T>
    struct is_null_pointer : is_same<typename remove_cv<T>::type, decltype(nullptr)> {};

    /**
     * 类型别名
     */
    template <typename T>
    using remove_const_t = typename remove_const<T>::type;

    template <typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template <typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    template <typename T>
    using add_const_t = typename add_const<T>::type;

    template <typename T>
    using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

    template <typename T>
    using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

    template <typename T>
    using decay_t = typename decay<T>::type;

    template <typename T>
    using add_pointer_t = typename add_pointer<T>::type;

    template <typename T>
    using remove_extent_t = typename remove_extent<T>::type;

    template <bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template <bool B, typename T1, typename T2>
    using conditional_t = typename conditional<B, T1, T2>::type;

    /**
     * declval 函数声明，用于SFINAE
     */
    template <typename T>
    add_rvalue_reference_t<T> declval() noexcept;

    /**
     * 判断是否可构造
     */
    template <typename T, typename... Args>
    struct is_constructible {
    private:
        template <typename U>
        static auto test(int) -> decltype(U(declval<Args>()...), true_type());
        template <typename>
        static false_type test(...);
    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    /**
     * 判断是否可转换
     */
    template <typename From, typename To>
    struct is_convertible {
    private:
        template <typename U>
        static auto test(int) -> decltype(static_cast<To>(declval<U>()), true_type());
        template <typename>
        static false_type test(...);
    public:
        static constexpr bool value = decltype(test<From>(0))::value;
    };

    /**
     * 判断是否可赋值
     */
    template <typename T, typename U = T>
    struct is_assignable {
    private:
        template <typename V>
        static auto test(int) -> decltype(declval<V>() = declval<U>(), true_type());
        template <typename>
        static false_type test(...);
    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    /**
     * 判断是否为基类
     * 用SFINAE和sizeof实现，避免static_cast导致的编译错误
     */
    template <typename Base, typename Derived>
    struct is_base_of {
    private:
        static char test(const Base*);
        static int test(...);
    public:
        static constexpr bool value =
            sizeof(test(static_cast<Derived*>(nullptr))) == sizeof(char) &&
            !is_same<Base, Derived>::value;
    };

    /**
     * 判断是否可移动构造
     */
    template <typename T>
    struct is_move_constructible : is_constructible<T, T&&> {};

    /**
     * 判断是否可拷贝构造
     */
    template <typename T>
    struct is_copy_constructible : is_constructible<T, const T&> {};

    /**
     * 判断是否可移动赋值
     */
    template <typename T>
    struct is_move_assignable : is_assignable<T, T&&> {};

    /**
     * 判断是否可拷贝赋值
     */
    template <typename T>
    struct is_copy_assignable : is_assignable<T, const T&> {};

    /**
     * 判断是否为算术类型
     */
    template <typename T>
    struct is_arithmetic 
    : conditional<
        is_integer<typename remove_cv<typename remove_reference<T>::type>::type>::value ||
        is_float<typename remove_cv<typename remove_reference<T>::type>::type>::value,
        true_type, 
        false_type
    >::type {};

    /**
     * 判断是否为标量类型（算术类型、枚举、指针、成员指针、nullptr_t）
     */
    template <typename T>
    struct is_scalar 
    : conditional<
        is_arithmetic<typename remove_cv<typename remove_reference<T>::type>::type>::value ||
        is_pointer<typename remove_cv<typename remove_reference<T>::type>::type>::value ||
        is_null_pointer<typename remove_cv<typename remove_reference<T>::type>::type>::value,
        true_type, 
        false_type
    >::type {};

    /**
     * 判断是否为对象类型（非函数、非引用、非void）
     */
    template <typename T>
    struct is_object 
    : conditional<
        !is_function<typename remove_cv<typename remove_reference<T>::type>::type>::value &&
        !is_same<typename remove_cv<typename remove_reference<T>::type>::type, void>::value,
        true_type,
        false_type
    >::type {};

    /**
     * 判断是否为平凡类型（简化版本）
     */
    template <typename T>
    struct is_trivial : false_type {};

    template <>
    struct is_trivial<bool> : true_type {};

    template <>
    struct is_trivial<char> : true_type {};

    template <>
    struct is_trivial<signed char> : true_type {};

    template <>
    struct is_trivial<unsigned char> : true_type {};

    template <>
    struct is_trivial<short> : true_type {};

    template <>
    struct is_trivial<unsigned short> : true_type {};

    template <>
    struct is_trivial<int> : true_type {};

    template <>
    struct is_trivial<unsigned int> : true_type {};

    template <>
    struct is_trivial<long> : true_type {};

    template <>
    struct is_trivial<unsigned long> : true_type {};

    template <>
    struct is_trivial<long long> : true_type {};

    template <>
    struct is_trivial<unsigned long long> : true_type {};

    template <>
    struct is_trivial<float> : true_type {};

    template <>
    struct is_trivial<double> : true_type {};

    template <>
    struct is_trivial<long double> : true_type {};

    template <>
    struct is_trivial<decltype(nullptr)> : true_type {};

    template <typename T>
    using is_integral = is_integer<T>;

} // namespace sugar

#endif // TYPE_TRAITS_H_