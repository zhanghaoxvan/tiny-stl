/**
 * @file shared_ptr.hpp
 * @brief 该文件实现了 `tiny_stl` 命名空间下的 `shared_ptr` 智能指针，用于管理动态分配的对象或数组。
 * `shared_ptr` 是一种引用计数型智能指针，允许多个指针共享同一个对象的所有权，当引用计数降为 0 时，自动释放对象。
 */

#pragma once

#ifdef __GNUC__
#   pragma GCC system_header
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1920
#   pragma system_header
#endif

#include "utility.hpp"
#include <atomic>

namespace tiny_stl {
    
    /**
     * @class control_block
     * @brief 控制块基类，用于管理引用计数。
     * 该类是所有控制块的基类，包含一个原子类型的引用计数，提供了增加、减少引用计数以及获取引用计数的方法。
     */
    class control_block {
    private:
        std::atomic<size_t> ref_count_; /**< 原子类型的引用计数 */

    public:
        /**
         * @brief 构造函数，初始化引用计数为 1。
         */
        control_block() : ref_count_(1) {}

        /**
         * @brief 虚析构函数，确保派生类对象能正确释放。
         */
        virtual ~control_block() = default;
        
        /**
         * @brief 增加引用计数。
         * 使用 `std::memory_order_relaxed` 内存顺序，仅增加引用计数。
         */
        void increment() noexcept {
            ref_count_.fetch_add(1, std::memory_order_relaxed);
        }
        
        /**
         * @brief 减少引用计数并返回减少后的计数。
         * 使用 `std::memory_order_acq_rel` 内存顺序，确保操作的原子性和内存同步。
         * @return 减少后的引用计数。
         */
        size_t decrement() noexcept {
            return ref_count_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        }
        
        /**
         * @brief 获取当前引用计数。
         * 使用 `std::memory_order_acquire` 内存顺序，确保读取到最新的引用计数值。
         * @return 当前引用计数。
         */
        size_t use_count() const noexcept {
            return ref_count_.load(std::memory_order_acquire);
        }
    };

    /**
     * @class object_control_block
     * @brief 对象控制块特化，用于管理单个对象的引用计数。
     * 继承自 `control_block`，负责在引用计数降为 0 时释放单个对象。
     * @tparam T 对象的类型。
     */
    template <typename T>
    class object_control_block : public control_block {
    private:
        T* ptr_; /**< 指向管理对象的指针 */

    public:
        /**
         * @brief 构造函数，初始化管理对象的指针。
         * @param ptr 指向管理对象的指针。
         */
        object_control_block(T* ptr) : ptr_(ptr) {}

        /**
         * @brief 析构函数，释放管理的单个对象。
         */
        ~object_control_block() override {
            delete ptr_;
        }
    };

    /**
     * @class array_control_block
     * @brief 数组控制块特化，用于管理数组的引用计数。
     * 继承自 `control_block`，负责在引用计数降为 0 时释放数组。
     * @tparam T 数组元素的类型。
     */
    template <typename T>
    class array_control_block : public control_block {
    private:
        T* ptr_; /**< 指向管理数组的指针 */

    public:
        /**
         * @brief 构造函数，初始化管理数组的指针。
         * @param ptr 指向管理数组的指针。
         */
        array_control_block(T* ptr) : ptr_(ptr) {}

        /**
         * @brief 析构函数，释放管理的数组。
         */
        ~array_control_block() override {
            delete[] ptr_;
        }
    };

    /**
     * @class shared_ptr
     * @brief 主模板，用于管理单个对象的 `shared_ptr`。
     * 该类实现了引用计数型智能指针，允许多个 `shared_ptr` 共享同一个对象的所有权。
     * @tparam T 管理对象的类型。
     */
    template <typename T>
    class shared_ptr {
    private:
        T* ptr_ = nullptr; /**< 指向管理对象的指针 */
        control_block* ctrl_block_ = nullptr; /**< 指向控制块的指针 */

        /**
         * @brief 创建控制块。
         * 如果 `ptr_` 不为空，创建一个 `object_control_block` 来管理该对象。
         */
        void create_control_block() {
            if (ptr_) {
                ctrl_block_ = new object_control_block<T>(ptr_);
            }
        }

        /**
         * @brief 释放资源。
         * 减少引用计数，如果引用计数降为 0，释放控制块和管理的对象。
         */
        void release() noexcept {
            if (ctrl_block_) {
                if (ctrl_block_->decrement() == 0) {
                    delete ctrl_block_;
                }
                ctrl_block_ = nullptr;
                ptr_ = nullptr;
            }
        }

    public:
        /**
         * @brief 默认构造函数，初始化指针和控制块为空。
         */
        shared_ptr() noexcept = default;
        
        /**
         * @brief 显式构造函数，使用指针初始化 `shared_ptr`。
         * @param ptr 指向管理对象的指针。
         */
        explicit shared_ptr(T* ptr) : ptr_(ptr) {
            create_control_block();
        }
        
        /**
         * @brief 拷贝构造函数，共享同一个对象的所有权。
         * 增加引用计数以确保对象不会被提前释放。
         * @param other 另一个 `shared_ptr` 对象。
         */
        shared_ptr(const shared_ptr& other) noexcept 
            : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
            if (ctrl_block_) {
                ctrl_block_->increment();
            }
        }
        
        /**
         * @brief 移动构造函数，转移对象的所有权。
         * 避免资源的不必要复制。
         * @param other 另一个 `shared_ptr` 对象。
         */
        shared_ptr(shared_ptr&& other) noexcept 
            : ptr_(exchange(other.ptr_, nullptr)), 
            ctrl_block_(exchange(other.ctrl_block_, nullptr)) {}
        
        /**
         * @brief 析构函数，释放资源。
         */
        ~shared_ptr() {
            release();
        }
        
        /**
         * @brief 拷贝赋值运算符，共享同一个对象的所有权。
         * 先释放当前管理的资源，再共享另一个 `shared_ptr` 的资源。
         * @param other 另一个 `shared_ptr` 对象。
         * @return 引用自身。
         */
        shared_ptr& operator=(const shared_ptr& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = other.ptr_;
                ctrl_block_ = other.ctrl_block_;
                if (ctrl_block_) {
                    ctrl_block_->increment();
                }
            }
            return *this;
        }
        
        /**
         * @brief 移动赋值运算符，转移对象的所有权。
         * 先释放当前管理的资源，再转移另一个 `shared_ptr` 的资源。
         * @param other 另一个 `shared_ptr` 对象。
         * @return 引用自身。
         */
        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = exchange(other.ptr_, nullptr);
                ctrl_block_ = exchange(other.ctrl_block_, nullptr);
            }
            return *this;
        }
        
        /**
         * @brief 获取管理对象的指针。
         * @return 指向管理对象的指针。
         */
        T* get() const noexcept { return ptr_; }

        /**
         * @brief 解引用运算符，返回管理对象的引用。
         * @return 管理对象的引用。
         */
        T& operator*() const noexcept { return *ptr_; }

        /**
         * @brief 箭头运算符，返回管理对象的指针。
         * @return 指向管理对象的指针。
         */
        T* operator->() const noexcept { return ptr_; }

        /**
         * @brief 转换为布尔类型，判断 `shared_ptr` 是否有效。
         * @return 如果 `ptr_` 不为空，返回 `true`；否则返回 `false`。
         */
        explicit operator bool() const noexcept { return ptr_ != nullptr; }
        
        /**
         * @brief 获取当前引用计数。
         * @return 当前引用计数，如果控制块为空，返回 0。
         */
        size_t use_count() const noexcept {
            return ctrl_block_ ? ctrl_block_->use_count() : 0;
        }
        
        /**
         * @brief 重置指针，释放当前管理的资源并管理新的对象。
         * @param ptr 指向新管理对象的指针，默认为 `nullptr`。
         */
        void reset(T* ptr = nullptr) {
            release();
            ptr_ = ptr;
            create_control_block();
        }
        
        /**
         * @brief 交换两个 `shared_ptr` 管理的资源。
         * @param other 另一个 `shared_ptr` 对象。
         */
        void swap(shared_ptr& other) noexcept {
            tiny_stl::swap(ptr_, other.ptr_);
            tiny_stl::swap(ctrl_block_, other.ctrl_block_);
        }
    };

    /**
     * @class shared_ptr<T[]>
     * @brief 数组特化版本，用于管理数组的 `shared_ptr`。
     * 该类实现了引用计数型智能指针，允许多个 `shared_ptr` 共享同一个数组的所有权。
     * @tparam T 数组元素的类型。
     */
    template <typename T>
    class shared_ptr<T[]> {
    private:
        T* ptr_ = nullptr; /**< 指向管理数组的指针 */
        control_block* ctrl_block_ = nullptr; /**< 指向控制块的指针 */

        /**
         * @brief 创建控制块。
         * 如果 `ptr_` 不为空，创建一个 `array_control_block` 来管理该数组。
         */
        void create_control_block() {
            if (ptr_) {
                ctrl_block_ = new array_control_block<T>(ptr_);
            }
        }

        /**
         * @brief 释放资源。
         * 减少引用计数，如果引用计数降为 0，释放控制块和管理的数组。
         */
        void release() noexcept {
            if (ctrl_block_) {
                if (ctrl_block_->decrement() == 0) {
                    delete ctrl_block_;
                }
                ctrl_block_ = nullptr;
                ptr_ = nullptr;
            }
        }

    public:
        /**
         * @brief 默认构造函数，初始化指针和控制块为空。
         */
        shared_ptr() noexcept = default;
        
        /**
         * @brief 显式构造函数，使用指针初始化 `shared_ptr`。
         * @param ptr 指向管理数组的指针。
         */
        explicit shared_ptr(T* ptr) : ptr_(ptr) {
            create_control_block();
        }
        
        /**
         * @brief 析构函数，释放资源。
         */
        ~shared_ptr() {
            release();
        }
        
        /**
         * @brief 拷贝构造函数，共享同一个数组的所有权。
         * 增加引用计数以确保数组不会被提前释放。
         * @param other 另一个 `shared_ptr` 对象。
         */
        shared_ptr(const shared_ptr& other) noexcept 
            : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
            if (ctrl_block_) ctrl_block_->increment();
        }
        
        /**
         * @brief 移动构造函数，转移数组的所有权。
         * 避免资源的不必要复制。
         * @param other 另一个 `shared_ptr` 对象。
         */
        shared_ptr(shared_ptr&& other) noexcept 
            : ptr_(exchange(other.ptr_, nullptr)), 
            ctrl_block_(exchange(other.ctrl_block_, nullptr)) {}
        
        /**
         * @brief 拷贝赋值运算符，共享同一个数组的所有权。
         * 先释放当前管理的资源，再共享另一个 `shared_ptr` 的资源。
         * @param other 另一个 `shared_ptr` 对象。
         * @return 引用自身。
         */
        shared_ptr& operator=(const shared_ptr& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = other.ptr_;
                ctrl_block_ = other.ctrl_block_;
                if (ctrl_block_) ctrl_block_->increment();
            }
            return *this;
        }
        
        /**
         * @brief 移动赋值运算符，转移数组的所有权。
         * 先释放当前管理的资源，再转移另一个 `shared_ptr` 的资源。
         * @param other 另一个 `shared_ptr` 对象。
         * @return 引用自身。
         */
        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = exchange(other.ptr_, nullptr);
                ctrl_block_ = exchange(other.ctrl_block_, nullptr);
            }
            return *this;
        }
        
        /**
         * @brief 数组访问运算符，返回指定位置的数组元素引用。
         * @param index 数组元素的索引。
         * @return 指定位置的数组元素引用。
         */
        T& operator[](size_t index) const { return ptr_[index]; }

        /**
         * @brief 获取管理数组的指针。
         * @return 指向管理数组的指针。
         */
        T* get() const noexcept { return ptr_; }

        /**
         * @brief 获取当前引用计数。
         * @return 当前引用计数，如果控制块为空，返回 0。
         */
        size_t use_count() const noexcept {
            return ctrl_block_ ? ctrl_block_->use_count() : 0;
        }

        /**
         * @brief 重置指针，释放当前管理的资源并管理新的数组。
         * @param ptr 指向新管理数组的指针，默认为 `nullptr`。
         */
        void reset(T* ptr = nullptr) {
            release();
            ptr_ = ptr;
            create_control_block();
        }
    };

    /**
     * @brief 创建 `shared_ptr` 的辅助函数，用于管理单个对象。
     * 使用完美转发传递参数，创建一个新的对象并返回一个 `shared_ptr` 管理该对象。
     * @tparam T 管理对象的类型。
     * @tparam Args 构造函数参数的类型包。
     * @param args 构造函数参数。
     * @return 管理新对象的 `shared_ptr`。
     */
    template <typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T>(new T(forward<Args>(args)...));
    }

    /**
     * @brief 创建 `shared_ptr` 的辅助函数，用于管理数组。
     * 创建一个指定大小的数组并返回一个 `shared_ptr` 管理该数组。
     * @tparam T 数组元素的类型。
     * @param size 数组的大小。
     * @return 管理新数组的 `shared_ptr`。
     */
    template <typename T>
    shared_ptr<T[]> make_shared(size_t size) {
        return shared_ptr<T[]>(new T[size]());
    }

}
