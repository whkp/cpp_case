#include <bits/stdc++.h>

template<typename T>
class Shared_ptr {
private:
    T* ptr_;
    //引用计数要用指针，因为所有变量共享这一资源
    std::atomic<std::size_t>* ref_count_;
    void release() {
        if(ref_count_ && ref_count_->fetch_sub(1) == 1) {
            delete ptr_;
            delete ref_count_;
        }
    }
public:
    Shared_ptr() : ptr_(nullptr), ref_count_(nullptr) {}
    //防止隐式转换
    explicit Shared_ptr(T* ptr = nullptr) : ptr_(ptr), ref_count_(ptr ? new std::atomic<std::size_t>(1), nullptr) {}

    ~Shared_ptr() {
        release();
    }

    Shared_ptr(const Shared_ptr& other) : ptr_(other.ptr_), ref_count_(ref_count_) {
        if(ref_count_) {
            ref_count_->fetch_add(1);
        }
    }

    Shared_ptr& operator=(const Shared_ptr& other) {
        //要先判断是否是自赋值
        if(this != &other) {
            //先释放掉当前资源，再去修改当前值
            release();
            ptr_ = other.ptr_;
            ref_count_ = other.ref_count_;
            if(ref_count) {
                ref_count -> fetch_add(1);
            }
        }
        return *this;
    }

    Shared_ptr(Shared_ptr&& other) noexcept {
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    } 

    Shared_ptr& operator=(Shared_ptr&& other) noexcept {
        if(&other != this) {
            release();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* get() {
        return ptr_;
    }

    T& operator*() {
        return *ptr_;
    }

    T* operator->() {
        return ptr_;
    }

    size_t use_count() const {
        return ref_count_ ? ref_count_->load() : 0;
    }
    
};
