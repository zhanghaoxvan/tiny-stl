/**
 * @file deque.hpp
 * @brief 此文件定义了一个双端队列，模仿STL库中的deque
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include <cstddef>
#include <algorithm> // 添加此头文件用于 std::copy 和 std::copy_backward
                     // 稍后自己实现并替换
// #include <algorithm.hpp>
#include <iterator>
#include <memory> 

namespace tiny_stl {
    /**
     * @class __deque_iterator
     * @brief deque的迭代器
     * @tparam T 迭代器的元素类型
     * @tparam buffer 缓冲区大小
     * @tparam Ptr 指针，默认为T*
     * @tparam Ref 引用，默认为T&
     */
    template <typename T, size_t buffer, typename Ptr = T*, typename Ref = T&>
    struct __deque_iterator {
    public:
        using iterator = __deque_iterator<T, buffer, T*, T&>; /**< 迭代器 */
        using const_iterator =  __deque_iterator<T, buffer, const T*, const T&>; /**< 常量迭代器 */
        using size_type = size_t; /**< 大小的类型 */
        using map_pointer = T**; /**< 数据指针，指向管控中心 */
        using self = __deque_iterator; /**< deque_iterator自身 */

        using value_type = T; /**< 元素类型 */
        using iterator_category = std::random_access_iterator_tag; /**< 迭代器类型 */
        using difference_type = ptrdiff_t; /**< 距离类型 */
        using pointer = Ptr; /**< 指针类型 */
        using reference = Ref; /**< 引用类型 */
    public:
        T* cur; /**< 当前位置 */
        T* first; /**< 缓冲区的第一个 */
        T* last; /**< 缓冲区的最后一个 */
        map_pointer node; /**< 管控中心 */
    public:
        /**
         * @brief 获取缓冲区大小
         * @return 缓冲区大小
         */
        size_type buffer_size() const {
            return buffer;
        }

        /**
         * @brief 设置管控中心节点
         * @param new_node 新的管控中心节点指针
         */
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }
    public:
        /**
         * @brief 构造函数，根据管控中心指针和当前位置构造
         * @param x 当前位置
         * @param y 管控中心指针
         */
        __deque_iterator(T* x, map_pointer y) 
            : cur(x), first(*y), last(*y + buffer_size()), node(y) { }

        /**
         * @brief 默认构造函数
         */
        __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) { }

        /**
         * @brief 复制构造函数
         * @param x 要复制的迭代器
         */
        __deque_iterator(const iterator& x)
            : cur(x.cur), first(x.first), last(x.last), node(x.node) { }
    public:
        /**
         * @brief 访问当前指向的元素的*运算符
         * @return 当前指向的元素
         */
        reference operator*() { return *cur; }

        /**
         * @brief 访问当前指向的元素的->运算符
         * @return 当前指向的元素
         */
        pointer operator->() const { return cur; }

        /**
         * @brief 返回*this与x之间的距离
         * @param x 另一个迭代器
         * @return 两个迭代器之间的距离
         */
        difference_type operator-(const self& x) const {
            return difference_type(buffer_size()) * (node - x.node - 1) +
                (cur - first) + (x.last - x.cur);
        }

        /**
         * @brief 使迭代器向后移动一位
         * @return 移动后的迭代器
         */
        self& operator++() {
            cur++;
            if (cur == last) {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        /**
         * @brief 使迭代器向后移动一位
         * @return 移动前的迭代器
         */
        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        /**
         * @brief 使迭代器向前移动一位
         * @return 移动后的迭代器
         */
        self& operator--() {
            if (cur == first) {
                set_node(node - 1);
                cur = last;
            }
            cur--;
            return *this;
        }

        /**
         * @brief 使迭代器向前移动一位
         * @return 移动前的迭代器
         */
        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }

        /**
         * @brief 将迭代器向前移动n位
         * @param n 要移动的位数
         * @return 移动后的迭代器
         */
        self& operator+=(difference_type n) {
            difference_type offset = n + (cur - first);
            if (offset >= 0 && offset <= difference_type(buffer_size())) {
                cur += n;
            } else {
                difference_type node_offset =
                    offset > 0 ? offset / difference_type(buffer_size())
                        : -difference_type((-offset - 1) / buffer_size()) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * difference_type(buffer_size()));
            }
            return *this;
        }

        /**
         * @brief 获取当前迭代器后方第n个迭代器
         * @param n 偏移量
         * @return 后方第n个迭代器
         */
        self operator+(difference_type n) const {
            self tmp = *this;
            return tmp += n;
        }

        /**
         * @brief 将迭代器向后移动n位
         * @param n 要移动的位数
         * @return 移动后的迭代器
         */
        self& operator-=(difference_type n) { return *this += -n; }

        /**
         * @brief 获取当前迭代器后方第n个迭代器
         * @param n 偏移量
         * @return 后方第n个迭代器
         */
        self operator-(difference_type n) const {
            self tmp = *this;
            return tmp -= n;
        }

        /**
         * @brief 获取以当前迭代器为数组首位的第n个元素
         * @param n 元素索引
         * @return 以当前迭代器为数组首位的第n个元素
         */
        reference operator[](difference_type n) const { return *(*this + n); }

        /**
         * @brief 判断两个迭代器是否等同
         * @param x 另一个迭代器
         * @return 如果等同，返回true；否则返回false
         */
        bool operator==(const self& x) const { return cur == x.cur; }

        /**
         * @brief 判断两个迭代器是否不等同
         * @param x 另一个迭代器
         * @return 如果不等同，返回true；否则返回false
         */
        bool operator!=(const self& x) const { return !(*this == x); }

        /**
         * @brief 判断*this是否小于x
         * @param x 另一个迭代器
         * @return 如果*this小于x，返回true；否则返回false
         */
        bool operator<(const self& x) const {
            return (node == x.node) ? (cur < x.cur) : (node < x.node);
        }

        /**
         * @brief 判断*this是否大于x
         * @param x 另一个迭代器
         * @return 如果*this大于x，返回true；否则返回false
         */
        bool operator>(const self& x) const {
            return (node == x.node) ? (cur > x.cur) : (node > x.node);
        }

        /**
         * @brief 判断*this是否大于等于x
         * @param x 另一个迭代器
         * @return 如果*this大于等于x，返回true；否则返回false
         */
        bool operator>=(const self& x) const {
            return (*this == x) || (*this > x);
        }

        /**
         * @brief 判断*this是否小于等于x
         * @param x 另一个迭代器
         * @return 如果*this小于等于x，返回true；否则返回false
         */
        bool operator<=(const self& x) const {
            return (*this == x) || (*this < x);
        }
    };

    /**
     * @brief deque实现
     * @tparam T 元素类型
     * @tparam Alloc 内存分配器
     * @tparam buffer 缓冲区大小
     */
    template <typename T, typename Alloc = allocator<T>, size_t buffer = 512>
    class deque {
    public:
        using value_type = T; /**< 元素类型 */
        using size_type = size_t; /**< 元素大小类型 */
        using pointer = T*; /**< 指针类型 */
        using reference = T&; /**< 引用类型 */
        using difference_type = ptrdiff_t; /**< 指针距离类型 */
        using iterator = __deque_iterator<T, buffer>; /**< 迭代器 */
    protected:
        using map_pointer = pointer*;
    public:
        /**
         * @brief 默认构造函数
         */
        deque() : _map(nullptr), _map_size(0) {
            initialize_map_and_nodes(0);
        }

        /**
         * @brief 构造函数，创建包含n个值为value的元素的deque
         * @param n 元素数量
         * @param value 元素的值
         */
        deque(size_type n, const value_type& value) : _map(nullptr), _map_size(0) {
            initialize_map_and_nodes(n);
            for (iterator it = _begin; it != _end; ++it) {
                *it = value;
            }
        }

        /**
         * @brief 复制构造函数
         * @param other 要复制的deque
         */
        deque(const deque& other) : _map(nullptr), _map_size(0) {
            initialize_map_and_nodes(other.size());
            iterator dest = _begin;
            for (iterator src = other._begin; src != other._end; ++src, ++dest) {
                *dest = *src;
            }
        }

        /**
         * @brief 移动构造函数
         * @param other 要移动的deque
         */
        deque(deque&& other) noexcept : _begin(other._begin), _end(other._end), _map(other._map), _map_size(other._map_size) {
            other._begin = iterator();
            other._end = iterator();
            other._map = nullptr;
            other._map_size = 0;
        }

        /**
         * @brief 构造函数，使用迭代器范围 [first, last) 初始化 deque
         * @tparam InputIterator 输入迭代器类型
         * @param first 起始迭代器
         * @param last 结束迭代器
         */
        template <typename InputIterator>
        deque(InputIterator first, InputIterator last) : _map(nullptr), _map_size(0) {
            // 计算元素数量
            size_type num_elements = 0;
            InputIterator temp = first;
            while (temp != last) {
                ++num_elements;
                ++temp;
            }

            // 初始化管控中心和节点
            initialize_map_and_nodes(num_elements);

            // 将迭代器范围内的元素复制到 deque 中
            iterator dest = _begin;
            for (InputIterator src = first; src != last; ++src, ++dest) {
                *dest = *src;
            }
        }

        /**
         * @brief 析构函数
         */
        ~deque() {
            clear();
            deallocate_map();
        }

        /**
         * @brief 复制赋值运算符
         * @param other 要复制的deque
         * @return 赋值后的deque引用
         */
        deque& operator=(const deque& other) {
            if (this != &other) {
                clear();
                initialize_map_and_nodes(other.size());
                iterator dest = _begin;
                for (iterator src = other._begin; src != other._end; ++src, ++dest) {
                    *dest = *src;
                }
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         * @param other 要移动的deque
         * @return 赋值后的deque引用
         */
        deque& operator=(deque&& other) noexcept {
            if (this != &other) {
                clear();
                deallocate_map();
                _begin = other._begin;
                _end = other._end;
                _map = other._map;
                _map_size = other._map_size;
                other._begin = iterator();
                other._end = iterator();
                other._map = nullptr;
                other._map_size = 0;
            }
            return *this;
        }

        /**
         * @brief 获取deque的首迭代器
         * @return deque的首迭代器
         */
        iterator begin() { return _begin; }

        /**
         * @brief 获取deque的尾迭代器
         * @return deque的尾迭代器
         */
        iterator end() { return _end; }

        /**
         * @brief 获取deque的第n个元素
         * @param n 元素索引
         * @return deque的第n个元素
         */
        reference operator[](size_type n) {
            return _begin[difference_type(n)];
        }

        /**
         * @brief 获取deque的第一个元素
         * @return deque的第一个元素
         */
        reference front() {
            return *_begin;
        }

        /**
         * @brief 获取deque的最后一个元素
         * @return deque的最后一个元素
         */
        reference back() {
            return *(_end - 1);
        }

        /**
         * @brief 获取deque的大小
         * @return deque的大小
         */
        size_type size() const { return _end - _begin; }

        /**
         * @brief 获取deque的最大大小
         * @return deque的最大大小
         */
        size_type max_size() const { return size_type(-1); }

        /**
         * @brief 判断deque是否为空
         * @return 如果为空，返回true；否则返回false
         */
        bool empty() const { return _end == _begin; }

        /**
         * @brief 在deque的末尾添加一个元素
         * @param value 要添加的元素的值
         */
        void push_back(const value_type& value) {
            if (_end.cur != _end.last - 1) {
                alloc.construct(_end.cur, value);
                ++_end.cur;
            } else {
                // 在当前缓冲区最后位置构造元素
                alloc.construct(_end.cur, value);
                // 检查并扩展 map
                if (_end.node + 1 == _map + _map_size) {
                    reallocate_map(1, false);
                }
                reserve_map_at_back();
                // 分配新缓冲区并移动迭代器
                map_pointer new_node = _end.node + 1;
                *new_node = allocate_node();
                _end.set_node(new_node);
                _end.cur = _end.first; // 新缓冲区起始位置
            }
        }

        /**
         * @brief 在deque的开头添加一个元素
         * @param value 要添加的元素的值
         */
        void push_front(const value_type& value) {
            if (_begin.cur == _begin.first) {
                if (_begin.node == _map) {
                    reallocate_map(1, true);
                }
                reserve_map_at_front();
                *--_begin.node = allocate_node();
                _begin.set_node(_begin.node);
                _begin.cur = _begin.last - 1;
            } else {
                --_begin.cur;
            }
            alloc.construct(_begin.cur, value);
        }

        /**
         * @brief 删除deque的最后一个元素
         */
        void pop_back() {
            if (!empty()) {
                if (_end.cur != _end.first) {
                    --_end.cur;
                    alloc.destroy(_end.cur);
                } else {
                    // 移动到前一个缓冲区
                    deallocate_node(_end.node); // 释放空缓冲区
                    _end.set_node(_end.node - 1);
                    _end.cur = _end.last - 1;   // 最后一个元素位置
                    alloc.destroy(_end.cur);
                    _end.cur = _end.last;       // 设置为尾后位置
                }
            }
        }

        /**
         * @brief 删除deque的第一个元素
         */
        void pop_front() {
            if (!empty()) {
                alloc.destroy(_begin.cur);
                if (_begin.cur == _begin.last - 1) {
                    deallocate_node(_begin.node);
                    _begin.set_node(_begin.node + 1);
                    _begin.cur = _begin.first;
                } else {
                    ++_begin.cur;
                }
            }
        }

        /**
         * @brief 清空deque中的所有元素
         */
        void clear() {
            for (map_pointer node = _begin.node + 1; node < _end.node; ++node) {
                for (pointer p = *node; p < *node + buffer; ++p) {
                    alloc.destroy(p);
                }
                deallocate_node(*node);
            }
            if (_begin.node != _end.node) {
                for (pointer p = _begin.cur; p < _begin.last; ++p) {
                    alloc.destroy(p);
                }
                for (pointer p = _end.first; p < _end.cur; ++p) {
                    alloc.destroy(p);
                }
                deallocate_node(*_end.node);
            } else {
                for (pointer p = _begin.cur; p < _end.cur; ++p) {
                    alloc.destroy(p);
                }
            }
            _end = _begin;
        }

    private:
        iterator _begin;
        iterator _end;
        map_pointer _map;
        size_type _map_size;
        Alloc alloc;
        typename Alloc::template rebind<pointer>::other map_alloc;

        /**
         * @brief 初始化管控中心和节点
         * @param num_elements 元素数量
         */
        void initialize_map_and_nodes(size_type num_elements) {
            size_type num_nodes = num_elements / buffer + 1;
            _map_size = std::max(static_cast<size_type>(8), num_nodes + 2);
            _map = allocate_map(_map_size);
            map_pointer nstart = _map + (_map_size - num_nodes) / 2;
            map_pointer nfinish = nstart + num_nodes;
            try {
                create_nodes(nstart, nfinish);
            } catch (...) {
                deallocate_map();
                throw;
            }
            _begin.set_node(nstart);
            _end.set_node(nfinish - 1);
            _begin.cur = _begin.first;
            _end.cur = _end.first + num_elements % buffer;
        }

        /**
         * @brief 分配管控中心内存
         * @param n 要分配的节点数量
         * @return 分配的管控中心指针
         */
        map_pointer allocate_map(size_type n) {
            return map_alloc.allocate(n);
        }

        /**
         * @brief 释放管控中心内存
         */
        void deallocate_map() {
            if (_map) {
                map_alloc.deallocate(_map, _map_size);
            }
        }

        /**
         * @brief 分配一个节点的内存
         * @return 分配的节点指针
         */
        pointer allocate_node() {
            return alloc.allocate(buffer);
        }

        /**
         * @brief 释放一个节点的内存
         * @param p 要释放的节点指针
         */
        void deallocate_node(pointer p) {
            alloc.deallocate(p, buffer);
        }

        /**
         * @brief 创建节点
         * @param nstart 起始节点指针
         * @param nfinish 结束节点指针
         */
        void create_nodes(map_pointer nstart, map_pointer nfinish) {
            for (map_pointer cur = nstart; cur < nfinish; ++cur) {
                *cur = allocate_node();
            }
        }

        /**
         * @brief 重新分配管控中心内存
         * @param nodes_to_add 要添加的节点数量
         * @param add_at_front 是否在前面添加
         */
        void reallocate_map(size_type nodes_to_add, bool add_at_front) {
            size_type old_num_nodes = _end.node - _begin.node + 1;
            size_type new_num_nodes = old_num_nodes + nodes_to_add;
            map_pointer new_nstart;
            if (_map_size > 2 * new_num_nodes) {
                new_nstart = _map + (_map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
                if (new_nstart < _begin.node) {
                    std::copy(_begin.node, _end.node + 1, new_nstart);
                } else {
                    std::copy_backward(_begin.node, _end.node + 1, new_nstart + old_num_nodes);
                }
            } else {
                size_type new_map_size = _map_size + std::max(_map_size, nodes_to_add) + 2;
                map_pointer new_map = allocate_map(new_map_size);
                new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                    + (add_at_front ? nodes_to_add : 0);
                std::copy(_begin.node, _end.node + 1, new_nstart);
                deallocate_map();
                _map = new_map;
                _map_size = new_map_size;
            }
            _begin.set_node(new_nstart);
            _end.set_node(new_nstart + old_num_nodes - 1);
        }

        /**
         * @brief 为尾部预留管控中心空间
         */
        void reserve_map_at_back(size_type nodes_to_add = 1) {
            if (nodes_to_add > _map_size - (_end.node - _map)) {
                reallocate_map(nodes_to_add, false);
            }
        }

        /**
         * @brief 为头部预留管控中心空间
         */
        void reserve_map_at_front(size_type nodes_to_add = 1) {
            if (nodes_to_add > static_cast<size_type>(_begin.node - _map)) {
                reallocate_map(nodes_to_add, true);
            }
        }
    };
}
