#pragma once

#include <cstddef>  // 只使用标准类型定义，不使用 STL
#include <utility>  // 需要包含这个来使用 std::move
#include "Iterable.h"
#include "Iterator.h"
// 简单的异常类（替代 std::exception）
class SimpleException {
private:
    const char* message_;
public:
    SimpleException(const char* msg) : message_(msg) {}
    const char* what() const { return message_; }
};

// ========== 动态数组类 ==========
// 前置声明
template<typename T>
class DynamicArray;
//ArrayIterator begin

template<typename T>
class ArrayIterator:public Iterator<T>{
public:
    ArrayIterator(const DynamicArray<T>* array);
    virtual void First();
    virtual void Next();
    virtual bool isDone() const;
    virtual T CurrentItem() const;
private:
    const DynamicArray<T>* _array;
    long _current;
};

template<class T>
ArrayIterator<T>::ArrayIterator(const DynamicArray<T> *array):
    _array(array),_current(0){}

template<class T>
void ArrayIterator<T>::First(){
    _current=0;
}

template<class T>
void ArrayIterator<T>::Next(){
    _current++;
}

template<class T>
bool ArrayIterator<T>::isDone() const{
    return _current>=_array->size();
}

template<class T>
T ArrayIterator<T>::CurrentItem() const{
    if(isDone()){
        throw std::out_of_range("Array index");
    }
    return _array->at(_current);
}

//ArrayIterator end

template<typename T>
class DynamicArray:public Iterable<T>{
private:
    T* data_;
    size_t size_;
    size_t capacity_;

    // 重新分配内存
    void reallocate(size_t new_capacity);

    // 边界检查（内联以提高性能）
    void check_index(size_t index) const {
        if (index >= size_) {
            throw SimpleException("Index out of bounds");
        }
    }

public:
    // 默认构造函数
    DynamicArray(size_t initial_capacity = 16)
        : data_(nullptr), size_(0), capacity_(initial_capacity > 0 ? initial_capacity : 1) {
        data_ = new T[capacity_];
    }

    // 带初始大小的构造函数
    DynamicArray(size_t count, const T& initial_value)
        : data_(nullptr), size_(count), capacity_(count > 0 ? count : 1) {
        data_ = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = initial_value;
        }
    }

    // 拷贝构造函数
    DynamicArray(const DynamicArray& other)
        : data_(nullptr), size_(other.size_), capacity_(other.capacity_) {
        data_ = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    // 移动构造函数（C++11 风格）
    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // 析构函数
    ~DynamicArray() {
        delete[] data_;
    }
    // 拷贝赋值 deepcopy
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            // 创建临时副本（异常安全）
            DynamicArray temp(other);
            this->swap(temp);
        }
        return *this;
    }

    // 移动赋值
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // ========== 元素访问 ==========

    T& operator[](size_t index) {
        check_index(index);
        return data_[index];
    }

    const T& operator[](size_t index) const {
        check_index(index);
        return data_[index];
    }

    T& at(size_t index) {
        check_index(index);
        return data_[index];
    }

    const T& at(size_t index) const {
        check_index(index);
        return data_[index];
    }

    T& front() {
        if (size_ == 0) throw SimpleException("Array is empty");
        return data_[0];
    }

    const T& front() const {
        if (size_ == 0) throw SimpleException("Array is empty");
        return data_[0];
    }

    T& back() {
        if (size_ == 0) throw SimpleException("Array is empty");
        return data_[size_ - 1];
    }

    const T& back() const {
        if (size_ == 0) throw SimpleException("Array is empty");
        return data_[size_ - 1];
    }

    T* data() { return data_; }
    const T* data() const { return data_; }

    // ========== 容量操作 ==========

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    void reserve(size_t new_capacity) {
        reallocate(new_capacity);
    }

    void shrink_to_fit() {
        if (size_ < capacity_) {
            reallocate(size_ > 0 ? size_ : 1);
        }
    }

    // ========== 修改操作 ==========

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            reallocate(capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void push_back(T&& value) {
        if (size_ >= capacity_) {
            reallocate(capacity_ * 2);
        }
        data_[size_++] = std::move(value);  // 移动语义（如果可用）
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
        }
    }

    void clear() {
        size_ = 0;
    }

    void resize(size_t new_size, const T& value = T()) {
        if (new_size > capacity_) {
            reallocate(new_size);
        }

        if (new_size > size_) {
            // 填充新元素
            for (size_t i = size_; i < new_size; ++i) {
                data_[i] = value;
            }
        }
        size_ = new_size;
    }

    // ========== 迭代器方法 ==========
    Iterator<T>* createIterator(){
        return new ArrayIterator<T>(this);
    }
    // ========== 工具函数 ==========

    void swap(DynamicArray& other) noexcept {
        // 交换所有成员变量
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
};

// 全局 swap 函数
template<typename T>
void swap(DynamicArray<T>& lhs, DynamicArray<T>& rhs) noexcept {
    lhs.swap(rhs);
}
