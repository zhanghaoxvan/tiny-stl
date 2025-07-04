/**
 * @file vector.hpp
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
     * @brief 动态数组容器
     */
    template <typename T, typename Alloc = allocator<T>>
    class vector {
    public:
        using value_type             = T;
        using allocator_type         = Alloc;
        using size_type              = size_t;
        using difference_type        = ptrdiff_t;
        using pointer                = T*;
        using const_pointer          = const T*;
        using reference              = T&;
        using const_reference        = const T&;
        using iterator               = T*;
        using const_iterator         = const T*;
        using reverse_iterator       = tiny_stl::reverse_iterator<iterator>;
        using const_reverse_iterator = tiny_stl::reverse_iterator<const_iterator>;
    public:
        vector() : _begin(nullptr), _end(nullptr), _end_of_storage(nullptr),
                   _alloc(allocator_type()) { }
        vector(size_type n, const value_type& value = value_type())
            : _alloc(allocator_type()) {
            _begin = _alloc.allocate(n);
            _end = _begin + n;
            _end_of_storage = _end;
            for (size_type i = 0; i < n; ++i) {
                _alloc.construct(_begin + i, value);
            }
        }
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
        vector(vector&& other) noexcept
            : _begin(other._begin), _end(other._end),
              _end_of_storage(other._end_of_storage), _alloc(move(other._alloc)) {
            other._begin = nullptr;
            other._end = nullptr;
            other._end_of_storage = nullptr;
        }
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
        vector(std::initializer_list<value_type> list) {
            *this = vector{ list.begin(), list.end() };
        }
        ~vector() {
            for (iterator it = _begin; it != _end; ++it) {
                _alloc.destroy(it);
            }
            _alloc.deallocate(_begin, _end_of_storage - _begin);
        }
    public:
        size_type size() const noexcept {
            return _end - _begin;
        }
        size_type capacity() const noexcept {
            return _end_of_storage - _begin;
        }
        bool empty() const noexcept {
            return _begin == _end;
        }
        void clear() noexcept {
            for (iterator it = _begin; it != _end; ++it) {
                _alloc.destroy(it);
            }
            _end = _begin;
        }
        iterator begin() noexcept {
            return _begin;
        }
        const_iterator begin() const noexcept {
            return _begin;
        }
        iterator end() noexcept {
            return _end;
        }
        const_iterator end() const noexcept {
            return _end;
        }
        reverse_iterator rbegin() noexcept {
            return reverse_iterator(_end);
        }
        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator(_end);
        }
        reverse_iterator rend() noexcept {
            return reverse_iterator(_begin);
        }
        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator(_begin);
        }
    public:
        reference operator[](size_type index) {
            return _begin[index];
        }
        const_reference operator[](size_type index) const {
            return _begin[index];
        }
        reference at(size_type index) {
            if (index >= size()) {
                throw std::out_of_range("Index out of range");
            }
            return _begin[index];
        }
        const_reference at(size_type index) const {
            if (index >= size()) {
                throw std::out_of_range("Index out of range");
            }
            return _begin[index];
        }
        reference front() {
            return *_begin;
        }
        const_reference front() const {
            return *_begin;
        }
        reference back() {
            return *(_end - 1);
        }
        const_reference back() const {
            return *(_end - 1);
        }
        pointer data() noexcept {
            return _begin;
        }
        const_pointer data() const noexcept {
            return _begin;
        }
        
    public:
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
        void pop_back() {
            if (_begin != _end) {
                --_end;
                _alloc.destroy(_end);
            }
        }
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
        
        void swap(vector& other) noexcept {
            swap(_begin, other._begin);
            swap(_end, other._end);
            swap(_end_of_storage, other._end_of_storage);
            swap(_alloc, other._alloc);
        }
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
        bool operator!=(const vector& other) const {
            return !(*this == other);
        }
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
        bool operator<=(const vector& other) const {
            return *this < other || *this == other;
        }
        bool operator>(const vector& other) const {
            return !(*this <= other);
        }
        bool operator>=(const vector& other) const {
            return !(*this < other);
        }
        friend void swap(vector& lhs, vector& rhs) noexcept {
            lhs.swap(rhs);
        }
        allocator_type get_allocator() const noexcept {
            return _alloc;
        }

        // 插入单个元素
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

        // 删除单个元素
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

        // 删除指定范围的元素
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
        iterator _begin;
        iterator _end;
        iterator _end_of_storage;
        allocator_type _alloc;
    };

}
