#pragma once
#include "utility.hpp"  // 自定义工具函数

namespace tiny_stl {
    
    template <typename T>
    class unique_ptr {
    private:
        T* ptr_ = nullptr;

    public:
        // 构造函数
        explicit unique_ptr(T* ptr = nullptr) noexcept : ptr_(ptr) {}
        
        // 禁止拷贝
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;
        
        // 移动构造函数
        unique_ptr(unique_ptr&& other) noexcept 
            : ptr_(other.release()) {}
        
        // 移动赋值运算符
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            reset(other.release());
            return *this;
        }
        
        // 析构函数
        ~unique_ptr() {
            reset();
        }
        
        // 释放所有权
        T* release() noexcept {
            return exchange(ptr_, nullptr);
        }
        
        // 重置指针
        void reset(T* new_ptr = nullptr) noexcept {
            T* old_ptr = exchange(ptr_, new_ptr);
            if (old_ptr) {
                delete old_ptr;
            }
        }
        
        // 交换指针
        void swap(unique_ptr& other) noexcept {
            tiny_stl::swap(ptr_, other.ptr_);
        }
        
        // 访问指针
        T* get() const noexcept { return ptr_; }
        explicit operator bool() const noexcept { return ptr_ != nullptr; }
        
        // 解引用操作符
        T& operator*() const noexcept { return *ptr_; }
        T* operator->() const noexcept { return ptr_; }
    };

    // 数组特化版本
    template <typename T>
    class unique_ptr<T[]> {
    private:
        T* ptr_ = nullptr;

    public:
        explicit unique_ptr(T* ptr = nullptr) noexcept : ptr_(ptr) {}
        ~unique_ptr() { reset(); }
        
        // 禁止拷贝和移动
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr(unique_ptr&& other) noexcept : ptr_(other.release()) {}
        unique_ptr& operator=(const unique_ptr&) = delete;
        unique_ptr& operator=(unique_ptr&& other) noexcept {
            reset(other.release());
            return *this;
        }
        
        T* release() noexcept { return exchange(ptr_, nullptr); }
        
        void reset(T* new_ptr = nullptr) noexcept {
            T* old_ptr = exchange(ptr_, new_ptr);
            if (old_ptr) {
                delete[] old_ptr;
            }
        }
        
        T& operator[](size_t index) const { return ptr_[index]; }
        T* get() const noexcept { return ptr_; }
        explicit operator bool() const noexcept { return ptr_ != nullptr; }
    };

    // 辅助函数用于创建 unique_ptr
    template <typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args) {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }

    template <typename T>
    unique_ptr<T[]> make_unique(size_t size) {
        return unique_ptr<T[]>(new T[size]());
    }

}
