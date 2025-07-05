/**
 * @file allocator.hpp
 * @brief 此文件定义了一个简单的内存分配器类 `allocator`，用于管理内存的分配、释放、对象的构造和销毁。
 * 
 * 该分配器实现了标准库 `std::allocator` 的基本功能，可用于为自定义容器或数据结构提供内存管理。
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include <cstdint>
#include <cstddef>
#include <new>
#include <cassert>

namespace tiny_stl {

    /**
     * @class allocator
     * @brief 一个简单的内存分配器，用于管理特定类型对象的内存分配和释放。
     *
     * 这个分配器提供了基本的内存管理功能，包括分配、释放内存，以及对象的构造和销毁。
     * 它是无状态的，因此所有相同类型的分配器实例都被认为是相等的。
     *
     * @tparam T 分配器管理的对象类型。
     */
    template <typename T>
    class allocator {
    public:
        /**
         * @brief 分配器管理的对象类型。
         */
        using value_type      = T;
        /**
         * @brief 表示分配器可以分配的对象数量的类型。
         */
        using size_type       = size_t;
        /**
         * @brief 指向 `value_type` 的指针类型。
         */
        using pointer         = T*;
        /**
         * @brief 指向常量 `value_type` 的指针类型。
         */
        using const_pointer   = const T*;
        /**
         * @brief `value_type` 的引用类型。
         */
        using reference       = T&;
        /**
         * @brief 常量 `value_type` 的引用类型。
         */
        using const_reference = const T&;
        /**
         * @brief 表示两个指针之间差值的类型。
         */
        using difference_type = ptrdiff_t;

    public:
        
        template <class U>
        struct rebind {
            using other = allocator<U>;
        };

    public:
        /**
         * @brief 默认构造函数。
         *
         * 构造一个新的 `allocator` 实例。由于该分配器是无状态的，因此此构造函数不执行任何操作。
         */
        allocator() noexcept = default;

        /**
         * @brief 拷贝构造函数。
         *
         * 构造一个新的 `allocator` 实例，该实例是另一个 `allocator` 实例的副本。
         * 由于该分配器是无状态的，因此此构造函数不执行任何操作。
         *
         * @param other 要复制的 `allocator` 实例。
         */
        allocator(const allocator&) noexcept = default;

        /**
         * @brief 模板拷贝构造函数。
         *
         * 构造一个新的 `allocator` 实例，该实例是另一个不同类型 `allocator` 实例的副本。
         * 由于该分配器是无状态的，因此此构造函数不执行任何操作。
         *
         * @tparam U 另一个 `allocator` 实例管理的对象类型。
         * @param other 要复制的 `allocator<U>` 实例。
         */
        template <class U>
        allocator(const allocator<U>&) noexcept {}

        /**
         * @brief 析构函数。
         *
         * 销毁 `allocator` 实例。由于该分配器是无状态的，因此此析构函数不执行任何操作。
         */
        ~allocator() = default;

    public:
        /**
         * @brief 获取对象的地址。
         *
         * 返回给定对象的地址。
         *
         * @param x 要获取地址的对象。
         * @return 指向对象的指针。
         */
        pointer address(reference x) const noexcept {
            return &x;
        }

        /**
         * @brief 获取常量对象的地址。
         *
         * 返回给定常量对象的地址。
         *
         * @param x 要获取地址的常量对象。
         * @return 指向常量对象的指针。
         */
        const_pointer address(const_reference x) const noexcept {
            return &x;
        }

        /**
         * @brief 分配未初始化的内存。
         *
         * 分配足够的内存来存储 `n` 个 `value_type` 对象，但不构造这些对象。
         *
         * @param n 要分配的对象数量。
         * @return 指向分配的内存的指针，如果 `n` 为 0，则返回 `nullptr`。
         * @throws std::bad_alloc 如果内存分配失败。
         */
        pointer allocate(size_type n) {
            if (n == 0) {
                return nullptr;
            }
            return static_cast<pointer>(::operator new(n * sizeof(value_type)));
        }

        /**
         * @brief 释放已分配的内存。
         *
         * 释放之前通过 `allocate` 分配的内存。
         *
         * @param p 指向要释放的内存的指针。
         * @param n 之前分配的对象数量。
         * @pre `p` 必须是通过 `allocate` 分配的指针，并且 `n` 必须与分配时的数量相同。
         */
        void deallocate(pointer p, size_type n) {
            assert(p != nullptr && n > 0);
            // 释放内存
            // 注意：这里的 p 是通过 allocate 分配的内存
            //       因此需要使用 delete 释放
            ::operator delete(p);
        }

        /**
         * @brief 在已分配的内存上构造对象。
         *
         * 在给定的内存位置构造一个 `value_type` 对象。
         *
         * @param p 指向已分配内存的指针。
         * @param n 用于构造对象的值。
         * @return 指向构造好的对象的指针。
         * @pre `p` 必须指向足够大的未初始化内存。
         */
        pointer construct(pointer p, value_type n) {
            assert(p != nullptr);
            // 使用 placement new 构造对象
            return new (p) value_type(n);
        }

        /**
         * @brief 销毁已构造的对象。
         *
         * 调用给定对象的析构函数，但不释放对象占用的内存。
         *
         * @param p 指向要销毁的对象的指针。
         * @pre `p` 必须指向一个已构造的对象。
         */
        void destroy(pointer p) {
            assert(p != nullptr);
            // 调用析构函数
            p->~value_type();
        }

        /**
         * @brief 获取分配器可以分配的最大对象数量。
         *
         * 返回分配器可以一次分配的最大 `value_type` 对象数量。
         *
         * @return 分配器可以分配的最大对象数量。
         */
        size_type max_size() const noexcept {
            return size_type(-1) / sizeof(value_type);
        }
    };

    /**
     * @brief 判断两个分配器是否相等。
     *
     * 由于 `allocator` 是无状态的，因此所有相同类型的分配器实例都被认为是相等的。
     *
     * @tparam T1 第一个分配器管理的对象类型。
     * @tparam T2 第二个分配器管理的对象类型。
     * @param lhs 第一个分配器实例。
     * @param rhs 第二个分配器实例。
     * @return 总是返回 `true`。
     */
    template <class T1, class T2>
    bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept {
        return true; // 因为是无状态的，所以总是相等
    }

    /**
     * @brief 判断两个分配器是否不相等。
     *
     * 由于 `allocator` 是无状态的，因此所有相同类型的分配器实例都被认为是相等的。
     *
     * @tparam T1 第一个分配器管理的对象类型。
     * @tparam T2 第二个分配器管理的对象类型。
     * @param lhs 第一个分配器实例。
     * @param rhs 第二个分配器实例。
     * @return 总是返回 `false`。
     */
    template <class T1, class T2>
    bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept {
        return false;
    }

}
