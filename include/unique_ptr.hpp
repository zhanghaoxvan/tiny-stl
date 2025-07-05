/**
 * @file unique_ptr.hpp
 * @brief 此文件定义了 `tiny_stl` 命名空间下的 `unique_ptr` 类模板，用于实现独占式智能指针。
 * 它提供了对动态分配对象的自动内存管理，确保对象在不再使用时被正确释放。
 */

#pragma once
#include "utility.hpp"  // 自定义工具函数

namespace tiny_stl {
    
    /**
     * @class unique_ptr
     * @brief 独占式智能指针，确保同一时间只有一个 `unique_ptr` 实例可以拥有该指针。
     * 它提供了自动内存管理，避免了手动内存管理的复杂性和潜在的内存泄漏问题。
     * @tparam T 指向的对象类型
     */
    template <typename T>
    class unique_ptr {
    private:
        /**
         * @brief 指向管理的对象的原始指针
         */
        T* ptr_ = nullptr;

    public:
        /**
         * @brief 构造函数，接受一个原始指针并管理它。
         * @param ptr 要管理的原始指针，默认为 `nullptr`
         * @note 该构造函数被声明为 `explicit`，以防止隐式转换。
         */
        explicit unique_ptr(T* ptr = nullptr) noexcept : ptr_(ptr) {}
        
        /**
         * @brief 禁止拷贝构造函数，确保 `unique_ptr` 的独占性。
         */
        unique_ptr(const unique_ptr&) = delete;
        
        /**
         * @brief 禁止拷贝赋值运算符，确保 `unique_ptr` 的独占性。
         */
        unique_ptr& operator=(const unique_ptr&) = delete;
        
        /**
         * @brief 移动构造函数，从另一个 `unique_ptr` 实例中转移所有权。
         * @param other 要转移所有权的 `unique_ptr` 实例
         */
        unique_ptr(unique_ptr&& other) noexcept 
            : ptr_(other.release()) {}
        
        /**
         * @brief 移动赋值运算符，从另一个 `unique_ptr` 实例中转移所有权。
         * @param other 要转移所有权的 `unique_ptr` 实例
         * @return 指向当前 `unique_ptr` 实例的引用
         */
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            reset(other.release());
            return *this;
        }
        
        /**
         * @brief 析构函数，释放管理的对象。
         */
        ~unique_ptr() {
            reset();
        }
        
        /**
         * @brief 释放当前 `unique_ptr` 对对象的所有权，并返回原始指针。
         * @return 指向管理对象的原始指针
         */
        T* release() noexcept {
            return exchange(ptr_, nullptr);
        }
        
        /**
         * @brief 重置 `unique_ptr` 管理的指针。
         * 若存在旧指针，则释放旧指针指向的对象，并更新为新指针。
         * @param new_ptr 新的要管理的指针，默认为 `nullptr`
         */
        void reset(T* new_ptr = nullptr) noexcept {
            T* old_ptr = exchange(ptr_, new_ptr);
            if (old_ptr) {
                delete old_ptr;
            }
        }
        
        /**
         * @brief 交换两个 `unique_ptr` 实例管理的指针。
         * @param other 要交换的 `unique_ptr` 实例
         */
        void swap(unique_ptr& other) noexcept {
            tiny_stl::swap(ptr_, other.ptr_);
        }
        
        /**
         * @brief 获取管理的原始指针。
         * @return 指向管理对象的原始指针
         */
        T* get() const noexcept { return ptr_; }
        
        /**
         * @brief 转换为布尔值，用于检查 `unique_ptr` 是否持有有效指针。
         * @return 若 `unique_ptr` 持有有效指针则返回 `true`，否则返回 `false`
         */
        explicit operator bool() const noexcept { return ptr_ != nullptr; }
        
        /**
         * @brief 解引用操作符，返回管理对象的引用。
         * @return 管理对象的引用
         */
        T& operator*() const noexcept { return *ptr_; }
        
        /**
         * @brief 箭头操作符，返回管理对象的指针。
         * @return 指向管理对象的指针
         */
        T* operator->() const noexcept { return ptr_; }
    };

    /**
     * @class unique_ptr<T[]>
     * @brief `unique_ptr` 的数组特化版本，用于管理动态分配的数组。
     * @tparam T 数组元素的类型
     */
    template <typename T>
    class unique_ptr<T[]> {
    private:
        /**
         * @brief 指向管理的数组的原始指针
         */
        T* ptr_ = nullptr;

    public:
        /**
         * @brief 构造函数，接受一个指向数组的原始指针并管理它。
         * @param ptr 要管理的指向数组的原始指针，默认为 `nullptr`
         */
        explicit unique_ptr(T* ptr = nullptr) noexcept : ptr_(ptr) {}
        
        /**
         * @brief 析构函数，释放管理的数组。
         */
        ~unique_ptr() { reset(); }
        
        /**
         * @brief 禁止拷贝构造函数，确保 `unique_ptr` 的独占性。
         */
        unique_ptr(const unique_ptr&) = delete;
        
        /**
         * @brief 移动构造函数，从另一个 `unique_ptr` 实例中转移所有权。
         * @param other 要转移所有权的 `unique_ptr` 实例
         */
        unique_ptr(unique_ptr&& other) noexcept : ptr_(other.release()) {}
        
        /**
         * @brief 禁止拷贝赋值运算符，确保 `unique_ptr` 的独占性。
         */
        unique_ptr& operator=(const unique_ptr&) = delete;
        
        /**
         * @brief 移动赋值运算符，从另一个 `unique_ptr` 实例中转移所有权。
         * @param other 要转移所有权的 `unique_ptr` 实例
         * @return 指向当前 `unique_ptr` 实例的引用
         */
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            reset(other.release());
            return *this;
        }
        
        /**
         * @brief 释放当前 `unique_ptr` 对数组的所有权，并返回原始指针。
         * @return 指向管理数组的原始指针
         */
        T* release() noexcept { return exchange(ptr_, nullptr); }
        
        /**
         * @brief 重置 `unique_ptr` 管理的数组指针。
         * 若存在旧指针，则释放旧指针指向的数组，并更新为新指针。
         * @param new_ptr 新的要管理的数组指针，默认为 `nullptr`
         */
        void reset(T* new_ptr = nullptr) noexcept {
            T* old_ptr = exchange(ptr_, new_ptr);
            if (old_ptr) {
                delete[] old_ptr;
            }
        }
        
        /**
         * @brief 下标操作符，返回数组中指定位置的元素的引用。
         * @param index 数组元素的索引
         * @return 数组中指定位置元素的引用
         */
        T& operator[](size_t index) const { return ptr_[index]; }
        
        /**
         * @brief 获取管理的原始数组指针。
         * @return 指向管理数组的原始指针
         */
        T* get() const noexcept { return ptr_; }
        
        /**
         * @brief 转换为布尔值，用于检查 `unique_ptr` 是否持有有效数组指针。
         * @return 若 `unique_ptr` 持有有效数组指针则返回 `true`，否则返回 `false`
         */
        explicit operator bool() const noexcept { return ptr_ != nullptr; }
    };

    /**
     * @brief 辅助函数，用于创建 `unique_ptr` 实例。
     * 该函数使用 `new` 运算符在堆上分配一个新对象，并返回一个管理该对象的 `unique_ptr` 实例。
     * @tparam T 要创建对象的类型
     * @tparam Args 构造函数参数的类型包
     * @param args 传递给对象构造函数的参数
     * @return 管理新创建对象的 `unique_ptr` 实例
     */
    template <typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args) {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }

    /**
     * @brief 辅助函数，用于创建管理动态数组的 `unique_ptr` 实例。
     * 该函数使用 `new[]` 运算符在堆上分配一个新数组，并返回一个管理该数组的 `unique_ptr` 实例。
     * @tparam T 数组元素的类型
     * @param size 数组的大小
     * @return 管理新创建数组的 `unique_ptr` 实例
     */
    template <typename T>
    unique_ptr<T[]> make_unique(size_t size) {
        return unique_ptr<T[]>(new T[size]());
    }

}
