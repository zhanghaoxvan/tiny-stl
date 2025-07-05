/**
 * @file iterator.hpp
 * @brief 该文件定义了迭代器相关的标签、特性模板、辅助函数以及反向迭代器适配器，用于支持容器的迭代操作，类似于标准库 `<iterator>` 头文件中的部分功能。
 * 
 * 迭代器是一种抽象的概念，用于提供对容器元素的访问和遍历。本文件中的类和函数为容器提供了统一的迭代器接口，使得不同容器可以以相同的方式被遍历和操作。
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

    /**
     * @brief 输入迭代器标签，用于标记只支持单向、单次遍历的迭代器。
     * 
     * 输入迭代器可以用于读取元素，但不保证可以多次读取相同的元素，也不支持随机访问。
     */
    struct input_iterator_tag {};

    /**
     * @brief 输出迭代器标签，用于标记只支持单向、单次写入的迭代器。
     * 
     * 输出迭代器可以用于写入元素，但不保证可以多次写入相同的位置，也不支持随机访问。
     */
    struct output_iterator_tag {};

    /**
     * @brief 前向迭代器标签，继承自输入迭代器标签，用于标记支持单向、多次遍历的迭代器。
     * 
     * 前向迭代器可以用于读取和写入元素，并且可以多次遍历相同的元素，但不支持随机访问。
     */
    struct forward_iterator_tag : public input_iterator_tag {};

    /**
     * @brief 双向迭代器标签，继承自前向迭代器标签，用于标记支持双向遍历的迭代器。
     * 
     * 双向迭代器可以用于读取和写入元素，支持向前和向后遍历，并且可以多次遍历相同的元素，但不支持随机访问。
     */
    struct bidirectional_iterator_tag : public forward_iterator_tag {};

    /**
     * @brief 随机访问迭代器标签，继承自双向迭代器标签，用于标记支持随机访问的迭代器。
     * 
     * 随机访问迭代器可以用于读取和写入元素，支持向前和向后遍历，并且可以在常数时间内访问任意位置的元素。
     */
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};

    /**
     * @class iterator_traits
     * @brief 迭代器特性模板，用于提取迭代器的相关类型信息。
     * 
     * 该模板提供了一种统一的方式来获取迭代器的类别、值类型、差值类型、指针类型和引用类型。
     * 对于自定义迭代器，只需要定义相应的内部类型，就可以通过该模板获取这些类型信息。
     * 
     * @tparam T 迭代器类型。
     */
    template <typename T>
    struct iterator_traits {
        /**
         * @brief 迭代器的类别。
         */
        using iterator_category = typename T::iterator_category;
        /**
         * @brief 迭代器指向元素的值类型。
         */
        using value_type = typename T::value_type;
        /**
         * @brief 迭代器的差值类型，用于表示两个迭代器之间的距离。
         */
        using difference_type = typename T::difference_type;
        /**
         * @brief 迭代器指向元素的指针类型。
         */
        using pointer = typename T::pointer;
        /**
         * @brief 迭代器指向元素的引用类型。
         */
        using reference = typename T::reference;
    };

    /**
     * @brief 指针的迭代器特性特化，用于处理普通指针类型。
     * 
     * 普通指针被视为随机访问迭代器，因此该特化版本将指针的迭代器类别设置为 `random_access_iterator_tag`。
     * 
     * @tparam T 指针指向的元素类型。
     */
    template <typename T>
    struct iterator_traits<T*> {
        /**
         * @brief 指针的迭代器类别为随机访问迭代器。
         */
        using iterator_category = random_access_iterator_tag;
        /**
         * @brief 指针指向元素的值类型。
         */
        using value_type = T;
        /**
         * @brief 指针的差值类型，通常为 `ptrdiff_t`。
         */
        using difference_type = ptrdiff_t;
        /**
         * @brief 指针类型。
         */
        using pointer = T*;
        /**
         * @brief 指针指向元素的引用类型。
         */
        using reference = T&;
    };

    /**
     * @brief 常量指针的迭代器特性特化，用于处理常量指针类型。
     * 
     * 常量指针同样被视为随机访问迭代器，该特化版本将常量指针的迭代器类别设置为 `random_access_iterator_tag`，
     * 并将指针和引用类型设置为常量类型。
     * 
     * @tparam T 常量指针指向的元素类型。
     */
    template <typename T>
    struct iterator_traits<const T*> {
        /**
         * @brief 常量指针的迭代器类别为随机访问迭代器。
         */
        using iterator_category = random_access_iterator_tag;
        /**
         * @brief 常量指针指向元素的值类型。
         */
        using value_type = T;
        /**
         * @brief 常量指针的差值类型，通常为 `ptrdiff_t`。
         */
        using difference_type = ptrdiff_t;
        /**
         * @brief 常量指针类型。
         */
        using pointer = const T*;
        /**
         * @brief 常量指针指向元素的引用类型。
         */
        using reference = const T&;
    };

    /**
     * @class iterator
     * @brief 迭代器基类模板，用于简化自定义迭代器的定义。
     * 
     * 自定义迭代器可以继承自该类，通过指定模板参数来定义迭代器的类别、值类型、差值类型、指针类型和引用类型。
     * 
     * @tparam T 迭代器指向元素的值类型。
     * @tparam Category 迭代器的类别。
     * @tparam DifferenceType 迭代器的差值类型。
     * @tparam Pointer 迭代器指向元素的指针类型。
     * @tparam Reference 迭代器指向元素的引用类型。
     */
    template <typename T,
            typename Category,
            typename DifferenceType,
            typename Pointer,
            typename Reference>
    struct iterator {
        /**
         * @brief 迭代器的类别。
         */
        using iterator_category = Category;
        /**
         * @brief 迭代器指向元素的值类型。
         */
        using value_type = T;
        /**
         * @brief 迭代器的差值类型，用于表示两个迭代器之间的距离。
         */
        using difference_type = DifferenceType;
        /**
         * @brief 迭代器指向元素的指针类型。
         */
        using pointer = Pointer;
        /**
         * @brief 迭代器指向元素的引用类型。
         */
        using reference = Reference;
    };

    /**
     * @brief 计算两个输入迭代器之间的距离的辅助函数。
     * 
     * 该函数通过遍历迭代器来计算距离，适用于输入迭代器。
     * 
     * @tparam InputIterator 输入迭代器类型。
     * @param first 起始迭代器。
     * @param last 结束迭代器。
     * @param tag 迭代器标签，用于标记迭代器的类别。
     * @return 两个迭代器之间的距离，类型为 `iterator_traits<InputIterator>::difference_type`。
     */
    template <typename InputIterator>
    auto distance_aux(
        InputIterator first,
        InputIterator last,
        input_iterator_tag
    )
    -> typename iterator_traits<InputIterator>::difference_type {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    /**
     * @brief 计算两个随机访问迭代器之间的距离的辅助函数。
     * 
     * 该函数通过直接相减来计算距离，适用于随机访问迭代器。
     * 
     * @tparam RandomAccessIterator 随机访问迭代器类型。
     * @param first 起始迭代器。
     * @param last 结束迭代器。
     * @param tag 迭代器标签，用于标记迭代器的类别。
     * @return 两个迭代器之间的距离，类型为 `iterator_traits<RandomAccessIterator>::difference_type`。
     */
    template <typename RandomAccessIterator>
    auto distance_aux(
        RandomAccessIterator first,
        RandomAccessIterator last,
        random_access_iterator_tag
    )
    -> typename iterator_traits<RandomAccessIterator>::difference_type {
        return last - first;
    }

    /**
     * @brief 计算两个迭代器之间的距离。
     * 
     * 该函数根据迭代器的类别调用相应的辅助函数来计算距离。
     * 
     * @tparam InputIterator 迭代器类型。
     * @param first 起始迭代器。
     * @param last 结束迭代器。
     * @return 两个迭代器之间的距离，类型为 `iterator_traits<InputIterator>::difference_type`。
     */
    template <typename InputIterator>
    auto distance(
        InputIterator first,
        InputIterator last
    ) 
        -> typename iterator_traits<InputIterator>::difference_type {
        typename iterator_traits<InputIterator>::iterator_category category;
        return distance_aux(first, last, category);
    }

    /**
     * @brief 将输入迭代器向前移动指定的距离的辅助函数。
     * 
     * 该函数通过逐个递增迭代器来移动，适用于输入迭代器。
     * 
     * @tparam InputIterator 输入迭代器类型。
     * @tparam Distance 距离类型。
     * @param it 要移动的迭代器。
     * @param n 要移动的距离。
     * @param tag 迭代器标签，用于标记迭代器的类别。
     */
    template <typename InputIterator, typename Distance>
    void advance_aux(
        InputIterator it,
        Distance n,
        input_iterator_tag
    ) {
        while (n-- > 0) {
            ++it;
        }
    }

    /**
     * @brief 将双向迭代器向前或向后移动指定的距离的辅助函数。
     * 
     * 该函数根据距离的正负来决定是向前还是向后移动迭代器，适用于双向迭代器。
     * 
     * @tparam BidirectionalIterator 双向迭代器类型。
     * @tparam Distance 距离类型。
     * @param it 要移动的迭代器。
     * @param n 要移动的距离。
     * @param tag 迭代器标签，用于标记迭代器的类别。
     */
    template <class BidirectionalIterator, typename Distance>
    void advance_aux(
        BidirectionalIterator it,
        Distance n,
        bidirectional_iterator_tag
    ) {
        if (n >= 0) {
            while (n-- > 0) {
                ++it;
            }
        } else {
            while (n++ < 0) {
                --it;
            }
        }
    }

    /**
     * @brief 将随机访问迭代器向前或向后移动指定的距离的辅助函数。
     * 
     * 该函数通过直接加上或减去距离来移动迭代器，适用于随机访问迭代器。
     * 
     * @tparam RandomAccessIterator 随机访问迭代器类型。
     * @tparam Distance 距离类型。
     * @param it 要移动的迭代器。
     * @param n 要移动的距离。
     * @param tag 迭代器标签，用于标记迭代器的类别。
     */
    template <class RandomAccessIterator, typename Distance>
    void advance_aux(
        RandomAccessIterator it,
        Distance n,
        random_access_iterator_tag
    ) {
        it += n;
    }

    /**
     * @brief 将迭代器向前或向后移动指定的距离。
     * 
     * 该函数根据迭代器的类别调用相应的辅助函数来移动迭代器。
     * 
     * @tparam InputIterator 迭代器类型。
     * @tparam Distance 距离类型。
     * @param it 要移动的迭代器。
     * @param n 要移动的距离。
     */
    template <typename InputIterator, typename Distance>
    void advance(
        InputIterator it,
        Distance n
    ) {
        typename iterator_traits<InputIterator>::iterator_category category;
        advance_aux(it, n, category);
    }

    /**
     * @class reverse_iterator
     * @brief 反向迭代器适配器，用于将双向迭代器或随机访问迭代器的方向反转。
     * 
     * 反向迭代器可以用于以相反的顺序遍历容器。它封装了一个正向迭代器，并提供了反向的操作接口。
     * 
     * @tparam Iterator 底层迭代器类型，必须是双向迭代器或随机访问迭代器。
     */
    template <typename Iterator>
    class reverse_iterator {
    public:
        /**
         * @brief 底层迭代器类型。
         */
        using iterator_type = Iterator;
        /**
         * @brief 反向迭代器的类别，与底层迭代器的类别相同。
         */
        using iterator_category = typename iterator_traits<Iterator>::iterator_category;
        /**
         * @brief 反向迭代器指向元素的值类型，与底层迭代器相同。
         */
        using value_type = typename iterator_traits<Iterator>::value_type;
        /**
         * @brief 反向迭代器的差值类型，与底层迭代器相同。
         */
        using difference_type = typename iterator_traits<Iterator>::difference_type;
        /**
         * @brief 反向迭代器指向元素的指针类型，与底层迭代器相同。
         */
        using pointer = typename iterator_traits<Iterator>::pointer;
        /**
         * @brief 反向迭代器指向元素的引用类型，与底层迭代器相同。
         */
        using reference = typename iterator_traits<Iterator>::reference;

        /**
         * @brief 默认构造函数，初始化底层迭代器为默认值。
         */
        reverse_iterator() : current() {}

        /**
         * @brief 从正向迭代器构造反向迭代器。
         * 
         * @param x 正向迭代器。
         */
        explicit reverse_iterator(Iterator x) : current(x) {}

        /**
         * @brief 从其他反向迭代器构造反向迭代器。
         * 
         * @tparam U 其他反向迭代器的底层迭代器类型。
         * @param other 其他反向迭代器。
         */
        template <typename U>
        reverse_iterator(const reverse_iterator<U>& other) 
            : current(other.base()) {}

        /**
         * @brief 获取底层迭代器。
         * 
         * @return 底层迭代器。
         */
        Iterator base() const { return current; }

        /**
         * @brief 解引用操作符，返回反向迭代器指向的元素的引用。
         * 
         * @return 反向迭代器指向的元素的引用。
         */
        reference operator*() const {
            Iterator tmp = current;
            return *--tmp;
        }

        /**
         * @brief 箭头操作符，返回反向迭代器指向的元素的指针。
         * 
         * @return 反向迭代器指向的元素的指针。
         */
        pointer operator->() const {
            return &(operator*());
        }

        /**
         * @brief 前置递增操作符，将反向迭代器向前移动一个位置。
         * 
         * @return 递增后的反向迭代器的引用。
         */
        reverse_iterator& operator++() {
            --current;
            return *this;
        }

        /**
         * @brief 后置递增操作符，将反向迭代器向前移动一个位置。
         * 
         * @param 占位参数，用于区分前置和后置递增。
         * @return 递增前的反向迭代器的副本。
         */
        reverse_iterator operator++(int) {
            reverse_iterator tmp(*this);
            --current;
            return tmp;
        }

        /**
         * @brief 前置递减操作符，将反向迭代器向后移动一个位置。
         * 
         * @return 递减后的反向迭代器的引用。
         */
        reverse_iterator& operator--() {
            ++current;
            return *this;
        }

        /**
         * @brief 后置递减操作符，将反向迭代器向后移动一个位置。
         * 
         * @param 占位参数，用于区分前置和后置递减。
         * @return 递减前的反向迭代器的副本。
         */
        reverse_iterator operator--(int) {
            reverse_iterator tmp(*this);
            ++current;
            return tmp;
        }

        /**
         * @brief 随机访问加法操作符，将反向迭代器向前移动指定的距离。
         * 
         * @param n 要移动的距离。
         * @return 移动后的反向迭代器的副本。
         */
        reverse_iterator operator+(difference_type n) const {
            return reverse_iterator(current - n);
        }

        /**
         * @brief 随机访问复合加法操作符，将反向迭代器向前移动指定的距离。
         * 
         * @param n 要移动的距离。
         * @return 移动后的反向迭代器的引用。
         */
        reverse_iterator& operator+=(difference_type n) {
            current -= n;
            return *this;
        }

        /**
         * @brief 随机访问减法操作符，将反向迭代器向后移动指定的距离。
         * 
         * @param n 要移动的距离。
         * @return 移动后的反向迭代器的副本。
         */
        reverse_iterator operator-(difference_type n) const {
            return reverse_iterator(current + n);
        }

        /**
         * @brief 随机访问复合减法操作符，将反向迭代器向后移动指定的距离。
         * 
         * @param n 要移动的距离。
         * @return 移动后的反向迭代器的引用。
         */
        reverse_iterator& operator-=(difference_type n) {
            current += n;
            return *this;
        }

        /**
         * @brief 随机访问下标操作符，返回反向迭代器偏移指定距离后的元素的引用。
         * 
         * @param n 偏移的距离。
         * @return 偏移后的元素的引用。
         */
        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        /**
         * @brief 相等比较操作符，比较两个反向迭代器是否相等。
         * 
         * @tparam It 另一个反向迭代器的底层迭代器类型。
         * @param other 另一个反向迭代器。
         * @return 如果两个反向迭代器相等，返回 `true`；否则返回 `false`。
         */
        template <typename It>
        bool operator==(const reverse_iterator<It>& other) const {
            return current == other.base();
        }

        /**
         * @brief 不等比较操作符，比较两个反向迭代器是否不相等。
         * 
         * @tparam It 另一个反向迭代器的底层迭代器类型。
         * @param other 另一个反向迭代器。
         * @return 如果两个反向迭代器不相等，返回 `true`；否则返回 `false`。
         */
        template <typename It>
        bool operator!=(const reverse_iterator<It>& other) const {
            return !(*this == other);
        }

        /**
         * @brief 小于比较操作符，比较两个反向迭代器的顺序。
         * 
         * @tparam It 另一个反向迭代器的底层迭代器类型。
         * @param other 另一个反向迭代器。
         * @return 如果当前反向迭代器小于另一个反向迭代器，返回 `true`；否则返回 `false`。
         */
        template <typename It>
        bool operator<(const reverse_iterator<It>& other) const {
            return current > other.base();
        }

        /**
         * @brief 小于等于比较操作符，比较两个反向迭代器的顺序。
         * 
         * @tparam It 另一个反向迭代器的底层迭代器类型。
         * @param other 另一个反向迭代器。
         * @return 如果当前反向迭代器小于等于另一个反向迭代器，返回 `true`；否则返回 `false`。
         */
        template <typename It>
        bool operator<=(const reverse_iterator<It>& other) const {
            return !(other < *this);
        }

        /**
         * @brief 大于比较操作符，比较两个反向迭代器的顺序。
         * 
         * @tparam It 另一个反向迭代器的底层迭代器类型。
         * @param other 另一个反向迭代器。
         * @return 如果当前反向迭代器大于另一个反向迭代器，返回 `true`；否则返回 `false`。
         */
        template <typename It>
        bool operator>(const reverse_iterator<It>& other) const {
            return other < *this;
        }

        /**
         * @brief 大于等于比较操作符，比较两个反向迭代器的顺序。
         * 
         * @tparam It 另一个反向迭代器的底层迭代器类型。
         * @param other 另一个反向迭代器。
         * @return 如果当前反向迭代器大于等于另一个反向迭代器，返回 `true`；否则返回 `false`。
         */
        template <typename It>
        bool operator>=(const reverse_iterator<It>& other) const {
            return !(*this < other);
        }

    private:
        Iterator current; // 底层迭代器
    };

    /**
     * @brief 非成员函数，用于将一个整数与反向迭代器相加。
     * 
     * @tparam Iterator 反向迭代器的底层迭代器类型。
     * @param n 要相加的整数。
     * @param it 反向迭代器。
     * @return 相加后的反向迭代器的副本。
     */
    template <typename Iterator>
    reverse_iterator<Iterator> operator+(
        typename reverse_iterator<Iterator>::difference_type n,
        const reverse_iterator<Iterator>& it) {
        return it + n;
    }

    /**
     * @brief 非成员函数，用于计算两个反向迭代器之间的距离。
     * 
     * @tparam Iterator1 第一个反向迭代器的底层迭代器类型。
     * @tparam Iterator2 第二个反向迭代器的底层迭代器类型。
     * @param lhs 第一个反向迭代器。
     * @param rhs 第二个反向迭代器。
     * @return 两个反向迭代器之间的距离，类型为 `decltype(rhs.base() - lhs.base())`。
     */
    template <typename Iterator1, typename Iterator2>
    auto operator-(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs)
        -> decltype(rhs.base() - lhs.base()) {
        return rhs.base() - lhs.base();
    }

    /**
     * @brief 非成员函数，用于比较两个反向迭代器是否相等。
     * 
     * @tparam Iterator1 第一个反向迭代器的底层迭代器类型。
     * @tparam Iterator2 第二个反向迭代器的底层迭代器类型。
     * @param lhs 第一个反向迭代器。
     * @param rhs 第二个反向迭代器。
     * @return 如果两个反向迭代器相等，返回 `true`；否则返回 `false`。
     */
    template <typename Iterator1, typename Iterator2>
    bool operator==(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return lhs.base() == rhs.base();
    }

    /**
     * @brief 非成员函数，用于比较两个反向迭代器是否不相等。
     * 
     * @tparam Iterator1 第一个反向迭代器的底层迭代器类型。
     * @tparam Iterator2 第二个反向迭代器的底层迭代器类型。
     * @param lhs 第一个反向迭代器。
     * @param rhs 第二个反向迭代器。
     * @return 如果两个反向迭代器不相等，返回 `true`；否则返回 `false`。
     */
    template <typename Iterator1, typename Iterator2>
    bool operator!=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief 非成员函数，用于比较两个反向迭代器的顺序。
     * 
     * @tparam Iterator1 第一个反向迭代器的底层迭代器类型。
     * @tparam Iterator2 第二个反向迭代器的底层迭代器类型。
     * @param lhs 第一个反向迭代器。
     * @param rhs 第二个反向迭代器。
     * @return 如果第一个反向迭代器小于第二个反向迭代器，返回 `true`；否则返回 `false`。
     */
    template <typename Iterator1, typename Iterator2>
    bool operator<(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return lhs.base() > rhs.base();
    }

    /**
     * @brief 非成员函数，用于比较两个反向迭代器的顺序。
     * 
     * @tparam Iterator1 第一个反向迭代器的底层迭代器类型。
     * @tparam Iterator2 第二个反向迭代器的底层迭代器类型。
     * @param lhs 第一个反向迭代器。
     * @param rhs 第二个反向迭代器。
     * @return 如果第一个反向迭代器小于等于第二个反向迭代器，返回 `true`；否则返回 `false`。
     */
    template <typename Iterator1, typename Iterator2>
    bool operator<=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return !(rhs < lhs);
    }

    /**
     * @brief 非成员函数，用于比较两个反向迭代器的顺序。
     * 
     * @tparam Iterator1 第一个反向迭代器的底层迭代器类型。
     * @tparam Iterator2 第二个反向迭代器的底层迭代器类型。
     * @param lhs 第一个反向迭代器。
     * @param rhs 第二个反向迭代器。
     * @return 如果第一个反向迭代器大于第二个反向迭代器，返回 `true`；否则返回 `false`。
     */
    template <typename Iterator1, typename Iterator2>
    bool operator>(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return rhs < lhs;
    }

    /**
     * @brief 非成员函数，用于比较两个反向迭代器的顺序。
     * 
     * @tparam Iterator1 第一个反向迭代器的底层迭代器类型。
     * @tparam Iterator2 第二个反向迭代器的底层迭代器类型。
     * @param lhs 第一个反向迭代器。
     * @param rhs 第二个反向迭代器。
     * @return 如果第一个反向迭代器大于等于第二个反向迭代器，返回 `true`；否则返回 `false`。
     */
    template <typename Iterator1, typename Iterator2>
    bool operator>=(const reverse_iterator<Iterator1>& lhs,
                const reverse_iterator<Iterator2>& rhs) {
        return !(lhs < rhs);
    }

} // namespace tiny_stl
