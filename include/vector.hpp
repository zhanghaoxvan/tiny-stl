/**
 * @file vector.hpp
 * @brief 此文件定义了 tiny_stl 命名空间下的 vector 类，实现了一个动态数组容器。
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
#include <utility.hpp>
#include <initializer_list>

namespace tiny_stl {

    /**
     * @class vector
     * @brief 动态数组容器，类似于标准库中的 std::vector。
     *
     * 该类提供了动态数组的功能，支持元素的插入、删除、访问等操作，并且可以自动管理内存。
     *
     * @tparam T 容器中存储的元素类型。
     * @tparam Alloc 用于内存分配和释放的分配器类型，默认为 allocator<T>。
     */
    template <typename T, typename Alloc = allocator<T>>
    class vector {
    public:
        /**
         * @brief 容器中存储的元素类型。
         */
        using value_type             = T;
        /**
         * @brief 用于内存分配和释放的分配器类型。
         */
        using allocator_type         = Alloc;
        /**
         * @brief 表示容器大小的无符号整数类型。
         */
        using size_type              = size_t;
        /**
         * @brief 表示迭代器之间差值的有符号整数类型。
         */
        using difference_type        = ptrdiff_t;
        /**
         * @brief 指向元素的指针类型。
         */
        using pointer                = T*;
        /**
         * @brief 指向常量元素的指针类型。
         */
        using const_pointer          = const T*;
        /**
         * @brief 元素的引用类型。
         */
        using reference              = T&;
        /**
         * @brief 常量元素的引用类型。
         */
        using const_reference        = const T&;
        /**
         * @brief 迭代器类型，用于遍历容器中的元素。
         */
        using iterator               = T*;
        /**
         * @brief 常量迭代器类型，用于遍历容器中的常量元素。
         */
        using const_iterator         = const T*;
        /**
         * @brief 反向迭代器类型，用于从容器末尾开始向前遍历元素。
         */
        using reverse_iterator       = tiny_stl::reverse_iterator<iterator>;
        /**
         * @brief 常量反向迭代器类型，用于从容器末尾开始向前遍历常量元素。
         */
        using const_reverse_iterator = tiny_stl::reverse_iterator<const_iterator>;

        /**
         * @brief 默认构造函数，创建一个空的 vector。
         */
        vector() : _begin(nullptr), _end(nullptr), _end_of_storage(nullptr),
                   _alloc(allocator_type()) { }

        /**
         * @brief 构造一个包含 n 个元素的 vector，每个元素的值都为 value。
         *
         * @param n 要创建的元素数量。
         * @param value 元素的初始值，默认为 value_type()。
         */
        vector(size_type n, const value_type& value = value_type())
            : _alloc(allocator_type()) {
            _begin = _alloc.allocate(n);
            _end = _begin + n;
            _end_of_storage = _end;
            for (size_type i = 0; i < n; ++i) {
                _alloc.construct(_begin + i, value);
            }
        }

        /**
         * @brief 拷贝构造函数，创建一个与 other 相同的 vector。
         *
         * @param other 要拷贝的 vector。
         */
        vector(const vector& other)
            : _alloc(other._alloc) {
            size_type n = other.size();
            _begin = _alloc.allocate(n);
            _end = _begin + n;
            _end_of_storage = _end;
            for (size_type i = 0; i < n; ++i) {
                _alloc.construct(_begin + i, other._begin[i]);
            }
        }

        /**
         * @brief 移动构造函数，将 other 的资源移动到新的 vector 中。
         *
         * @param other 要移动的 vector。
         */
        vector(vector&& other) noexcept
            : _begin(other._begin), _end(other._end),
              _end_of_storage(other._end_of_storage), _alloc(move(other._alloc)) {
            other._begin = nullptr;
            other._end = nullptr;
            other._end_of_storage = nullptr;
        }

        /**
         * @brief 构造一个包含 [first, last) 范围内元素的 vector。
         *
         * @tparam InputIterator 输入迭代器类型。
         * @param first 范围的起始迭代器。
         * @param last 范围的结束迭代器。
         */
        template <typename InputIterator>
        vector(InputIterator first, InputIterator last) {
            auto n = distance(first, last);
            _begin = _alloc.allocate(n);
            _end = _begin + n;
            _end_of_storage = _end;
            for (size_type i = 0; i < n; i++, first++) {
                _alloc.construct(_begin + i, *first);
            }
        }

        /**
         * @brief 构造一个包含 initializer_list 中元素的 vector。
         *
         * @param list 初始化列表。
         */
        vector(std::initializer_list<value_type> list) {
            *this = vector{ list.begin(), list.end() };
        }

        /**
         * @brief 析构函数，释放 vector 占用的内存。
         */
        ~vector() {
            for (iterator it = _begin; it != _end; ++it) {
                _alloc.destroy(it);
            }
            _alloc.deallocate(_begin, _end_of_storage - _begin);
        }

        /**
         * @brief 返回 vector 中当前存储的元素数量。
         *
         * @return 元素数量。
         */
        size_type size() const noexcept {
            return _end - _begin;
        }

        /**
         * @brief 返回 vector 当前分配的内存容量。
         *
         * @return 内存容量。
         */
        size_type capacity() const noexcept {
            return _end_of_storage - _begin;
        }

        /**
         * @brief 判断 vector 是否为空。
         *
         * @return 如果 vector 为空，返回 true；否则返回 false。
         */
        bool empty() const noexcept {
            return _begin == _end;
        }

        /**
         * @brief 清空 vector 中的所有元素，但不释放内存。
         */
        void clear() noexcept {
            for (iterator it = _begin; it != _end; ++it) {
                _alloc.destroy(it);
            }
            _end = _begin;
        }

        /**
         * @brief 返回指向 vector 第一个元素的迭代器。
         *
         * @return 指向第一个元素的迭代器。
         */
        iterator begin() noexcept {
            return _begin;
        }

        /**
         * @brief 返回指向 vector 第一个元素的常量迭代器。
         *
         * @return 指向第一个元素的常量迭代器。
         */
        const_iterator begin() const noexcept {
            return _begin;
        }

        /**
         * @brief 返回指向 vector 最后一个元素之后位置的迭代器。
         *
         * @return 指向最后一个元素之后位置的迭代器。
         */
        iterator end() noexcept {
            return _end;
        }

        /**
         * @brief 返回指向 vector 最后一个元素之后位置的常量迭代器。
         *
         * @return 指向最后一个元素之后位置的常量迭代器。
         */
        const_iterator end() const noexcept {
            return _end;
        }

        /**
         * @brief 返回指向 vector 最后一个元素的反向迭代器。
         *
         * @return 指向最后一个元素的反向迭代器。
         */
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(_end);
        }

        /**
         * @brief 返回指向 vector 最后一个元素的常量反向迭代器。
         *
         * @return 指向最后一个元素的常量反向迭代器。
         */
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(_end);
        }

        /**
         * @brief 返回指向 vector 第一个元素之前位置的反向迭代器。
         *
         * @return 指向第一个元素之前位置的反向迭代器。
         */
        reverse_iterator rend() noexcept {
            return reverse_iterator(_begin);
        }

        /**
         * @brief 返回指向 vector 第一个元素之前位置的常量反向迭代器。
         *
         * @return 指向第一个元素之前位置的常量反向迭代器。
         */
        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(_begin);
        }

        /**
         * @brief 重载 [] 运算符，返回指定位置的元素引用。
         *
         * @param index 元素的索引。
         * @return 指定位置的元素引用。
         */
        reference operator[](size_type index) {
            return _begin[index];
        }

        /**
         * @brief 重载 [] 运算符，返回指定位置的常量元素引用。
         *
         * @param index 元素的索引。
         * @return 指定位置的常量元素引用。
         */
        const_reference operator[](size_type index) const {
            return _begin[index];
        }

        /**
         * @brief 返回指定位置的元素引用，并进行边界检查。
         *
         * @param index 元素的索引。
         * @return 指定位置的元素引用。
         * @throw std::out_of_range 如果索引超出范围。
         */
        reference at(size_type index) {
            if (index >= size()) {
                throw std::out_of_range("Index out of range");
            }
            return _begin[index];
        }

        /**
         * @brief 返回指定位置的常量元素引用，并进行边界检查。
         *
         * @param index 元素的索引。
         * @return 指定位置的常量元素引用。
         * @throw std::out_of_range 如果索引超出范围。
         */
        const_reference at(size_type index) const {
            if (index >= size()) {
                throw std::out_of_range("Index out of range");
            }
            return _begin[index];
        }

        /**
         * @brief 返回 vector 第一个元素的引用。
         *
         * @return 第一个元素的引用。
         */
        reference front() {
            return *_begin;
        }

        /**
         * @brief 返回 vector 第一个元素的常量引用。
         *
         * @return 第一个元素的常量引用。
         */
        const_reference front() const {
            return *_begin;
        }

        /**
         * @brief 返回 vector 最后一个元素的引用。
         *
         * @return 最后一个元素的引用。
         */
        reference back() {
            return *(_end - 1);
        }

        /**
         * @brief 返回 vector 最后一个元素的常量引用。
         *
         * @return 最后一个元素的常量引用。
         */
        const_reference back() const {
            return *(_end - 1);
        }

        /**
         * @brief 返回指向 vector 第一个元素的指针。
         *
         * @return 指向第一个元素的指针。
         */
        pointer data() noexcept {
            return _begin;
        }

        /**
         * @brief 返回指向 vector 第一个元素的常量指针。
         *
         * @return 指向第一个元素的常量指针。
         */
        const_pointer data() const noexcept {
            return _begin;
        }

        /**
         * @brief 在 vector 的末尾添加一个元素。
         *
         * 如果容量不足，会自动进行扩容。
         *
         * @param value 要添加的元素。
         */
        void push_back(const value_type& value) {
            if (_end == _end_of_storage) {
                size_type new_capacity = capacity() ? capacity() * 2 : 1;
                pointer new_begin = _alloc.allocate(new_capacity);
                size_type count = size();
                for (size_type i = 0; i < count; ++i) {
                    _alloc.construct(new_begin + i, move(_begin[i]));
                    _alloc.destroy(_begin + i);
                }
                _alloc.deallocate(_begin, _end_of_storage - _begin);
                _begin = new_begin;
                _end = new_begin + count;
                _end_of_storage = new_begin + new_capacity;
            }
            _alloc.construct(_end, value);
            ++_end;
        }

        /**
         * @brief 删除 vector 末尾的元素。
         *
         * 如果 vector 不为空，会释放末尾元素的内存。
         */
        void pop_back() {
            if (_begin != _end) {
                --_end;
                _alloc.destroy(_end);
            }
        }

        /**
         * @brief 调整 vector 的大小。
         *
         * 如果 n 小于当前大小，会删除多余的元素；如果 n 大于当前大小，会添加新元素并初始化为 value。
         *
         * @param n 调整后的大小。
         * @param value 新元素的初始值，默认为 value_type()。
         */
        void resize(size_type n, const value_type& value = value_type()) {
            if (n < size()) {
                for (size_type i = n; i < size(); ++i) {
                    _alloc.destroy(_begin + i);
                }
                _end = _begin + n;
            } else if (n > size()) {
                if (n > capacity()) {
                    size_type new_capacity = capacity() ? capacity() * 2 : 1;
                    while (new_capacity < n) {
                        new_capacity *= 2;
                    }
                    pointer new_begin = _alloc.allocate(new_capacity);
                    for (size_type i = 0; i < size(); ++i) {
                        _alloc.construct(new_begin + i, move(_begin[i]));
                        _alloc.destroy(_begin + i);
                    }
                    _alloc.deallocate(_begin, _end_of_storage - _begin);
                    _begin = new_begin;
                    _end_of_storage = new_begin + new_capacity;
                }
                for (size_type i = size(); i < n; ++i) {
                    _alloc.construct(_end + i - size(), value);
                }
                _end = _begin + n;
            }
        }

        /**
         * @brief 预留至少 n 个元素的内存空间。
         *
         * 如果 n 大于当前容量，会进行扩容。
         *
         * @param n 要预留的内存空间大小。
         */
        void reserve(size_type n) {
            if (n > capacity()) {
                pointer new_begin = _alloc.allocate(n);
                for (size_type i = 0; i < size(); ++i) {
                    _alloc.construct(new_begin + i, move(_begin[i]));
                    _alloc.destroy(_begin + i);
                }
                _alloc.deallocate(_begin, _end_of_storage - _begin);
                _begin = new_begin;
                _end_of_storage = new_begin + n;
                _end = _begin + size();
            }
        }

        /**
         * @brief 交换两个 vector 的内容。
         *
         * @param other 要交换的 vector。
         */
        void swap(vector& other) noexcept {
            swap(_begin, other._begin);
            swap(_end, other._end);
            swap(_end_of_storage, other._end_of_storage);
            swap(_alloc, other._alloc);
        }

        /**
         * @brief 判断两个 vector 是否相等。
         *
         * @param other 要比较的 vector。
         * @return 如果两个 vector 的元素数量和对应元素都相等，返回 true；否则返回 false。
         */
        bool operator==(const vector& other) const {
            if (size() != other.size()) {
                return false;
            }
            for (size_type i = 0; i < size(); ++i) {
                if (_begin[i] != other._begin[i]) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief 判断两个 vector 是否不相等。
         *
         * @param other 要比较的 vector。
         * @return 如果两个 vector 不相等，返回 true；否则返回 false。
         */
        bool operator!=(const vector& other) const {
            return !(*this == other);
        }

        /**
         * @brief 判断当前 vector 是否小于另一个 vector。
         *
         * 比较规则是按元素逐个比较，直到找到不同的元素或其中一个 vector 结束。
         *
         * @param other 要比较的 vector。
         * @return 如果当前 vector 小于另一个 vector，返回 true；否则返回 false。
         */
        bool operator<(const vector& other) const {
            size_type min_size = min(size(), other.size());
            for (size_type i = 0; i < min_size; ++i) {
                if (_begin[i] < other._begin[i]) {
                    return true;
                } else if (_begin[i] > other._begin[i]) {
                    return false;
                }
            }
            return size() < other.size();
        }

        /**
         * @brief 判断当前 vector 是否小于等于另一个 vector。
         *
         * @param other 要比较的 vector。
         * @return 如果当前 vector 小于等于另一个 vector，返回 true；否则返回 false。
         */
        bool operator<=(const vector& other) const {
            return *this < other || *this == other;
        }

        /**
         * @brief 判断当前 vector 是否大于另一个 vector。
         *
         * @param other 要比较的 vector。
         * @return 如果当前 vector 大于另一个 vector，返回 true；否则返回 false。
         */
        bool operator>(const vector& other) const {
            return !(*this <= other);
        }

        /**
         * @brief 判断当前 vector 是否大于等于另一个 vector。
         *
         * @param other 要比较的 vector。
         * @return 如果当前 vector 大于等于另一个 vector，返回 true；否则返回 false。
         */
        bool operator>=(const vector& other) const {
            return !(*this < other);
        }

        /**
         * @brief 友元函数，用于交换两个 vector 的内容。
         *
         * @param lhs 第一个 vector。
         * @param rhs 第二个 vector。
         */
        friend void swap(vector& lhs, vector& rhs) noexcept {
            lhs.swap(rhs);
        }

        /**
         * @brief 返回当前使用的分配器。
         *
         * @return 当前使用的分配器。
         */
        allocator_type get_allocator() const noexcept {
            return _alloc;
        }

        /**
         * @brief 在指定位置插入一个元素。
         *
         * 如果容量不足，会自动进行扩容。
         *
         * @param position 插入位置的常量迭代器。
         * @param value 要插入的元素。
         * @return 指向插入元素的迭代器。
         */
        iterator insert(const_iterator position, const value_type& value) {
            difference_type index = position - begin();
            if (_end == _end_of_storage) {
                size_type new_capacity = capacity() ? capacity() * 2 : 1;
                pointer new_begin = _alloc.allocate(new_capacity);
                for (size_type i = 0; i < index; ++i) {
                    _alloc.construct(new_begin + i, move(_begin[i]));
                    _alloc.destroy(_begin + i);
                }
                _alloc.construct(new_begin + index, value);
                for (size_type i = index; i < size(); ++i) {
                    _alloc.construct(new_begin + i + 1, move(_begin[i]));
                    _alloc.destroy(_begin + i);
                }
                _alloc.deallocate(_begin, _end_of_storage - _begin);
                _begin = new_begin;
                _end = new_begin + size() + 1;
                _end_of_storage = new_begin + new_capacity;
            } else {
                for (iterator it = _end; it > _begin + index; --it) {
                    _alloc.construct(it, move(*(it - 1)));
                    _alloc.destroy(it - 1);
                }
                _alloc.construct(_begin + index, value);
                ++_end;
            }
            return _begin + index;
        }

        /**
         * @brief 删除指定位置的元素。
         *
         * @param position 要删除元素的常量迭代器。
         * @return 指向被删除元素之后位置的迭代器。
         */
        iterator erase(const_iterator position) {
            difference_type index = position - begin();
            _alloc.destroy(_begin + index);
            for (iterator it = _begin + index; it < _end - 1; ++it) {
                _alloc.construct(it, move(*(it + 1)));
                _alloc.destroy(it + 1);
            }
            --_end;
            return _begin + index;
        }

        /**
         * @brief 删除指定范围的元素。
         *
         * @param first 范围的起始常量迭代器。
         * @param last 范围的结束常量迭代器。
         * @return 指向被删除范围之后位置的迭代器。
         */
        iterator erase(const_iterator first, const_iterator last) {
            difference_type start_index = first - begin();
            difference_type end_index = last - begin();
            difference_type count = end_index - start_index;
            for (iterator it = _begin + start_index; it < _begin + end_index; ++it) {
                _alloc.destroy(it);
            }
            for (iterator it = _begin + end_index; it < _end; ++it) {
                _alloc.construct(it - count, move(*it));
                _alloc.destroy(it);
            }
            _end -= count;
            return _begin + start_index;
        }

    private:
        /**
         * @brief 指向 vector 第一个元素的迭代器。
         */
        iterator _begin;
        /**
         * @brief 指向 vector 最后一个元素之后位置的迭代器。
         */
        iterator _end;
        /**
         * @brief 指向 vector 分配内存末尾的迭代器。
         */
        iterator _end_of_storage;
        /**
         * @brief 当前使用的分配器。
         */
        allocator_type _alloc;
    };

}
