/**
 * @file array.hpp
 * @brief 该文件定义了一个固定大小的数组容器类。
 *
 * 此文件实现了 `tiny_stl::array` 类，它是一个固定大小的数组容器，
 * 提供了与标准库 `std::array` 类似的功能。
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include <cstddef>
#include <iterator.hpp>
#include <utility.hpp>
#include <memory.hpp>
#include <initializer_list>

namespace tiny_stl {
    
    /**
     * @class array
     * @brief 固定大小的数组容器。
     *
     * `array` 类模板表示一个固定大小的数组容器，其大小在编译时确定。
     * 它提供了随机访问功能，并且可以使用迭代器进行遍历。
     *
     * @tparam T 数组中元素的类型。
     * @tparam n 数组的大小，必须大于 0。
     */
    template <typename T, size_t n>
    class array {
        static_assert(n > 0, "数组大小必大于0");
    public:
        /**
         * @brief 数组中元素的类型。
         */
        using value_type = T;
        /**
         * @brief 表示容器大小的类型。
         */
        using size_type = size_t;
        /**
         * @brief 表示两个迭代器之间差值的类型。
         */
        using difference_type = ptrdiff_t;
        /**
         * @brief 指向数组元素的指针类型。
         */
        using pointer = T*;
        /**
         * @brief 指向常量数组元素的指针类型。
         */
        using const_pointer = const T*;
        /**
         * @brief 数组元素的引用类型。
         */
        using reference = T&;
        /**
         * @brief 常量数组元素的引用类型。
         */
        using const_reference = const T&;
        /**
         * @brief 数组的迭代器类型。
         */
        using iterator = T*;
        /**
         * @brief 常量数组的迭代器类型。
         */
        using const_iterator = const T*;
        /**
         * @brief 数组的反向迭代器类型。
         */
        using reverse_iterator = tiny_stl::reverse_iterator<iterator>;
        /**
         * @brief 常量数组的反向迭代器类型。
         */
        using const_reverse_iterator = tiny_stl::reverse_iterator<const_iterator>;

        /**
         * @brief 默认构造函数。
         *
         * 初始化数组的每个元素为默认值。
         */
        array() {
            for (size_type i = 0; i < n; i++) {
                _data[i] = T();
            }
        }

        /**
         * @brief 拷贝构造函数。
         *
         * 从另一个 `array` 对象复制元素。
         *
         * @param other 要复制的 `array` 对象。
         */
        array(const array& other) {
            for (size_type i = 0; i < n; i++) {
                _data[i] = other._data[i];
            }
        }

        /**
         * @brief 拷贝赋值运算符。
         *
         * 将另一个 `array` 对象的元素复制到当前对象。
         *
         * @param other 要复制的 `array` 对象。
         * @return 对当前对象的引用。
         */
        array& operator=(const array& other) {
            if (this != &other) {
                for (size_type i = 0; i < n; i++) {
                    _data[i] = other._data[i];
                }
            }
            return *this;
        }

        /**
         * @brief 初始化列表赋值运算符。
         *
         * 使用初始化列表中的元素赋值给数组。
         *
         * @param list 初始化列表，其长度必须等于数组的大小。
         * @return 对当前对象的引用。
         * @throws std::out_of_range 如果初始化列表的长度不等于数组的大小。
         */
        array& operator=(std::initializer_list<value_type> list) {
            if (list.size() != n) { throw std::out_of_range("list长度不等于数组长度"); }
            size_type i = 0;
            for (auto&& v : list) {
                _data[i++] = v;
            }
            return *this;
        }

        /**
         * @brief 移动构造函数。
         *
         * 从另一个 `array` 对象移动元素。
         *
         * @param other 要移动的 `array` 对象。
         */
        array(array&& other) noexcept {
            for (size_type i = 0; i < n; i++) {
                _data[i] = tiny_stl::move(other._data[i]);
            }
        }

        /**
         * @brief 使用初始化列表构造数组。
         *
         * 使用初始化列表中的元素初始化数组。
         *
         * @param list 初始化列表，其长度必须等于数组的大小。
         * @throws std::out_of_range 如果初始化列表的长度不等于数组的大小。
         */
        array(std::initializer_list<value_type> list) {
            if (list.size() != n) { throw std::out_of_range("list长度不等于数组长度"); }
            size_type i = 0;
            for (auto&& v : list) {
                _data[i++] = v;
            }
        }

        /**
         * @brief 下标运算符。
         *
         * 返回指定位置元素的引用。
         *
         * @param index 元素的索引。
         * @return 指定位置元素的引用。
         */
        reference operator[](size_type index) {
            return _data[index];
        }

        /**
         * @brief 常量版本的下标运算符。
         *
         * 返回指定位置常量元素的引用。
         *
         * @param index 元素的索引。
         * @return 指定位置常量元素的引用。
         */
        const_reference operator[](size_type index) const {
            return _data[index];
        }

        /**
         * @brief 检查边界的访问函数。
         *
         * 返回指定位置元素的引用，会检查索引是否越界。
         *
         * @param index 元素的索引。
         * @return 指定位置元素的引用。
         * @throws std::out_of_range 如果索引超出数组范围。
         */
        reference at(size_type index) {
            if (index >= n) {
                throw std::out_of_range("数组越界");
            }
            return _data[index];
        }

        /**
         * @brief 常量版本的检查边界的访问函数。
         *
         * 返回指定位置常量元素的引用，会检查索引是否越界。
         *
         * @param index 元素的索引。
         * @return 指定位置常量元素的引用。
         * @throws std::out_of_range 如果索引超出数组范围。
         */
        const_reference at(size_type index) const {
            if (index >= n) {
                throw std::out_of_range("数组越界");
            }
            return _data[index];
        }

        /**
         * @brief 返回数组的第一个元素的引用。
         *
         * @return 数组的第一个元素的引用。
         */
        reference front() {
            return _data[0];
        }

        /**
         * @brief 返回数组的第一个元素的常量引用。
         *
         * @return 数组的第一个元素的常量引用。
         */
        const_reference front() const {
            return _data[0];
        }

        /**
         * @brief 返回数组的最后一个元素的引用。
         *
         * @return 数组的最后一个元素的引用。
         */
        reference back() {
            return _data[n - 1];
        }

        /**
         * @brief 返回数组的最后一个元素的常量引用。
         *
         * @return 数组的最后一个元素的常量引用。
         */
        const_reference back() const {
            return _data[n - 1];
        }

        /**
         * @brief 返回指向数组数据的指针。
         *
         * @return 指向数组数据的指针。
         */
        pointer data() noexcept {
            return _data;
        }

        /**
         * @brief 返回指向常量数组数据的指针。
         *
         * @return 指向常量数组数据的指针。
         */
        const_pointer data() const noexcept {
            return _data;
        }

        /**
         * @brief 返回指向数组第一个元素的迭代器。
         *
         * @return 指向数组第一个元素的迭代器。
         */
        iterator begin() noexcept {
            return _data;
        }

        /**
         * @brief 返回指向常量数组第一个元素的迭代器。
         *
         * @return 指向常量数组第一个元素的迭代器。
         */
        const_iterator begin() const noexcept {
            return _data;
        }

        /**
         * @brief 返回指向数组最后一个元素之后的反向迭代器。
         *
         * @return 指向数组最后一个元素之后的反向迭代器。
         */
        reverse_iterator rbegin() const noexcept {
            return reverse_iterator{ _data };
        }

        /**
         * @brief 返回指向数组最后一个元素之后的迭代器。
         *
         * @return 指向数组最后一个元素之后的迭代器。
         */
        iterator end() noexcept {
            return _data + n;
        }

        /**
         * @brief 返回指向常量数组最后一个元素之后的迭代器。
         *
         * @return 指向常量数组最后一个元素之后的迭代器。
         */
        const_iterator end() const noexcept {
            return _data + n;
        }

        /**
         * @brief 返回指向数组第一个元素之前的反向迭代器。
         *
         * @return 指向数组第一个元素之前的反向迭代器。
         */
        reverse_iterator rend() const noexcept {
            return reverse_iterator{ _data + n };
        }

        /**
         * @brief 返回数组的大小。
         *
         * @return 数组的大小。
         */
        size_type size() const noexcept {
            return n;
        }

        /**
         * @brief 返回数组的最大大小。
         *
         * 由于数组大小是固定的，所以最大大小等于数组的大小。
         *
         * @return 数组的最大大小。
         */
        size_type max_size() const noexcept {
            return n;
        }

        /**
         * @brief 用指定的值填充数组。
         *
         * 将数组的每个元素设置为指定的值。
         *
         * @param value 要填充的值。
         */
        void fill(const T& value) {
            for (size_type i = 0; i < n; i++) {
                _data[i] = value;
            }
        }

        /**
         * @brief 交换两个数组的元素。
         *
         * 交换当前数组和另一个数组的元素。
         *
         * @param other 要交换的另一个数组。
         */
        void swap(array& other) noexcept {
            for (size_type i = 0; i < n; i++) {
                tiny_stl::swap(_data[i], other._data[i]);
            }
        }
    private:
        /**
         * @brief 存储数组元素的底层数组。
         */
        T _data[n]; 
    };

}
