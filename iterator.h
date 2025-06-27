/*
 * @file iterator.h
 * @author sugar
 * @date 2024-06-09
 * @brief MyMiniSTL 迭代器系统，完全独立实现，不依赖std
 */

#ifndef ITERATOR_H_
#define ITERATOR_H_

#include "type_traits.h"
#include "exceptdef.h"

namespace sugar {

// ============================ 迭代器分类标签 ============================

/**
 * @brief 迭代器分类标签，用于标识迭代器的能力和特性
 */
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// ============================ iterator_traits ============================

/**
 * @brief iterator_traits 主模板，为自定义迭代器类型提供特性
 */
template<typename Iterator>
struct iterator_traits {
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};

/**
 * @brief iterator_traits 针对原生指针的特化
 */
template<typename T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

/**
 * @brief iterator_traits 针对const原生指针的特化
 */
template<typename T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};

// ============================ 迭代器基类 ============================

/**
 * @brief 迭代器基类，定义迭代器的基本类型
 */
template<typename Category, typename T, typename Distance = ptrdiff_t,
         typename Pointer = T*, typename Reference = T&>
struct iterator {
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

// ============================ 迭代器操作函数 ============================

/**
 * @brief 计算两个迭代器之间的距离
 */
template<typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return distance_dispatch(first, last, 
        typename iterator_traits<InputIterator>::iterator_category());
}

/**
 * @brief 输入迭代器距离计算
 */
template<typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

/**
 * @brief 随机访问迭代器距离计算
 */
template<typename RandomAccessIterator>
typename iterator_traits<RandomAccessIterator>::difference_type
distance_dispatch(RandomAccessIterator first, RandomAccessIterator last, 
                 random_access_iterator_tag) {
    return last - first;
}

/**
 * @brief 使迭代器前进指定距离
 */
template<typename InputIterator, typename Distance>
void advance(InputIterator& it, Distance n) {
    advance_dispatch(it, n, 
        typename iterator_traits<InputIterator>::iterator_category());
}

/**
 * @brief 输入迭代器前进
 */
template<typename InputIterator, typename Distance>
void advance_dispatch(InputIterator& it, Distance n, input_iterator_tag) {
    SUGAR_CONDITION_CHECK(n >= 0, "Cannot advance input iterator backwards");
    while (n--) ++it;
}

/**
 * @brief 双向迭代器前进/后退
 */
template<typename BidirectionalIterator, typename Distance>
void advance_dispatch(BidirectionalIterator& it, Distance n, bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n--) ++it;
    } else {
        while (n++) --it;
    }
}

/**
 * @brief 随机访问迭代器前进
 */
template<typename RandomAccessIterator, typename Distance>
void advance_dispatch(RandomAccessIterator& it, Distance n, random_access_iterator_tag) {
    it += n;
}

/**
 * @brief 返回前进n步后的迭代器
 */
template<typename ForwardIterator>
ForwardIterator next(ForwardIterator it, 
                    typename iterator_traits<ForwardIterator>::difference_type n = 1) {
    advance(it, n);
    return it;
}

/**
 * @brief 返回后退n步后的迭代器
 */
template<typename BidirectionalIterator>
BidirectionalIterator prev(BidirectionalIterator it,
                          typename iterator_traits<BidirectionalIterator>::difference_type n = 1) {
    advance(it, -n);
    return it;
}

// ============================ reverse_iterator ============================

/**
 * reverse_iterator - 反向迭代器适配器
 */
template<typename Iterator>
class reverse_iterator : public iterator<
    typename iterator_traits<Iterator>::iterator_category,
    typename iterator_traits<Iterator>::value_type,
    typename iterator_traits<Iterator>::difference_type,
    typename iterator_traits<Iterator>::pointer,
    typename iterator_traits<Iterator>::reference> {
private:
    Iterator current_;

public:
    using iterator_type = Iterator;
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;

    // 构造函数
    reverse_iterator() : current_() {}
    explicit reverse_iterator(Iterator it) : current_(it) {}
    template<typename U>
    reverse_iterator(const reverse_iterator<U>& other) : current_(other.base()) {}

    // 基础迭代器访问
    Iterator base() const { return current_; }

    // 解引用操作
    reference operator*() const {
        Iterator tmp = current_;
        return *--tmp;
    }

    pointer operator->() const {
        return &(operator*());
    }

    // 下标操作
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    // 自增自减
    reverse_iterator& operator++() {
        --current_;
        return *this;
    }

    reverse_iterator operator++(int) {
        reverse_iterator tmp = *this;
        --current_;
        return tmp;
    }

    reverse_iterator& operator--() {
        ++current_;
        return *this;
    }

    reverse_iterator operator--(int) {
        reverse_iterator tmp = *this;
        ++current_;
        return tmp;
    }

    // 算术运算
    reverse_iterator& operator+=(difference_type n) {
        current_ -= n;
        return *this;
    }

    reverse_iterator& operator-=(difference_type n) {
        current_ += n;
        return *this;
    }

    reverse_iterator operator+(difference_type n) const {
        return reverse_iterator(current_ - n);
    }

    reverse_iterator operator-(difference_type n) const {
        return reverse_iterator(current_ + n);
    }
};

// reverse_iterator 比较操作
template<typename Iterator1, typename Iterator2>
bool operator==(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return lhs.base() == rhs.base();
}

template<typename Iterator1, typename Iterator2>
bool operator!=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return !(lhs == rhs);
}

template<typename Iterator1, typename Iterator2>
bool operator<(const reverse_iterator<Iterator1>& lhs,
               const reverse_iterator<Iterator2>& rhs) {
    return rhs.base() < lhs.base();
}

template<typename Iterator1, typename Iterator2>
bool operator<=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return !(rhs < lhs);
}

template<typename Iterator1, typename Iterator2>
bool operator>(const reverse_iterator<Iterator1>& lhs,
               const reverse_iterator<Iterator2>& rhs) {
    return rhs < lhs;
}

template<typename Iterator1, typename Iterator2>
bool operator>=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
    return !(lhs < rhs);
}

template<typename Iterator>
reverse_iterator<Iterator> operator+(typename reverse_iterator<Iterator>::difference_type n,
                                   const reverse_iterator<Iterator>& it) {
    return it + n;
}

template<typename Iterator1, typename Iterator2>
typename reverse_iterator<Iterator1>::difference_type
operator-(const reverse_iterator<Iterator1>& lhs,
          const reverse_iterator<Iterator2>& rhs) {
    return rhs.base() - lhs.base();
}

// ============================ 插入迭代器 ============================

/**
 * insert_iterator - 插入迭代器
 */
template<typename Container>
class insert_iterator : public iterator<output_iterator_tag, void, void, void, void> {
private:
    Container* container_;
    typename Container::iterator iter_;

public:
    using container_type = Container;

    insert_iterator(Container& c, typename Container::iterator i)
        : container_(&c), iter_(i) {}

    insert_iterator& operator=(const typename Container::value_type& value) {
        iter_ = container_->insert(iter_, value);
        ++iter_;
        return *this;
    }

    insert_iterator& operator*() { return *this; }
    insert_iterator& operator++() { return *this; }
    insert_iterator& operator++(int) { return *this; }
};

/**
 * back_insert_iterator - 尾部插入迭代器
 */
template<typename Container>
class back_insert_iterator : public iterator<output_iterator_tag, void, void, void, void> {
private:
    Container* container_;

public:
    using container_type = Container;

    explicit back_insert_iterator(Container& c) : container_(&c) {}

    back_insert_iterator& operator=(const typename Container::value_type& value) {
        container_->push_back(value);
        return *this;
    }

    back_insert_iterator& operator*() { return *this; }
    back_insert_iterator& operator++() { return *this; }
    back_insert_iterator& operator++(int) { return *this; }
};

/**
 * front_insert_iterator - 头部插入迭代器
 */
template<typename Container>
class front_insert_iterator : public iterator<output_iterator_tag, void, void, void, void> {
private:
    Container* container_;

public:
    using container_type = Container;

    explicit front_insert_iterator(Container& c) : container_(&c) {}

    front_insert_iterator& operator=(const typename Container::value_type& value) {
        container_->push_front(value);
        return *this;
    }

    front_insert_iterator& operator*() { return *this; }
    front_insert_iterator& operator++() { return *this; }
    front_insert_iterator& operator++(int) { return *this; }
};

// ============================ 工厂函数 ============================

/**
 * make_reverse_iterator - 创建反向迭代器
 */
template<typename Iterator>
reverse_iterator<Iterator> make_reverse_iterator(Iterator it) {
    return reverse_iterator<Iterator>(it);
}

/**
 * inserter - 创建插入迭代器
 */
template<typename Container>
insert_iterator<Container> inserter(Container& c, typename Container::iterator i) {
    return insert_iterator<Container>(c, i);
}

/**
 * back_inserter - 创建尾部插入迭代器
 */
template<typename Container>
back_insert_iterator<Container> back_inserter(Container& c) {
    return back_insert_iterator<Container>(c);
}

/**
 * front_inserter - 创建头部插入迭代器
 */
template<typename Container>
front_insert_iterator<Container> front_inserter(Container& c) {
    return front_insert_iterator<Container>(c);
}

// ============================ 便捷函数 ============================

/**
 * begin - 获取容器起始迭代器
 */
template<typename Container>
auto begin(Container& c) -> decltype(c.begin()) {
    return c.begin();
}

template<typename Container>
auto begin(const Container& c) -> decltype(c.begin()) {
    return c.begin();
}

template<typename T, size_t N>
T* begin(T (&array)[N]) {
    return array;
}

/**
 * end - 获取容器结束迭代器
 */
template<typename Container>
auto end(Container& c) -> decltype(c.end()) {
    return c.end();
}

template<typename Container>
auto end(const Container& c) -> decltype(c.end()) {
    return c.end();
}

template<typename T, size_t N>
T* end(T (&array)[N]) {
    return array + N;
}

/**
 * rbegin - 获取容器反向起始迭代器
 */
template<typename Container>
auto rbegin(Container& c) -> decltype(make_reverse_iterator(c.end())) {
    return make_reverse_iterator(c.end());
}

template<typename Container>
auto rbegin(const Container& c) -> decltype(make_reverse_iterator(c.end())) {
    return make_reverse_iterator(c.end());
}

/**
 * rend - 获取容器反向结束迭代器
 */
template<typename Container>
auto rend(Container& c) -> decltype(make_reverse_iterator(c.begin())) {
    return make_reverse_iterator(c.begin());
}

template<typename Container>
auto rend(const Container& c) -> decltype(make_reverse_iterator(c.begin())) {
    return make_reverse_iterator(c.begin());
}

// ============================ 类型别名 ============================

template<typename Iterator>
using reverse_iterator_t = reverse_iterator<Iterator>;

template<typename Container>
using insert_iterator_t = insert_iterator<Container>;

template<typename Container>
using back_insert_iterator_t = back_insert_iterator<Container>;

template<typename Container>
using front_insert_iterator_t = front_insert_iterator<Container>;

} // namespace sugar

#endif // ITERATOR_H_ 