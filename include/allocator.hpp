/**
 * @file allocator.hpp
 * 
 * 空间配置器
 * 
 * 文档见：https://cppreference.cn/w/cpp/memory/allocator
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <new>
#include <cassert>

namespace tiny_stl {

    /**
     * @class allocator
     * @brief 内存分配器
     */
    template <typename T>
    class allocator {
    public:
        using value_type      = T;
        using size_type       = size_t;
        using pointer         = T*;
        using const_pointer   = const T*;
        using reference       = T&;
        using const_reference = const T&;
        using difference_type = ptrdiff_t;
    public:
        allocator() noexcept = default;
        allocator(const allocator&) noexcept = default;
        template <class U>
        allocator(const allocator<U>&) noexcept {}
        ~allocator() = default;
    public:
        /**
         * @brief 获得 @c x 的地址
         * @param x 值
         * @return @c x 的地址
         */
        pointer address(reference x) const noexcept {
            return &x;
        }
        /**
         * @brief 获得 @c x 的地址
         * @param x 值
         * @return @c x 的地址
         */
        const_pointer address(const_reference x) const noexcept {
            return &x;
        }
        /**
         * @brief 分配 @c n 个大小的未初始化空间
         * @param n 空间大小
         * @return 指向空间的指针
         */
        pointer allocate(size_type n) {
            if (n == 0) {
                return nullptr;
            }
            return static_cast<pointer>(::operator new(n * sizeof(value_type)));
        }
        /**
         * @brief 释放 @c p 所在的 @c n 个大小的存储空间
         * @param p p
         * @param n n
         */
        void deallocate(const_pointer p, size_type n) {
            assert(p != nullptr && n > 0);
            // 释放内存
            // 注意：这里的 p 是通过 allocate 分配的内存
            //       因此需要使用 delete 释放
            ::operator delete(p);
        }
        /**
         * @brief 构造 @c p 所指向的对象
         * @param p 指向对象的指针
         * @param n 对象
         * @return 指向对象的指针
         */
        pointer construct(pointer p, value_type n) {
            assert(p != nullptr);
            // 使用 placement new 构造对象
            return new (p) value_type(n);
        }
        /**
         * @brief 销毁 @c p 所指向的对象
         * @param p 指向对象的指针
         */
        void destroy(pointer p) {
            assert(p != nullptr);
            // 调用析构函数
            p->~value_type();
        }
        /**
         * @brief 返回分配器可以分配的最大大小
         * @return 分配器可以分配的最大大小
         */
        size_type max_size() const noexcept {
            return size_type(-1) / sizeof(value_type);
        }
    };

    template <class T1, class T2>
    bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept {
        return true; // 因为是无状态的，所以总是相等
    }
    template <class T1, class T2>
    bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept {
        return false;
    }

}
