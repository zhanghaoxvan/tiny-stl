/**
 * @file shared_ptr.hpp
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
    
    // 控制块基类
    class control_block {
    private:
        std::atomic<size_t> ref_count_;

    public:
        control_block() : ref_count_(1) {}
        virtual ~control_block() = default;
        
        void increment() noexcept {
            ref_count_.fetch_add(1, std::memory_order_relaxed);
        }
        
        size_t decrement() noexcept {
            return ref_count_.fetch_sub(1, std::memory_order_acq_rel) - 1;
        }
        
        size_t use_count() const noexcept {
            return ref_count_.load(std::memory_order_acquire);
        }
    };

    // 对象控制块特化
    template <typename T>
    class object_control_block : public control_block {
    private:
        T* ptr_;

    public:
        object_control_block(T* ptr) : ptr_(ptr) {}
        ~object_control_block() override {
            delete ptr_;
        }
    };

    // 数组控制块特化
    template <typename T>
    class array_control_block : public control_block {
    private:
        T* ptr_;

    public:
        array_control_block(T* ptr) : ptr_(ptr) {}
        ~array_control_block() override {
            delete[] ptr_;
        }
    };

    // shared_ptr 主模板
    template <typename T>
    class shared_ptr {
    private:
        T* ptr_ = nullptr;
        control_block* ctrl_block_ = nullptr;

        void create_control_block() {
            if (ptr_) {
                ctrl_block_ = new object_control_block<T>(ptr_);
            }
        }

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
        shared_ptr() noexcept = default;
        
        explicit shared_ptr(T* ptr) : ptr_(ptr) {
            create_control_block();
        }
        
        // 拷贝构造函数
        shared_ptr(const shared_ptr& other) noexcept 
            : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
            if (ctrl_block_) {
                ctrl_block_->increment();
            }
        }
        
        // 移动构造函数
        shared_ptr(shared_ptr&& other) noexcept 
            : ptr_(exchange(other.ptr_, nullptr)), 
            ctrl_block_(exchange(other.ctrl_block_, nullptr)) {}
        
        // 析构函数
        ~shared_ptr() {
            release();
        }
        
        // 拷贝赋值
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
        
        // 移动赋值
        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = exchange(other.ptr_, nullptr);
                ctrl_block_ = exchange(other.ctrl_block_, nullptr);
            }
            return *this;
        }
        
        // 访问方法
        T* get() const noexcept { return ptr_; }
        T& operator*() const noexcept { return *ptr_; }
        T* operator->() const noexcept { return ptr_; }
        explicit operator bool() const noexcept { return ptr_ != nullptr; }
        
        // 引用计数
        size_t use_count() const noexcept {
            return ctrl_block_ ? ctrl_block_->use_count() : 0;
        }
        
        // 重置指针
        void reset(T* ptr = nullptr) {
            release();
            ptr_ = ptr;
            create_control_block();
        }
        
        // 交换指针
        void swap(shared_ptr& other) noexcept {
            tiny_stl::swap(ptr_, other.ptr_);
            tiny_stl::swap(ctrl_block_, other.ctrl_block_);
        }
    };

    // 数组特化版本
    template <typename T>
    class shared_ptr<T[]> {
    private:
        T* ptr_ = nullptr;
        control_block* ctrl_block_ = nullptr;

        void create_control_block() {
            if (ptr_) {
                ctrl_block_ = new array_control_block<T>(ptr_);
            }
        }

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
        shared_ptr() noexcept = default;
        
        explicit shared_ptr(T* ptr) : ptr_(ptr) {
            create_control_block();
        }
        
        ~shared_ptr() {
            release();
        }
        
        // 拷贝/移动构造和赋值
        shared_ptr(const shared_ptr& other) noexcept 
            : ptr_(other.ptr_), ctrl_block_(other.ctrl_block_) {
            if (ctrl_block_) ctrl_block_->increment();
        }
        
        shared_ptr(shared_ptr&& other) noexcept 
            : ptr_(exchange(other.ptr_, nullptr)), 
            ctrl_block_(exchange(other.ctrl_block_, nullptr)) {}
        
        shared_ptr& operator=(const shared_ptr& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = other.ptr_;
                ctrl_block_ = other.ctrl_block_;
                if (ctrl_block_) ctrl_block_->increment();
            }
            return *this;
        }
        
        shared_ptr& operator=(shared_ptr&& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = exchange(other.ptr_, nullptr);
                ctrl_block_ = exchange(other.ctrl_block_, nullptr);
            }
            return *this;
        }
        
        // 数组访问
        T& operator[](size_t index) const { return ptr_[index]; }
        T* get() const noexcept { return ptr_; }
        size_t use_count() const noexcept {
            return ctrl_block_ ? ctrl_block_->use_count() : 0;
        }
        void reset(T* ptr = nullptr) {
            release();
            ptr_ = ptr;
            create_control_block();
        }
    };

    // 创建 shared_ptr 的辅助函数
    template <typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T>(new T(forward<Args>(args)...));
    }

    template <typename T>
    shared_ptr<T[]> make_shared(size_t size) {
        return shared_ptr<T[]>(new T[size]());
    }

}
