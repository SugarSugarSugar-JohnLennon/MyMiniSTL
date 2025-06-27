/*
 * @file algorithm.h
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL 算法库，完全独立实现，不依赖std
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "iterator.h"
#include "type_traits.h"
#include "utility.h"

namespace sugar {

// ============================ 基础算法 ============================

/**
 * @brief 查找算法
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param value 要查找的值
 * @return 指向第一个匹配元素的迭代器，如果未找到返回last
 */
template<typename InputIt, typename T>
InputIt find(InputIt first, InputIt last, const T& value) {
    for (; first != last; ++first) {
        if (*first == value) {
            return first;
        }
    }
    return last;
}

/**
 * @brief 查找算法（使用谓词）
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param pred 谓词函数
 * @return 指向第一个满足谓词的元素的迭代器，如果未找到返回last
 */
template<typename InputIt, typename UnaryPredicate>
InputIt find_if(InputIt first, InputIt last, UnaryPredicate pred) {
    for (; first != last; ++first) {
        if (pred(*first)) {
            return first;
        }
    }
    return last;
}

/**
 * @brief 复制算法
 * @param first 源起始迭代器
 * @param last 源结束迭代器
 * @param d_first 目标起始迭代器
 * @return 指向目标范围末尾的迭代器
 */
template<typename InputIt, typename OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = *first;
    }
    return d_first;
}

/**
 * @brief 移动复制算法
 * @param first 源起始迭代器
 * @param last 源结束迭代器
 * @param d_first 目标起始迭代器
 * @return 指向目标范围末尾的迭代器
 */
template<typename InputIt, typename OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt d_first) {
    for (; first != last; ++first, ++d_first) {
        *d_first = sugar::move(*first);
    }
    return d_first;
}

/**
 * @brief 填充算法
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param value 填充值
 */
template<typename ForwardIt, typename T>
void fill(ForwardIt first, ForwardIt last, const T& value) {
    for (; first != last; ++first) {
        *first = value;
    }
}

/**
 * @brief 填充n个元素
 * @param first 起始迭代器
 * @param n 元素数量
 * @param value 填充值
 * @return 指向填充范围末尾的迭代器
 */
template<typename OutputIt, typename Size, typename T>
OutputIt fill_n(OutputIt first, Size n, const T& value) {
    for (Size i = 0; i < n; ++i, ++first) {
        *first = value;
    }
    return first;
}

/**
 * @brief 交换范围算法
 * @param first1 第一个范围的起始迭代器
 * @param last1 第一个范围的结束迭代器
 * @param first2 第二个范围的起始迭代器
 * @return 指向第二个范围末尾的迭代器
 */
template<typename ForwardIt1, typename ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        sugar::swap(*first1, *first2);
    }
    return first2;
}

/**
 * @brief 字典序比较算法
 * @param first1 第一个范围的起始迭代器
 * @param last1 第一个范围的结束迭代器
 * @param first2 第二个范围的起始迭代器
 * @param last2 第二个范围的结束迭代器
 * @return 如果第一个范围字典序小于第二个范围返回true，否则返回false
 */
template<typename InputIt1, typename InputIt2>
bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2) {
            return true;
        }
        if (*first2 < *first1) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

/**
 * @brief 字典序比较算法（使用比较函数）
 * @param first1 第一个范围的起始迭代器
 * @param last1 第一个范围的结束迭代器
 * @param first2 第二个范围的起始迭代器
 * @param last2 第二个范围的结束迭代器
 * @param comp 比较函数
 * @return 如果第一个范围字典序小于第二个范围返回true，否则返回false
 */
template<typename InputIt1, typename InputIt2, typename Compare>
bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, Compare comp) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (comp(*first1, *first2)) {
            return true;
        }
        if (comp(*first2, *first1)) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

/**
 * @brief 相等比较算法
 * @param first1 第一个范围的起始迭代器
 * @param last1 第一个范围的结束迭代器
 * @param first2 第二个范围的起始迭代器
 * @return 如果两个范围相等返回true，否则返回false
 */
template<typename InputIt1, typename InputIt2>
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 相等比较算法（使用比较函数）
 * @param first1 第一个范围的起始迭代器
 * @param last1 第一个范围的结束迭代器
 * @param first2 第二个范围的起始迭代器
 * @param pred 比较函数
 * @return 如果两个范围相等返回true，否则返回false
 */
template<typename InputIt1, typename InputIt2, typename BinaryPredicate>
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate pred) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 计数算法
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param value 要计数的值
 * @return 值的出现次数
 */
template<typename InputIt, typename T>
typename iterator_traits<InputIt>::difference_type count(InputIt first, InputIt last, const T& value) {
    typename iterator_traits<InputIt>::difference_type result = 0;
    for (; first != last; ++first) {
        if (*first == value) {
            ++result;
        }
    }
    return result;
}

/**
 * @brief 计数算法（使用谓词）
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param pred 谓词函数
 * @return 满足谓词的元素的个数
 */
template<typename InputIt, typename UnaryPredicate>
typename iterator_traits<InputIt>::difference_type count_if(InputIt first, InputIt last, UnaryPredicate pred) {
    typename iterator_traits<InputIt>::difference_type result = 0;
    for (; first != last; ++first) {
        if (pred(*first)) {
            ++result;
        }
    }
    return result;
}

/**
 * @brief 最小值算法
 * @param a 第一个值
 * @param b 第二个值
 * @return 较小的值
 */
template<typename T>
const T& min(const T& a, const T& b) {
    return (b < a) ? b : a;
}

/**
 * @brief 最小值算法（使用比较函数）
 * @param a 第一个值
 * @param b 第二个值
 * @param comp 比较函数
 * @return 较小的值
 */
template<typename T, typename Compare>
const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

/**
 * @brief 最大值算法
 * @param a 第一个值
 * @param b 第二个值
 * @return 较大的值
 */
template<typename T>
const T& max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

/**
 * @brief 最大值算法（使用比较函数）
 * @param a 第一个值
 * @param b 第二个值
 * @param comp 比较函数
 * @return 较大的值
 */
template<typename T, typename Compare>
const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

/**
 * @brief 最小最大值算法
 * @param a 第一个值
 * @param b 第二个值
 * @return 包含最小值和最大值的pair
 */
template<typename T>
sugar::pair<const T&, const T&> minmax(const T& a, const T& b) {
    return (b < a) ? sugar::make_pair(b, a) : sugar::make_pair(a, b);
}

/**
 * @brief 最小最大值算法（使用比较函数）
 * @param a 第一个值
 * @param b 第二个值
 * @param comp 比较函数
 * @return 包含最小值和最大值的pair
 */
template<typename T, typename Compare>
sugar::pair<const T&, const T&> minmax(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? sugar::make_pair(b, a) : sugar::make_pair(a, b);
}

/**
 * @brief 范围最小值算法
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @return 指向最小元素的迭代器
 */
template<typename ForwardIt>
ForwardIt min_element(ForwardIt first, ForwardIt last) {
    if (first == last) {
        return last;
    }
    
    ForwardIt smallest = first;
    ++first;
    for (; first != last; ++first) {
        if (*first < *smallest) {
            smallest = first;
        }
    }
    return smallest;
}

/**
 * @brief 范围最小值算法（使用比较函数）
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param comp 比较函数
 * @return 指向最小元素的迭代器
 */
template<typename ForwardIt, typename Compare>
ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp) {
    if (first == last) {
        return last;
    }
    
    ForwardIt smallest = first;
    ++first;
    for (; first != last; ++first) {
        if (comp(*first, *smallest)) {
            smallest = first;
        }
    }
    return smallest;
}

/**
 * @brief 范围最大值算法
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @return 指向最大元素的迭代器
 */
template<typename ForwardIt>
ForwardIt max_element(ForwardIt first, ForwardIt last) {
    if (first == last) {
        return last;
    }
    
    ForwardIt largest = first;
    ++first;
    for (; first != last; ++first) {
        if (*largest < *first) {
            largest = first;
        }
    }
    return largest;
}

/**
 * @brief 范围最大值算法（使用比较函数）
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param comp 比较函数
 * @return 指向最大元素的迭代器
 */
template<typename ForwardIt, typename Compare>
ForwardIt max_element(ForwardIt first, ForwardIt last, Compare comp) {
    if (first == last) {
        return last;
    }
    
    ForwardIt largest = first;
    ++first;
    for (; first != last; ++first) {
        if (comp(*largest, *first)) {
            largest = first;
        }
    }
    return largest;
}

/**
 * @brief 范围最小最大值算法
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @return 包含指向最小和最大元素的迭代器的pair
 */
template<typename ForwardIt>
sugar::pair<ForwardIt, ForwardIt> minmax_element(ForwardIt first, ForwardIt last) {
    if (first == last) {
        return sugar::make_pair(last, last);
    }
    
    ForwardIt smallest = first;
    ForwardIt largest = first;
    ++first;
    
    for (; first != last; ++first) {
        if (*first < *smallest) {
            smallest = first;
        } else if (*largest < *first) {
            largest = first;
        }
    }
    
    return sugar::make_pair(smallest, largest);
}

/**
 * @brief 范围最小最大值算法（使用比较函数）
 * @param first 起始迭代器
 * @param last 结束迭代器
 * @param comp 比较函数
 * @return 包含指向最小和最大元素的迭代器的pair
 */
template<typename ForwardIt, typename Compare>
sugar::pair<ForwardIt, ForwardIt> minmax_element(ForwardIt first, ForwardIt last, Compare comp) {
    if (first == last) {
        return sugar::make_pair(last, last);
    }
    
    ForwardIt smallest = first;
    ForwardIt largest = first;
    ++first;
    
    for (; first != last; ++first) {
        if (comp(*first, *smallest)) {
            smallest = first;
        } else if (comp(*largest, *first)) {
            largest = first;
        }
    }
    
    return sugar::make_pair(smallest, largest);
}

} // namespace sugar

#endif // ALGORITHM_H_ 