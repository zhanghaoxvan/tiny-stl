/**
 * @file iterator.hpp
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include <cstddef> // 包含 ptrdiff_t
#include "utility.hpp" // 包含 remove_reference, forward, move, swap 等

namespace tiny_stl {

    // 迭代器标签
    struct input_iterator_tag {};
    struct output_iterator_tag {};
    struct forward_iterator_tag : public input_iterator_tag {};
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    // 迭代器特性模板
    template <typename T>
    struct iterator_traits {
        using iterator_category = typename T::iterator_category;
        using value_type = typename T::value_type;
        using difference_type = typename T::difference_type;
        using pointer = typename T::pointer;
        using reference = typename T::reference;
    };

    // 指针特化
    template <typename T>
    struct iterator_traits<T*> {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    };

    // 常量指针特化
    template <typename T>
    struct iterator_traits<const T*> {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
    };

    template <typename T,
            typename Category,
            typename DifferenceType,
            typename Pointer,
            typename Reference>
    struct iterator {
        using iterator_category = Category;
        using value_type = T;
        using difference_type = DifferenceType;
        using pointer = Pointer;
        using reference = Reference;
    };

    template <typename InputIterator>
    auto distance_aux(
        InputIterator first,
        InputIterator last,
        input_iterator_tag
    )
    -> typename iterator_traits<InputIterator>::difference_type {
        typename iterator_traits<InputIterator>::difference_type n;
        while (first != last) {
            first++;
            n++;
        }
        return n;
    }


    template <typename RandomAccessIterator>
    auto distance_aux(
        RandomAccessIterator first,
        RandomAccessIterator last,
        random_access_iterator_tag
    )
    -> typename iterator_traits<RandomAccessIterator>::difference_type {
        return last - first;
    }

    template <typename InputIterator>
    auto distance(
        InputIterator first,
        InputIterator last
    ) 
        -> typename iterator_traits<InputIterator>::difference_type {
        typename iterator_traits<InputIterator>::iterator_category category;
        return distance_aux(first, last, category);
    }

    template <typename InputIterator, typename Distance>
    void advance_aux(
        InputIterator it,
        Distance n,
        input_iterator_tag
    ) {
        while (n--) {
            it++;
        }
    }

    template <class BidirectionalIterator, typename Distance>
    void advance_aux(
        BidirectionalIterator it,
        Distance n,
        bidirectional_iterator_tag
    ) {
        if (n >= 0) {
            while (n--) {
                it++;
            }
        } else {
            while (n++) {
                it--;
            }
        }
    }

    template <class RandomAccessIterator, typename Distance>
    void advance_aux(
        RandomAccessIterator it,
        Distance n,
        random_access_iterator_tag
    ) {
        it += n;
    }

    template <typename InputIterator, typename Distance>
    void advance(
        InputIterator it,
        Distance n
    ) {
        typename iterator_traits<InputIterator>::iterator_category category;
        return advance_aux(it, n, category);
    }

    /**
     * @class reverse_iterator
     * @brief 反向迭代器适配器
     * 
     * 将双向迭代器或随机访问迭代器的方向反转
     */
    template <typename Iterator>
    class reverse_iterator {
    public:
        // 成员类型定义
        using iterator_type = Iterator;
        using iterator_category = typename iterator_traits<Iterator>::iterator_category;
        using value_type = typename iterator_traits<Iterator>::value_type;
        using difference_type = typename iterator_traits<Iterator>::difference_type;
        using pointer = typename iterator_traits<Iterator>::pointer;
        using reference = typename iterator_traits<Iterator>::reference;

        // 默认构造函数
        reverse_iterator() : current() {}

        // 从正向迭代器构造
        explicit reverse_iterator(Iterator x) : current(x) {}

        // 从其他反向迭代器构造
        template <typename U>
        reverse_iterator(const reverse_iterator<U>& other) 
            : current(other.base()) {}

        // 获取底层迭代器
        Iterator base() const { return current; }

        // 解引用操作符
        reference operator*() const {
            Iterator tmp = current;
            return *--tmp;
        }

        // 箭头操作符
        pointer operator->() const {
            return &(operator*());
        }

        // 前置递增
        reverse_iterator& operator++() {
            --current;
            return *this;
        }

        // 后置递增
        reverse_iterator operator++(int) {
            reverse_iterator tmp(*this);
            --current;
            return tmp;
        }

        // 前置递减
        reverse_iterator& operator--() {
            ++current;
            return *this;
        }

        // 后置递减
        reverse_iterator operator--(int) {
            reverse_iterator tmp(*this);
            ++current;
            return tmp;
        }

        // 随机访问：加法
        reverse_iterator operator+(difference_type n) const {
            return reverse_iterator(current - n);
        }

        // 随机访问：复合加法
        reverse_iterator& operator+=(difference_type n) {
            current -= n;
            return *this;
        }

        // 随机访问：减法
        reverse_iterator operator-(difference_type n) const {
            return reverse_iterator(current + n);
        }

        // 随机访问：复合减法
        reverse_iterator& operator-=(difference_type n) {
            current += n;
            return *this;
        }

        // 随机访问：下标操作符
        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        // 比较操作符
        template <typename It>
        bool operator==(const reverse_iterator<It>& other) const {
            return current == other.base();
        }

        template <typename It>
        bool operator!=(const reverse_iterator<It>& other) const {
            return !(*this == other);
        }

        template <typename It>
        bool operator<(const reverse_iterator<It>& other) const {
            return current > other.base();
        }

        template <typename It>
        bool operator<=(const reverse_iterator<It>& other) const {
            return !(other < *this);
        }

        template <typename It>
        bool operator>(const reverse_iterator<It>& other) const {
            return other < *this;
        }

        template <typename It>
        bool operator>=(const reverse_iterator<It>& other) const {
            return !(*this < other);
        }

    private:
        Iterator current; // 底层迭代器
    };

    // 非成员函数：加法
    template <typename Iterator>
    reverse_iterator<Iterator> operator+(
        typename reverse_iterator<Iterator>::difference_type n,
        const reverse_iterator<Iterator>& it) {
        return it + n;
    }

    // 非成员函数：减法
    template <typename Iterator1, typename Iterator2>
    auto operator-(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs)
        -> decltype(rhs.base() - lhs.base()) {
        return rhs.base() - lhs.base();
    }

    // 关系操作符
    template <typename Iterator1, typename Iterator2>
    bool operator==(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return lhs.base() == rhs.base();
    }

    template <typename Iterator1, typename Iterator2>
    bool operator!=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return !(lhs == rhs);
    }

    template <typename Iterator1, typename Iterator2>
    bool operator<(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return lhs.base() > rhs.base();
    }

    template <typename Iterator1, typename Iterator2>
    bool operator<=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return !(rhs < lhs);
    }

    template <typename Iterator1, typename Iterator2>
    bool operator>(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return rhs < lhs;
    }

    template <typename Iterator1, typename Iterator2>
    bool operator>=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return !(lhs < rhs);
    }

} // namespace tiny_stl
