/**
 * @file functional.hpp
 * @brief 此文件定义了一系列函数对象，用于实现常见的算术、比较等操作，类似于标准库 `<functional>` 头文件中的部分功能。
 * 
 * 这些函数对象可以作为模板参数传递给算法或容器，以实现自定义的操作逻辑。
 */

#pragma once
#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

namespace tiny_stl {

    /**
     * @class unary_function
     * @brief 一元函数的基类模板，用于定义一元函数对象的参数类型和返回值类型。
     * 
     * 该类模板提供了两个类型别名 `argument_type` 和 `result_type`，分别表示一元函数的参数类型和返回值类型。
     * 其他一元函数对象可以继承自该类，以统一接口。
     * 
     * @tparam Arg 一元函数的参数类型。
     * @tparam Result 一元函数的返回值类型。
     */
    template <typename Arg, typename Result>
    struct unary_function {
        /**
         * @brief 一元函数的参数类型。
         */
        using argument_type = Arg;
        /**
         * @brief 一元函数的返回值类型。
         */
        using result_type   = Result;
    };

    /**
     * @class binary_function
     * @brief 二元函数的基类模板，用于定义二元函数对象的参数类型和返回值类型。
     * 
     * 该类模板提供了三个类型别名 `first_argument_type`、`second_argument_type` 和 `result_type`，
     * 分别表示二元函数的第一个参数类型、第二个参数类型和返回值类型。
     * 其他二元函数对象可以继承自该类，以统一接口。
     * 
     * @tparam Arg1 二元函数的第一个参数类型。
     * @tparam Arg2 二元函数的第二个参数类型。
     * @tparam Result 二元函数的返回值类型。
     */
    template <typename Arg1, typename Arg2, typename Result>
    struct binary_function {
        /**
         * @brief 二元函数的第一个参数类型。
         */
        using first_argument_type  = Arg1;
        /**
         * @brief 二元函数的第二个参数类型。
         */
        using second_argument_type = Arg2;
        /**
         * @brief 二元函数的返回值类型。
         */
        using result_type          = Result;
    };

    /**
     * @class plus
     * @brief 二元函数对象，用于实现加法操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的加法。
     * 
     * @tparam T 加法操作的操作数类型。
     */
    template <typename T>
    struct plus : public binary_function<T, T, T> {
        /**
         * @brief 执行加法操作。
         * 
         * @param x 第一个操作数。
         * @param y 第二个操作数。
         * @return 两个操作数相加的结果。
         */
        T operator()(const T& x, const T& y) const {
            return x + y;
        }
    };

    /**
     * @class minus
     * @brief 二元函数对象，用于实现减法操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的减法。
     * 
     * @tparam T 减法操作的操作数类型。
     */
    template <typename T>
    struct minus : public binary_function<T, T, T> {
        /**
         * @brief 执行减法操作。
         * 
         * @param x 被减数。
         * @param y 减数。
         * @return 两个操作数相减的结果。
         */
        T operator()(const T& x, const T& y) const {
            return x - y;
        }
    };

    /**
     * @class multiplies
     * @brief 二元函数对象，用于实现乘法操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的乘法。
     * 
     * @tparam T 乘法操作的操作数类型。
     */
    template <typename T>
    struct multiplies : public binary_function<T, T, T> {
        /**
         * @brief 执行乘法操作。
         * 
         * @param x 第一个操作数。
         * @param y 第二个操作数。
         * @return 两个操作数相乘的结果。
         */
        T operator()(const T& x, const T& y) const {
            return x * y;
        }
    };

    /**
     * @class divides
     * @brief 二元函数对象，用于实现除法操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的除法。
     * 
     * @tparam T 除法操作的操作数类型。
     */
    template <typename T>
    struct divides : public binary_function<T, T, T> {
        /**
         * @brief 执行除法操作。
         * 
         * @param x 被除数。
         * @param y 除数。
         * @return 两个操作数相除的结果。
         * @pre `y` 不能为零，否则可能导致未定义行为。
         */
        T operator()(const T& x, const T& y) const {
            return x / y;
        }
    };

    /**
     * @class modulus
     * @brief 二元函数对象，用于实现取模操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的取模。
     * 
     * @tparam T 取模操作的操作数类型。
     */
    template <typename T>
    struct modulus : public binary_function<T, T, T> {
        /**
         * @brief 执行取模操作。
         * 
         * @param x 被除数。
         * @param y 除数。
         * @return 两个操作数取模的结果。
         * @pre `y` 不能为零，否则可能导致未定义行为。
         */
        T operator()(const T& x, const T& y) const {
            return x % y;
        }
    };

    /**
     * @class negate
     * @brief 一元函数对象，用于实现取负操作。
     * 
     * 该类继承自 `unary_function`，并重载了 `operator()` 以实现对一个对象的取负。
     * 
     * @tparam T 取负操作的操作数类型。
     */
    template <typename T>
    struct negate : public unary_function<T, T> {
        /**
         * @brief 执行取负操作。
         * 
         * @param x 操作数。
         * @return 操作数取负的结果。
         */
        T operator()(const T& x) const {
            return -x;
        }
    };

    /**
     * @class equal
     * @brief 二元函数对象，用于实现相等比较操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的相等比较。
     * 
     * @tparam T 比较操作的操作数类型。
     */
    template <typename T>
    struct equal : public binary_function<T, T, bool> {
        /**
         * @brief 执行相等比较操作。
         * 
         * @param x 第一个操作数。
         * @param y 第二个操作数。
         * @return 如果两个操作数相等，返回 `true`；否则返回 `false`。
         */
        bool operator()(const T& x, const T& y) const {
            return x == y;
        }
    };

    /**
     * @class greater
     * @brief 二元函数对象，用于实现大于比较操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的大于比较。
     * 
     * @tparam T 比较操作的操作数类型。
     */
    template <typename T>
    struct greater : public binary_function<T, T, bool> {
        /**
         * @brief 执行大于比较操作。
         * 
         * @param x 第一个操作数。
         * @param y 第二个操作数。
         * @return 如果 `x` 大于 `y`，返回 `true`；否则返回 `false`。
         */
        bool operator()(const T& x, const T& y) const {
            return x > y;
        }
    };

    /**
     * @class greater_equal
     * @brief 二元函数对象，用于实现大于等于比较操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的大于等于比较。
     * 
     * @tparam T 比较操作的操作数类型。
     */
    template <typename T>
    struct greater_equal : public binary_function<T, T, bool> {
        /**
         * @brief 执行大于等于比较操作。
         * 
         * @param x 第一个操作数。
         * @param y 第二个操作数。
         * @return 如果 `x` 大于等于 `y`，返回 `true`；否则返回 `false`。
         */
        bool operator()(const T& x, const T& y) const {
            return x >= y;
        }
    };

    /**
     * @class less
     * @brief 二元函数对象，用于实现小于比较操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的小于比较。
     * 
     * @tparam T 比较操作的操作数类型。
     */
    template <typename T>
    struct less : public binary_function<T, T, bool> {
        /**
         * @brief 执行小于比较操作。
         * 
         * @param x 第一个操作数。
         * @param y 第二个操作数。
         * @return 如果 `x` 小于 `y`，返回 `true`；否则返回 `false`。
         */
        bool operator()(const T& x, const T& y) const {
            return x < y;
        }
    };

    /**
     * @class less_equal
     * @brief 二元函数对象，用于实现小于等于比较操作。
     * 
     * 该类继承自 `binary_function`，并重载了 `operator()` 以实现两个相同类型对象的小于等于比较。
     * 
     * @tparam T 比较操作的操作数类型。
     */
    template <typename T>
    struct less_equal : public binary_function<T, T, bool> {
        /**
         * @brief 执行小于等于比较操作。
         * 
         * @param x 第一个操作数。
         * @param y 第二个操作数。
         * @return 如果 `x` 小于等于 `y`，返回 `true`；否则返回 `false`。
         */
        bool operator()(const T& x, const T& y) const {
            return x <= y;
        }
    };
}
