/**
 * @file utility.hpp
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include <iterator.hpp>

namespace tiny_stl {

    // 实现 remove_reference
    template <typename T> struct remove_reference { using type = T; };
    template <typename T> struct remove_reference<T&> { using type = T; };
    template <typename T> struct remove_reference<T&&> { using type = T; };

    // 实现 forward
    template <typename T>
    constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }

    template <typename T>
    constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
        return static_cast<T&&>(t);
    }

    // 实现 move
    template <typename T>
    constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
        return static_cast<typename remove_reference<T>::type&&>(t);
    }

    // 实现 exchange
    template <typename T, typename U = T>
    T exchange(T& obj, U&& new_val) {
        T old_val = move(obj);
        obj = forward<U>(new_val);
        return old_val;
    }

    // 实现 swap
    template <typename T>
    void swap(T& a, T& b) noexcept {
        T temp = move(a);
        a = move(b);
        b = move(temp);
    }

    template <typename T, typename U>
    struct pair {
        T first;
        U second;
        
        pair() = default;
        pair(const T& f, const U& s) : first(f), second(s) { }
        pair(T&& f, U&& s) : first(forward<T>(f)), second(forward<U>(s)) { }
        pair(const pair& other) = default;
        pair(pair&& other) noexcept : first(move(other.first)), second(move(other.second)) {
            other.first = T();
            other.second = U();
        }
        pair& operator=(const pair& other) = default;
        pair& operator=(pair&& other) noexcept {
            if (this != &other) {
                first = move(other.first);
                second = move(other.second);
                other.first = T();
                other.second = U();
            }
            return *this;
        }
        bool operator==(const pair& other) const {
            return first == other.first && second == other.second;
        }
        bool operator!=(const pair& other) const {
            return !(*this == other);
        }
    };
    template <typename T, typename U>
    pair<T, U> make_pair(T&& first, U&& second) {
        return pair<T, U>(forward<T>(first), forward<U>(second));
    }

}
