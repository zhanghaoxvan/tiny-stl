/**
 * @file list.hpp
 * @brief 本文件定义了一个轻量级的双向链表类 `tiny_stl::list`。
 *
 * 该链表类提供了基本的链表操作，如插入、删除、合并、反转和排序等功能。
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include <memory.hpp>
#include <iterator.hpp>
#include <functional.hpp>
#include <initializer_list>

namespace tiny_stl {
    /**
     * @brief 链表节点结构体。
     *
     * 该结构体表示双向链表中的一个节点，包含指向前一个节点和后一个节点的指针，以及节点存储的值。
     * @tparam T 节点存储的值的类型。
     */
    template <typename T>
    struct __list_node {
        using self = __list_node<T>; /**< 节点自身类型的别名。 */
        self* prev; /**< 指向前一个节点的指针。 */
        self* next; /**< 指向后一个节点的指针。 */
        T value; /**< 节点存储的值。 */
    };

    /**
     * @brief 链表迭代器结构体。
     *
     * 该结构体实现了双向链表的迭代器功能，支持双向遍历链表。
     * @tparam T 节点存储的值的类型。
     * @tparam Ref 引用类型。
     * @tparam Ptr 指针类型。
     */
    template <typename T, typename Ref, typename Ptr>
    struct __list_iterator  {
        using iterator = __list_iterator<T, T&, T*>; /**< 迭代器类型的别名。 */
        using const_iterator = __list_iterator<T, const T&, const T*>; /**< 常量迭代器类型的别名。 */
        using self = __list_iterator<T, Ref, Ptr>; /**< 迭代器自身类型的别名。 */

        using iterator_category = std::bidirectional_iterator_tag; /**< 迭代器的类别。 */
        using value_type = T; /**< 迭代器指向元素的值类型。 */
        using difference_type = std::ptrdiff_t; /**< 迭代器的差值类型，用于表示两个迭代器之间的距离。 */
        using pointer = Ptr; /**< 迭代器指向元素的指针类型。 */
        using reference = Ref; /**< 迭代器指向元素的引用类型。 */

        __list_node<T>* node; /**< 指向链表节点的指针。 */

        /**
         * @brief 构造函数，用一个节点指针初始化迭代器。
         * @param x 指向链表节点的指针。
         */
        __list_iterator(__list_node<T>* x) : node(x) {}

        /**
         * @brief 默认构造函数，将迭代器初始化为空。
         */
        __list_iterator() : node(nullptr) {}

        /**
         * @brief 拷贝构造函数，用另一个迭代器初始化当前迭代器。
         * @param x 另一个迭代器。
         */
        __list_iterator(const iterator& x) : node(x.node) {}

        /**
         * @brief 比较两个迭代器是否相等。
         * @param x 另一个迭代器。
         * @return 如果两个迭代器指向同一个节点，返回 `true`，否则返回 `false`。
         */
        bool operator==(const self& x) const { return node == x.node; }

        /**
         * @brief 比较两个迭代器是否不相等。
         * @param x 另一个迭代器。
         * @return 如果两个迭代器指向不同的节点，返回 `true`，否则返回 `false`。
         */
        bool operator!=(const self& x) const { return node != x.node; }

        /**
         * @brief 解引用操作符，返回迭代器指向的节点的值的引用。
         * @return 迭代器指向的节点的值的引用。
         */
        reference operator*() const { return node->value; }

        /**
         * @brief 箭头操作符，返回迭代器指向的节点的值的指针。
         * @return 迭代器指向的节点的值的指针。
         */
        pointer operator->() const { return &(operator*()); }

        /**
         * @brief 前置递增操作符，将迭代器指向下一个节点。
         * @return 递增后的迭代器的引用。
         */
        self& operator++() {
            node = node->next;
            return *this;
        }

        /**
         * @brief 后置递增操作符，将迭代器指向下一个节点。
         * @param 占位参数，用于区分前置和后置递增。
         * @return 递增前的迭代器的副本。
         */
        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        /**
         * @brief 前置递减操作符，将迭代器指向前一个节点。
         * @return 递减后的迭代器的引用。
         */
        self& operator--() {
            node = node->prev;
            return *this;
        }

        /**
         * @brief 后置递减操作符，将迭代器指向前一个节点。
         * @param 占位参数，用于区分前置和后置递减。
         * @return 递减前的迭代器的副本。
         */
        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }
    };

    /**
     * @brief 双向链表类。
     *
     * 该类实现了一个双向链表，提供了基本的链表操作，如插入、删除、合并、反转和排序等功能。
     * @tparam T 链表节点存储的值的类型。
     * @tparam Alloc 节点分配器类型，默认为 `allocator<__list_node<T>>`。
     */
    template <typename T, typename Alloc = allocator<__list_node<T>>>
    class list {
    public:
        using value_type = T; /**< 链表节点存储的值的类型。 */
        using pointer = T*; /**< 指针类型。 */
        using const_pointer = const T*; /**< 常量指针类型。 */
        using reference = T&; /**< 引用类型。 */
        using const_reference = const T&; /**< 常量引用类型。 */
        using size_type = size_t; /**< 大小类型。 */
        using difference_type = std::ptrdiff_t; /**< 差值类型。 */
        using iterator = __list_iterator<T, T&, T*>; /**< 迭代器类型。 */
        using const_iterator = __list_iterator<T, const T&, const T*>; /**< 常量迭代器类型。 */
    protected:
        using node_allocator = Alloc; /**< 节点分配器类型。 */
        using node_type = __list_node<T>; /**< 节点类型。 */
        using node_pointer = node_type*; /**< 节点指针类型。 */

        node_pointer node;  /**< 指向一个空白节点，作为链表的头和尾。 */
        node_allocator alloc; /**< 节点分配器对象。 */

        /**
         * @brief 分配一个新节点。
         * @return 指向新分配节点的指针。
         */
        node_pointer get_node() { return alloc.allocate(1); }

        /**
         * @brief 释放一个节点。
         * @param p 指向要释放节点的指针。
         */
        void put_node(node_pointer p) { alloc.deallocate(p, 1); }

        /**
         * @brief 构造一个新节点。
         * @param x 节点存储的值。
         * @return 指向新构造节点的指针。
         */
        node_pointer create_node(const T& x) {
            node_pointer p = get_node();
            try {
                alloc.construct(p, node_type{nullptr, nullptr, x});
            } catch (...) {
                put_node(p);
                throw;
            }
            return p;
        }

        /**
         * @brief 将[first, last)内的所有元素移动到pos之前。
         * @param pos 目标位置的迭代器。
         * @param first 移动范围的起始迭代器。
         * @param last 移动范围的结束迭代器。
         */
        void transfer(iterator pos, iterator first, iterator last) {
            if (pos == last || first == last) return; // 无需移动的情况
            
            node_pointer f = first.node;
            node_pointer l = last.node->prev; // 移动区域的最后一个节点
            
            // 从原位置断开 [first, last)
            f->prev->next = l->next;
            l->next->prev = f->prev;
            
            // 插入到pos之前
            node_pointer p = pos.node;
            l->next = p;
            f->prev = p->prev;
            p->prev->next = f;
            p->prev = l;
        }

        /**
         * @brief 销毁一个节点。
         * @param p 指向要销毁节点的指针。
         */
        void destroy_node(node_pointer p) {
            alloc.destroy(p);
            put_node(p);
        }

        /**
         * @brief 在某个节点之后插入一个新节点。
         * @param pos 插入位置的节点指针。
         * @param x 新节点存储的值。
         */
        void insert_aux(node_pointer pos, const T& x) {
            node_pointer new_node = create_node(x);
            new_node->next = pos;
            new_node->prev = pos->prev;
            pos->prev->next = new_node;
            pos->prev = new_node;
        }

    public:
        /**
         * @brief 默认构造函数，初始化一个空链表。
         */
        list() { empty_initialize(); }

        /**
         * @brief 构造函数，创建一个包含n个值为v的节点的链表。
         * @param n 节点数量。
         * @param v 节点存储的值，默认为值类型的默认构造值。
         */
        list(size_type n, const value_type& v = value_type{}) {
            empty_initialize(); // 初始化空链表结构
            
            if (n == 0) {
                return;
            }
            
            node_pointer current = node; // 从哨兵节点开始
            
            try {
                // 创建n个节点并连接
                for (size_type i = 0; i < n; ++i) {
                    node_pointer new_node = create_node(v);
                    
                    // 插入到current之后
                    new_node->prev = current;
                    new_node->next = current->next;
                    current->next->prev = new_node;
                    current->next = new_node;
                    
                    current = new_node; // 移动到新节点
                }
            } catch (...) {
                clear(); // 异常发生时清理已分配的节点
                throw;
            }
        }

        /**
         * @brief 构造函数，使用输入迭代器范围初始化链表。
         * @tparam InputIterator 输入迭代器类型。
         * @param first 输入范围的起始迭代器。
         * @param last 输入范围的结束迭代器。
         */
        template <class InputIterator>
        list(InputIterator first, InputIterator last) {
            empty_initialize(); // 初始化空链表结构
            
            if (first == last) {
                return;
            }
            
            node_pointer current = node; // 从哨兵节点开始
            
            try {
                for (; first != last; first++) {
                    node_pointer new_node = create_node(*first);
                    
                    // 插入到current之后
                    new_node->prev = current;
                    new_node->next = current->next;
                    current->next->prev = new_node;
                    current->next = new_node;
                    
                    current = new_node; // 移动到新节点
                }
            } catch (...) {
                clear(); // 异常发生时清理已分配的节点
                throw;
            }
        }

        /**
         * @brief 构造函数，使用初始化列表初始化链表。
         * @param l 初始化列表。
         */
        list(std::initializer_list<T> l) {
            *this = list(l.begin(), l.end());
        }

        /**
         * @brief 析构函数，销毁链表。
         */
        ~list() {
            clear();
            destroy_node(node);
        }

        /**
         * @brief 清空链表。
         */
        void clear() {
            node_pointer cur = node->next;
            while (cur != node) {
                node_pointer tmp = cur;
                cur = cur->next;
                destroy_node(tmp);
            }
            node->next = node;
            node->prev = node;
        }

        /**
         * @brief 初始化一个空链表。
         */
        void empty_initialize() {
            node = get_node();
            node->next = node;
            node->prev = node;
        }

        /**
         * @brief 返回链表的起始迭代器。
         * @return 指向链表第一个节点的迭代器。
         */
        iterator begin() { return node->next; }

        /**
         * @brief 返回链表的起始常量迭代器。
         * @return 指向链表第一个节点的常量迭代器。
         */
        const_iterator begin() const { return node->next; }

        /**
         * @brief 返回链表的结束迭代器。
         * @return 指向链表尾部哨兵节点的迭代器。
         */
        iterator end() { return node; }

        /**
         * @brief 返回链表的结束常量迭代器。
         * @return 指向链表尾部哨兵节点的常量迭代器。
         */
        const_iterator end() const { return node; }

        /**
         * @brief 判断链表是否为空。
         * @return 如果链表为空，返回 `true`，否则返回 `false`。
         */
        bool empty() const { return node->next == node; }

        /**
         * @brief 返回链表的大小。
         * @return 链表中节点的数量。
         */
        size_type size() const {
            size_type result = 0;
            const_iterator it = begin();
            while (it != end()) {
                ++it;
                ++result;
            }
            return result;
        }

        /**
         * @brief 返回链表的第一个元素的引用。
         * @return 链表第一个元素的引用。
         */
        reference front() { return *begin(); }

        /**
         * @brief 返回链表的第一个元素的常量引用。
         * @return 链表第一个元素的常量引用。
         */
        const_reference front() const { return *begin(); }

        /**
         * @brief 返回链表的最后一个元素的引用。
         * @return 链表最后一个元素的引用。
         */
        reference back() { return *(--end()); }

        /**
         * @brief 返回链表的最后一个元素的常量引用。
         * @return 链表最后一个元素的常量引用。
         */
        const_reference back() const { return *(--end()); }

        /**
         * @brief 在链表尾部插入一个元素。
         * @param x 要插入的元素的值。
         */
        void push_back(const T& x) { insert_aux(node, x); }

        /**
         * @brief 在链表头部插入一个元素。
         * @param x 要插入的元素的值。
         */
        void push_front(const T& x) { insert_aux(node->next, x); }

        /**
         * @brief 删除链表的第一个元素。
         */
        void pop_front() { erase(begin()); }

        /**
         * @brief 删除链表的最后一个元素。
         */
        void pop_back() {
            iterator tmp = end();
            erase(--tmp);
        }

        /**
         * @brief 在指定位置插入一个元素。
         * @param position 插入位置的迭代器。
         * @param x 要插入的元素的值。
         * @return 指向插入元素的迭代器。
         */
        iterator insert(iterator position, const T& x) {
            insert_aux(position.node, x);
            return --position;
        }

        /**
         * @brief 删除指定位置的元素。
         * @param position 要删除元素的迭代器。
         * @return 指向被删除元素下一个元素的迭代器。
         */
        iterator erase(iterator position) {
            node_pointer next_node = position.node->next;
            node_pointer prev_node = position.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(position.node);
            return next_node;
        }

        /**
         * @brief 将链表x的所有元素移动到pos之前，x变为空。
         * @param pos 目标位置的迭代器。
         * @param x 要移动元素的链表。
         */
        void splice(iterator pos, list& x) {
            if (!x.empty()) {
                transfer(pos, x.begin(), x.end());
            }
        }

        /**
         * @brief 将迭代器i指向的元素移动到pos之前。
         * @param pos 目标位置的迭代器。
         * @param i 要移动元素的迭代器。
         */
        void splice(iterator pos, iterator i) {
            iterator j = i;
            ++j;
            if (pos == i || pos == j) return;
            transfer(pos, i, j);
        }

        /**
         * @brief 将[first, last)区间的元素移动到pos之前。
         * @param pos 目标位置的迭代器。
         * @param first 移动范围的起始迭代器。
         * @param last 移动范围的结束迭代器。
         */
        void splice(iterator pos, iterator first, iterator last) {
            if (first != last) {
                transfer(pos, first, last);
            }
        }

        /**
         * @brief 合并两个有序链表（默认升序）。
         * @tparam Pred 比较函数类型。
         * @param x 要合并的链表。
         * @param comp 比较函数，默认为 `less<T>()`。
         */
        template <typename Pred>
        void merge(list& x, Pred comp = less<T>()) {
            iterator first1 = begin();
            iterator last1 = end();
            iterator first2 = x.begin();
            iterator last2 = x.end();
            
            while (first1 != last1 && first2 != last2) {
                if (comp(*first2, *first1)) {
                    iterator next = first2;
                    ++next;
                    transfer(first1, first2, next);
                    first2 = next;
                } else {
                    ++first1;
                }
            }
            
            // 将x中剩余元素全部移到末尾
            if (first2 != last2) {
                transfer(last1, first2, last2);
            }
            
            // 清空x
            x.clear();
        }

        /**
         * @brief 反转链表。
         */
        void reverse() {
            if (node->next == node || node->next->next == node) {
                return;
            }
            
            iterator first = begin();
            ++first;
            
            while (first != end()) {
                iterator old = first;
                ++first;
                transfer(begin(), old, first);
            }
        }

        /**
         * @brief 对链表进行排序。
         * @tparam Pred 比较函数类型。
         * @param comp 比较函数，默认为 `less<T>()`。
         */
        template <typename Pred>
        void sort(Pred comp = less<T>()) {
            // 空链表或只有一个元素时无需排序
            if (node->next == node || node->next->next == node) return;

            list<T> carry;
            list<T> counter[64]; // 用于归并的计数器数组
            int fill = 0;        // 当前使用的计数器位置

            while (!empty()) {
                // 将当前链表的第一个元素移到carry中
                carry.splice(carry.begin(), *this, begin());

                int i = 0;
                while (i < fill && !counter[i].empty()) {
                    // 合并carry和counter[i]，结果存入counter[i]
                    counter[i].merge(carry, comp);
                    // 将合并后的结果移到carry
                    carry.swap(counter[i]);
                    ++i;
                }
                // 将carry存入计数器数组
                carry.swap(counter[i]);
                if (i == fill) ++fill;
            }

            // 合并所有计数器中的链表
            for (int i = 1; i < fill; ++i) {
                counter[i].merge(counter[i - 1], comp);
            }
            // 将排序好的链表移回本链表
            swap(counter[fill - 1]);
        }
    };
}
