/**
 * @file array.hpp
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
     * @brief 固定大小的数组容器
     */
    template <typename T, size_t n>
    class array {
        static_assert(n > 0, "数组大小必大于0");
    public:
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = tiny_stl::reverse_iterator<iterator>;
        using const_reverse_iterator = tiny_stl::reverse_iterator<const_iterator>;
    public:
        array() {
            for (size_type i = 0; i < n; i++) {
                _data[i] = T();
            }
        }
        array(const array& other) {
            for (size_type i = 0; i < n; i++) {
                _data[i] = other._data[i];
            }
        }
        array& operator=(const array& other) {
            if (this != &other) {
                for (size_type i = 0; i < n; i++) {
                    _data[i] = other._data[i];
                }
            }
        }
        array& operator=(std::initializer_list<value_type> list) {
            if (list.size() != n) { throw std::out_of_range("list长度不等于数组长度"); }
            size_type i = 0;
            for (auto&& v : list) {
                _data[i++] = v;
            }
            return *this;
        }
        array(array&& other) noexcept {
            for (size_type i = 0; i < n; i++) {
                _data[i] = tiny_stl::move(other._data[i]);
            }
        }
        array(std::initializer_list<value_type> list) {
            if (list.size() != n) { throw std::out_of_range("list长度不等于数组长度"); }
            size_type i = 0;
            for (auto&& v : list) {
                _data[i++] = v;
            }
        }
    public:
        reference operator[](size_type index) {
            return _data[index];
        }
        const_reference operator[](size_type index) const {
            return _data[index];
        }
        reference at(size_type index) {
            if (index >= n) {
                throw std::out_of_range("数组越界");
            }
            return _data[index];
        }
        const_reference at(size_type index) const {
            if (index >= n) {
                throw std::out_of_range("数组越界");
            }
            return _data[index];
        }
        reference front() {
            return _data[0];
        }
        const_reference front() const {
            return _data[0];
        }
        reference back() {
            return _data[n - 1];
        }
        const_reference back() const {
            return _data[n - 1];
        }
        pointer data() noexcept {
            return _data;
        }
        const_pointer data() const noexcept {
            return _data;
        }
        iterator begin() noexcept {
            return _data;
        }
        const_iterator begin() const noexcept {
            return _data;
        }
        reverse_iterator rbegin() const noexcept {
            return reverse_iterator{ _data };
        }
        iterator end() noexcept {
            return _data + n;
        }
        const_iterator end() const noexcept {
            return _data + n;
        }
        reverse_iterator rend() const noexcept {
            return reverse_iterator{ _data + n };
        }
    public:
        size_type size() const noexcept {
            return n;
        }
        size_type max_size() const noexcept {
            return n;
        }
        void fill(const T& value) {
            for (size_type i = 0; i < n; i++) {
                _data[i] = value;
            }
        }
        void swap(array& other) noexcept {
            for (size_type i = 0; i < n; i++) {
                tiny_stl::swap(_data[i], other._data[i]);
            }
        }
    private:
        T _data[n]; // 存储数据的数组
    };

}
