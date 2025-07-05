/**
 * @file utility.hpp
 * @brief 此文件定义了 `tiny_stl` 命名空间下的一些常用工具函数和模板类，如类型操作、转发、移动、交换等，以及 `pair` 模板类。
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

    /**
     * @struct remove_reference
     * @brief 移除类型的引用修饰符。
     * @tparam T 要处理的类型
     */
    template <typename T> 
    struct remove_reference { 
        /**
         * @brief 移除引用后的类型
         */
        using type = T; 
    };

    /**
     * @struct remove_reference<T&>
     * @brief 移除左值引用修饰符的特化版本。
     * @tparam T 要处理的类型
     */
    template <typename T> 
    struct remove_reference<T&> { 
        /**
         * @brief 移除引用后的类型
         */
        using type = T; 
    };

    /**
     * @struct remove_reference<T&&>
     * @brief 移除右值引用修饰符的特化版本。
     * @tparam T 要处理的类型
     */
    template <typename T> 
    struct remove_reference<T&&> { 
        /**
         * @brief 移除引用后的类型
         */
        using type = T; 
    };

    /**
     * @brief 完美转发左值引用。
     * 该函数将左值引用转发为原始类型的引用，保留其值类别。
     * @tparam T 要转发的类型
     * @param t 要转发的左值引用
     * @return 转发后的引用
     */
    template <typename T>
    constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }

    /**
     * @brief 完美转发右值引用。
     * 该函数将右值引用转发为原始类型的引用，保留其值类别。
     * @tparam T 要转发的类型
     * @param t 要转发的右值引用
     * @return 转发后的引用
     */
    template <typename T>
    constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
        return static_cast<T&&>(t);
    }

    /**
     * @brief 将左值或右值转换为右值引用。
     * 该函数用于实现移动语义，允许资源的高效转移。
     * @tparam T 要处理的类型
     * @param t 要转换的对象
     * @return 转换后的右值引用
     */
    template <typename T>
    constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
        return static_cast<typename remove_reference<T>::type&&>(t);
    }

    /**
     * @brief 替换对象的值并返回其旧值。
     * 该函数使用移动语义，避免不必要的复制。
     * @tparam T 对象的类型
     * @tparam U 新值的类型，默认为 T
     * @param obj 要替换值的对象
     * @param new_val 新的值
     * @return 对象的旧值
     */
    template <typename T, typename U = T>
    T exchange(T& obj, U&& new_val) {
        T old_val = move(obj);
        obj = forward<U>(new_val);
        return old_val;
    }

    /**
     * @brief 交换两个对象的值。
     * 该函数使用移动语义，避免不必要的复制。
     * @tparam T 对象的类型
     * @param a 第一个对象
     * @param b 第二个对象
     */
    template <typename T>
    void swap(T& a, T& b) noexcept {
        T temp = move(a);
        a = move(b);
        b = move(temp);
    }

    /**
     * @class pair
     * @brief 存储两个不同类型的值的模板类。
     * 该类类似于标准库中的 `std::pair`，提供了基本的构造、赋值和比较操作。
     * @tparam T 第一个值的类型
     * @tparam U 第二个值的类型
     */
    template <typename T, typename U>
    struct pair {
        /**
         * @brief 第一个值
         */
        T first;
        /**
         * @brief 第二个值
         */
        U second;
        
        /**
         * @brief 默认构造函数。
         */
        pair() = default;

        /**
         * @brief 构造函数，使用左值引用初始化两个值。
         * @param f 第一个值的左值引用
         * @param s 第二个值的左值引用
         */
        pair(const T& f, const U& s) : first(f), second(s) { }

        /**
         * @brief 构造函数，使用右值引用初始化两个值。
         * 该构造函数使用完美转发，避免不必要的复制。
         * @param f 第一个值的右值引用
         * @param s 第二个值的右值引用
         */
        pair(T&& f, U&& s) : first(forward<T>(f)), second(forward<U>(s)) { }

        /**
         * @brief 拷贝构造函数。
         */
        pair(const pair& other) = default;

        /**
         * @brief 移动构造函数。
         * 该构造函数使用移动语义，将资源从另一个 `pair` 对象转移到当前对象。
         * @param other 要移动的 `pair` 对象
         */
        pair(pair&& other) noexcept : first(move(other.first)), second(move(other.second)) {
            other.first = T();
            other.second = U();
        }

        /**
         * @brief 拷贝赋值运算符。
         */
        pair& operator=(const pair& other) = default;

        /**
         * @brief 移动赋值运算符。
         * 该运算符使用移动语义，将资源从另一个 `pair` 对象转移到当前对象。
         * @param other 要移动的 `pair` 对象
         * @return 指向当前 `pair` 对象的引用
         */
        pair& operator=(pair&& other) noexcept {
            if (this != &other) {
                first = move(other.first);
                second = move(other.second);
                other.first = T();
                other.second = U();
            }
            return *this;
        }

        /**
         * @brief 相等比较运算符。
         * 比较两个 `pair` 对象的第一个值和第二个值是否分别相等。
         * @param other 要比较的 `pair` 对象
         * @return 若两个 `pair` 对象相等则返回 `true`，否则返回 `false`
         */
        bool operator==(const pair& other) const {
            return first == other.first && second == other.second;
        }

        /**
         * @brief 不相等比较运算符。
         * 比较两个 `pair` 对象是否不相等。
         * @param other 要比较的 `pair` 对象
         * @return 若两个 `pair` 对象不相等则返回 `true`，否则返回 `false`
         */
        bool operator!=(const pair& other) const {
            return !(*this == other);
        }
    };

    /**
     * @brief 创建一个 `pair` 对象。
     * 该函数使用完美转发，根据传入的参数类型自动推导 `pair` 的类型。
     * @tparam T 第一个值的类型
     * @tparam U 第二个值的类型
     * @param first 第一个值
     * @param second 第二个值
     * @return 一个包含两个值的 `pair` 对象
     */
    template <typename T, typename U>
    pair<T, U> make_pair(T&& first, U&& second) {
        return pair<T, U>(forward<T>(first), forward<U>(second));
    }

}
