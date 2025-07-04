/**
 * @file list.hpp
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
    template <typename T>
    struct __list_node {
        using self = __list_node<T>;
        self* prev;
        self* next;
        T value;
    };
    template <typename T, typename Ref, typename Ptr>
    struct __list_iterator {
        using iterator = __list_iterator<T, T&, T*>;
        using const_iterator = __list_iterator<T, const T&, const T*>;
        using self = __list_iterator<T, Ref, Ptr>;

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = Ptr;
        using reference = Ref;

        __list_node<T>* node;

        __list_iterator(__list_node<T>* x) : node(x) {}
        __list_iterator() : node(nullptr) {}
        __list_iterator(const iterator& x) : node(x.node) {}

        bool operator==(const self& x) const { return node == x.node; }
        bool operator!=(const self& x) const { return node != x.node; }

        reference operator*() const { return node->value; }
        pointer operator->() const { return &(operator*()); }

        self& operator++() {
            node = node->next;
            return *this;
        }
        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }
        self& operator--() {
            node = node->prev;
            return *this;
        }
        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }
    };

    // 链表类
    template <typename T, typename Alloc = allocator<__list_node<T>>>
    class list {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using iterator = __list_iterator<T, T&, T*>;
        using const_iterator = __list_iterator<T, const T&, const T*>;
    protected:
        using node_allocator = Alloc;
        using node_type = __list_node<T>;
        using node_pointer = node_type*;

        node_pointer node;  // 指向一个空白节点，作为链表的头和尾
        node_allocator alloc;

        // 分配一个新节点
        node_pointer get_node() { return alloc.allocate(1); }
        // 释放一个节点
        void put_node(node_pointer p) { alloc.deallocate(p, 1); }
        // 构造一个新节点
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
        // 将[first, last)内的所有元素移动到pos之前
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
        // 销毁一个节点
        void destroy_node(node_pointer p) {
            alloc.destroy(p);
            put_node(p);
        }

        // 在某个节点之后插入一个新节点
        void insert_aux(node_pointer pos, const T& x) {
            node_pointer new_node = create_node(x);
            new_node->next = pos;
            new_node->prev = pos->prev;
            pos->prev->next = new_node;
            pos->prev = new_node;
        }

    public:
        // 构造函数
        list() { empty_initialize(); }

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
        list(std::initializer_list<T> l) {
            *this = list(l.begin(), l.end());
        }

        // 析构函数
        ~list() {
            clear();
            destroy_node(node);
        }

        // 清空链表
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

        // 初始化一个空链表
        void empty_initialize() {
            node = get_node();
            node->next = node;
            node->prev = node;
        }

        // 返回链表的起始迭代器
        iterator begin() { return node->next; }
        const_iterator begin() const { return node->next; }
        // 返回链表的结束迭代器
        iterator end() { return node; }
        const_iterator end() const { return node; }

        // 判断链表是否为空
        bool empty() const { return node->next == node; }
        // 返回链表的大小
        size_type size() const {
            size_type result = 0;
            const_iterator it = begin();
            while (it != end()) {
                ++it;
                ++result;
            }
            return result;
        }

        // 返回链表的第一个元素
        reference front() { return *begin(); }
        const_reference front() const { return *begin(); }
        // 返回链表的最后一个元素
        reference back() { return *(--end()); }
        const_reference back() const { return *(--end()); }

        // 在链表尾部插入一个元素
        void push_back(const T& x) { insert_aux(node, x); }
        // 在链表头部插入一个元素
        void push_front(const T& x) { insert_aux(node->next, x); }

        // 删除链表的第一个元素
        void pop_front() { erase(begin()); }
        // 删除链表的最后一个元素
        void pop_back() {
            iterator tmp = end();
            erase(--tmp);
        }

        // 在指定位置插入一个元素
        iterator insert(iterator position, const T& x) {
            insert_aux(position.node, x);
            return --position;
        }

        // 删除指定位置的元素
        iterator erase(iterator position) {
            node_pointer next_node = position.node->next;
            node_pointer prev_node = position.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(position.node);
            return next_node;
        }
        // 将链表x的所有元素移动到pos之前，x变为空
        void splice(iterator pos, list& x) {
            if (!x.empty()) {
                transfer(pos, x.begin(), x.end());
            }
        }

        // 将迭代器i指向的元素移动到pos之前
        void splice(iterator pos, iterator i) {
            iterator j = i;
            ++j;
            if (pos == i || pos == j) return;
            transfer(pos, i, j);
        }

        // 将[first, last)区间的元素移动到pos之前
        void splice(iterator pos, iterator first, iterator last) {
            if (first != last) {
                transfer(pos, first, last);
            }
        }

        // 合并两个有序链表（默认升序）
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

        // 反转链表
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
