/*
 * @file utility.h
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL 通用工具函数与pair类型，完全独立实现，不依赖std
 */

#ifndef SUGAR_UTILITY_H_
#define SUGAR_UTILITY_H_

#include "type_traits.h"

namespace sugar {

// ============================ 通用工具函数 ============================

/**
 * @brief forward 函数模板，实现完美转发
 */
template<typename T>
constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

/**
 * @brief forward 函数模板，实现完美转发（右值）
 */
template<typename T>
constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
    static_assert(!is_lvalue_reference<T>::value, "bad forward call");
    return static_cast<T&&>(t);
}

/**
 * @brief move 函数模板，实现移动语义
 */
template<typename T>
constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(t);
}

/**
 * @brief swap 函数模板，交换两个值
 */
template<typename T>
void swap(T& a, T& b) noexcept(noexcept(T(sugar::move(a))) && noexcept(a = sugar::move(b))) {
    T temp = sugar::move(a);
    a = sugar::move(b);
    b = sugar::move(temp);
}

// ============================ pair 类型 ============================

/**
 * @brief pair 类模板，存储两个不同类型的值
 */
template<typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    /**
     * @brief 默认构造函数
     */
    constexpr pair() : first(), second() {}

    /**
     * @brief 拷贝构造函数
     */
    constexpr pair(const T1& x, const T2& y) : first(x), second(y) {}

    /**
     * @brief 移动构造函数
     */
    constexpr pair(T1&& x, T2&& y) : first(sugar::move(x)), second(sugar::move(y)) {}

    /**
     * @brief 混合构造函数
     */
    constexpr pair(const T1& x, T2&& y) : first(x), second(sugar::move(y)) {}
    constexpr pair(T1&& x, const T2& y) : first(sugar::move(x)), second(y) {}

    /**
     * @brief 模板拷贝构造函数
     */
    template<typename U1, typename U2>
    constexpr pair(const pair<U1, U2>& p) : first(static_cast<T1>(p.first)), second(static_cast<T2>(p.second)) {}

    /**
     * @brief 模板移动构造函数
     */
    template<typename U1, typename U2>
    constexpr pair(pair<U1, U2>&& p) : first(static_cast<T1>(sugar::move(p.first))), second(static_cast<T2>(sugar::move(p.second))) {}

    /**
     * @brief 赋值操作符
     */
    pair& operator=(const pair& p) {
        if (this != &p) {
            first = p.first;
            second = p.second;
        }
        return *this;
    }

    /**
     * @brief 移动赋值操作符
     */
    pair& operator=(pair&& p) noexcept {
        if (this != &p) {
            first = sugar::move(p.first);
            second = sugar::move(p.second);
        }
        return *this;
    }

    /**
     * @brief 模板赋值操作符
     */
    template<typename U1, typename U2>
    pair& operator=(const pair<U1, U2>& p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    template<typename U1, typename U2>
    pair& operator=(pair<U1, U2>&& p) {
        first = sugar::move(p.first);
        second = sugar::move(p.second);
        return *this;
    }

    /**
     * @brief 析构函数
     */
    ~pair() = default;

    /**
     * @brief swap 函数
     */
    void swap(pair& p) noexcept(noexcept(sugar::swap(first, p.first)) && 
                                noexcept(sugar::swap(second, p.second))) {
        sugar::swap(first, p.first);
        sugar::swap(second, p.second);
    }
};

// ============================ pair 相关函数 ============================

/**
 * @brief make_pair 函数，创建pair对象
 */
template<typename T1, typename T2>
constexpr pair<typename decay<T1>::type, typename decay<T2>::type>
make_pair(T1&& x, T2&& y) {
    return pair<typename decay<T1>::type, typename decay<T2>::type>(
        sugar::forward<T1>(x), sugar::forward<T2>(y));
}

/**
 * @brief pair 比较操作符
 */
template<typename T1, typename T2>
constexpr bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<typename T1, typename T2>
constexpr bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs == rhs);
}

template<typename T1, typename T2>
constexpr bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

template<typename T1, typename T2>
constexpr bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(rhs < lhs);
}

template<typename T1, typename T2>
constexpr bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return rhs < lhs;
}

template<typename T1, typename T2>
constexpr bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
    return !(lhs < rhs);
}

// ============================ 其他工具函数 ============================

/**
 * @brief exchange 函数，交换并返回旧值
 */
template<typename T, typename U = T>
T exchange(T& obj, U&& new_val) {
    T old_val = sugar::move(obj);
    obj = sugar::forward<U>(new_val);
    return old_val;
}

/**
 * @brief as_const 函数，添加const限定符
 */
template<typename T>
constexpr add_const_t<T>& as_const(T& t) noexcept {
    return t;
}

template<typename T>
void as_const(const T&&) = delete;

/**
 * @brief declval 函数，用于decltype
 */
template<typename T>
add_rvalue_reference_t<T> declval() noexcept;

} // namespace sugar

#endif // SUGAR_UTILITY_H_ 