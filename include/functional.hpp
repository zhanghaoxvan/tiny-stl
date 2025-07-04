/**
 * @file functional.hpp
 */

#pragma once
#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

namespace tiny_stl {
    template <typename Arg, typename Result>
    struct unary_function {
        using argument_type = Arg;
        using result_type   = Result;
    };
    template <typename Arg1, typename Arg2, typename Result>
    struct binary_function {
        using first_argument_type  = Arg1;
        using second_argument_type = Arg2;
        using result_type          = Result;
    };
    template <typename T>
    struct plus : public binary_function<T, T, T> {
        T operator()(const T& x, const T& y) const {
            return x + y;
        }
    };
    template <typename T>
    struct minus : public binary_function<T, T, T> {
        T operator()(const T& x, const T& y) const {
            return x - y;
        }
    };
    template <typename T>
    struct multiplies : public binary_function<T, T, T> {
        T operator()(const T& x, const T& y) const {
            return x * y;
        }
    };
    template <typename T>
    struct divides : public binary_function<T, T, T> {
        T operator()(const T& x, const T& y) const {
            return x / y;
        }
    };
    template <typename T>
    struct modulus : public binary_function<T, T, T> {
        T operator()(const T& x, const T& y) const {
            return x % y;
        }
    };
    template <typename T>
    struct negate : public unary_function<T, T> {
        T operator()(const T& x) const {
            return -x;
        }
    };
    template <typename T>
    struct equal : public binary_function<T, T, bool> {
        bool operator()(const T& x, const T& y) const {
            return x == y;
        }
    };
    template <typename T>
    struct greater : public binary_function<T, T, bool> {
        bool operator()(const T& x, const T& y) const {
            return x > y;
        }
    };
    template <typename T>
    struct greater_equal : public binary_function<T, T, bool> {
        bool operator()(const T& x, const T& y) const {
            return x >= y;
        }
    };
    template <typename T>
    struct less : public binary_function<T, T, bool> {
        bool operator()(const T& x, const T& y) const {
            return x < y;
        }
    };
    template <typename T>
    struct less_equal : public binary_function<T, T, bool> {
        bool operator()(const T& x, const T& y) const {
            return x <= y;
        }
    };
}
