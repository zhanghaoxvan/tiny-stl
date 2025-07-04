/**
 * @file memory.hpp
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include <allocator.hpp>
#include <shared_ptr.hpp>
#include <unique_ptr.hpp>
#include <utility.hpp> // 包含 remove_reference, forward, move, swap 等
#include <new>        // 包含 placement new 和 bad_alloc
#include <cstddef>    // 包含 size_t, ptrdiff_t
#include <iterator.hpp>   // 包含 iterator_traits

namespace tiny_stl {

    // ==================== 基础工具 ====================

    /**
     * @brief 在给定地址构造对象
     * @param p 对象构造地址
     * @param args 构造函数参数
     * @return 指向构造对象的指针
     */
    template <typename T, typename... Args>
    T* construct_at(T* p, Args&&... args) {
        return ::new (static_cast<void*>(p)) T(tiny_stl::forward<Args>(args)...);
    }

    /**
     * @brief 销毁给定地址的对象
     * @param p 对象指针
     */
    template <typename T>
    void destroy_at(T* p) {
        if (p != nullptr) {
            p->~T();
        }
    }

    // ==================== 范围操作 ====================

    /**
     * @brief 销毁对象范围
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <typename ForwardIt>
    void destroy(ForwardIt first, ForwardIt last) {
        for (; first != last; ++first) {
            destroy_at(&*first);
        }
    }

    /**
     * @brief 销毁范围内的多个对象
     * @param first 起始迭代器
     * @param n 要销毁的对象数量
     * @return 最后销毁元素的下一个位置
     */
    template <typename ForwardIt>
    ForwardIt destroy_n(ForwardIt first, size_t n) {
        for (size_t i = 0; i < n; ++i, ++first) {
            destroy_at(&*first);
        }
        return first;
    }

    // ==================== 复制操作 ====================

    /**
     * @brief 将对象范围复制到未初始化的内存区域
     * @param first 输入范围的起始
     * @param last 输入范围的结束
     * @param d_first 目标范围的起始
     * @return 目标范围中最后复制元素的下一个位置的迭代器
     */
    template <typename InputIt, typename ForwardIt>
    ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) {
        ForwardIt current = d_first;
        try {
            for (; first != last; ++first, (void)++current) {
                construct_at(&*current, *first);
            }
            return current;
        } catch (...) {
            destroy(d_first, current);
            throw;
        }
    }

    /**
     * @brief 将多个对象复制到未初始化的内存区域
     * @param first 输入范围的起始
     * @param count 要复制的元素数量
     * @param d_first 目标范围的起始
     * @return 目标范围中最后复制元素的下一个位置的迭代器
     */
    template <typename InputIt, typename Size, typename ForwardIt>
    ForwardIt uninitialized_copy_n(InputIt first, Size count, ForwardIt d_first) {
        ForwardIt current = d_first;
        try {
            for (Size i = 0; i < count; ++i, ++first, (void)++current) {
                construct_at(&*current, *first);
            }
            return current;
        } catch (...) {
            destroy(d_first, current);
            throw;
        }
    }

    // ==================== 填充操作 ====================

    /**
     * @brief 将一个对象复制到未初始化的内存区域，由范围定义
     * @param first 起始迭代器
     * @param last 结束迭代器
     * @param value 要填充的值
     */
    template <typename ForwardIt, typename T>
    void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
        ForwardIt current = first;
        try {
            for (; current != last; ++current) {
                construct_at(&*current, value);
            }
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    /**
     * @brief 将一个对象复制到未初始化的内存区域，由起始位置和计数定义
     * @param first 起始迭代器
     * @param count 元素数量
     * @param value 要填充的值
     * @return 最后填充元素的下一个位置
     */
    template <typename ForwardIt, typename Size, typename T>
    ForwardIt uninitialized_fill_n(ForwardIt first, Size count, const T& value) {
        ForwardIt current = first;
        try {
            for (Size i = 0; i < count; ++i, ++current) {
                construct_at(&*current, value);
            }
            return current;
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    // ==================== 移动操作 ====================

    /**
     * @brief 将对象范围移动到未初始化的内存区域
     * @param first 输入范围的起始
     * @param last 输入范围的结束
     * @param d_first 目标范围的起始
     * @return 目标范围中最后移动元素的下一个位置的迭代器
     */
    template <typename InputIt, typename ForwardIt>
    ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) {
        ForwardIt current = d_first;
        try {
            for (; first != last; ++first, (void)++current) {
                construct_at(&*current, tiny_stl::move(*first));
            }
            return current;
        } catch (...) {
            destroy(d_first, current);
            throw;
        }
    }

    /**
     * @brief 将多个对象移动到未初始化的内存区域
     * @param first 输入范围的起始
     * @param count 要移动的元素数量
     * @param d_first 目标范围的起始
     * @return 目标范围中最后移动元素的下一个位置的迭代器
     */
    template <typename InputIt, typename Size, typename ForwardIt>
    ForwardIt uninitialized_move_n(InputIt first, Size count, ForwardIt d_first) {
        ForwardIt current = d_first;
        try {
            for (Size i = 0; i < count; ++i, ++first, (void)++current) {
                construct_at(&*current, tiny_stl::move(*first));
            }
            return current;
        } catch (...) {
            destroy(d_first, current);
            throw;
        }
    }

    // ==================== 默认构造 ====================

    /**
     * @brief 通过默认初始化在未初始化的内存区域中构造对象，由范围定义
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <typename ForwardIt>
    void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
        using ValueType = typename tiny_stl::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (; current != last; ++current) {
                ::new (static_cast<void*>(&*current)) ValueType;
            }
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    /**
     * @brief 通过默认初始化在未初始化的内存区域中构造对象，由起始位置和计数定义
     * @param first 起始迭代器
     * @param n 元素数量
     * @return 最后构造元素的下一个位置
     */
    template <typename ForwardIt>
    ForwardIt uninitialized_default_construct_n(ForwardIt first, size_t n) {
        using ValueType = typename tiny_stl::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (size_t i = 0; i < n; ++i, ++current) {
                ::new (static_cast<void*>(&*current)) ValueType;
            }
            return current;
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    // ==================== 值构造 ====================

    /**
     * @brief 通过值初始化在未初始化的内存区域中构造对象，由范围定义
     * @param first 起始迭代器
     * @param last 结束迭代器
     */
    template <typename ForwardIt>
    void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
        using ValueType = typename tiny_stl::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (; current != last; ++current) {
                ::new (static_cast<void*>(&*current)) ValueType();
            }
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

    /**
     * @brief 通过值初始化在未初始化的内存区域中构造对象，由起始位置和计数定义
     * @param first 起始迭代器
     * @param n 元素数量
     * @return 最后构造元素的下一个位置
     */
    template <typename ForwardIt>
    ForwardIt uninitialized_value_construct_n(ForwardIt first, size_t n) {
        using ValueType = typename tiny_stl::iterator_traits<ForwardIt>::value_type;
        ForwardIt current = first;
        try {
            for (size_t i = 0; i < n; ++i, ++current) {
                ::new (static_cast<void*>(&*current)) ValueType();
            }
            return current;
        } catch (...) {
            destroy(first, current);
            throw;
        }
    }

} // namespace tiny_stl
