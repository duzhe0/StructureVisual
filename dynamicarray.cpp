#include "dynamicarray.h"

template<typename T>
class DynamicArray {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

    // 重新分配内存
    void reallocate(size_t new_capacity) {
        if (new_capacity < size_) {
            throw DynamicArrayException("New capacity cannot be less than current size");
        }

        T* new_data = new T[new_capacity];

        // 拷贝现有元素
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[i];  // 依赖 T 的拷贝赋值
        }

        // 清理旧内存并更新指针
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

    // 边界检查（内联以提高性能）
    void check_index(size_t index) const {
        if (index >= size_) {
            throw DynamicArrayException("Index out of bounds");
        }
    }

public:
    // ========== 构造函数和析构函数 ==========

    // 默认构造函数
    DynamicArray(size_t initial_capacity = 16)
        : size_(0), capacity_(initial_capacity > 0 ? initial_capacity : 1) {
        data_ = new T[capacity_];
    }

    // 带初始大小的构造函数
    DynamicArray(size_t count, const T& initial_value)
        : size_(count), capacity_(count > 0 ? count : 1) {
        data_ = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = initial_value;
        }
    }

    // 拷贝构造函数
    DynamicArray(const DynamicArray& other)
        : size_(other.size_), capacity_(other.capacity_) {
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

    // ========== 赋值运算符 ==========

    // 拷贝赋值
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
        if (size_ == 0) throw DynamicArrayException("Array is empty");
        return data_[0];
    }

    const T& front() const {
        if (size_ == 0) throw DynamicArrayException("Array is empty");
        return data_[0];
    }

    T& back() {
        if (size_ == 0) throw DynamicArrayException("Array is empty");
        return data_[size_ - 1];
    }

    const T& back() const {
        if (size_ == 0) throw DynamicArrayException("Array is empty");
        return data_[size_ - 1];
    }

    T* data() { return data_; }
    const T* data() const { return data_; }

    // ========== 容量操作 ==========

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            reallocate(new_capacity);
        }
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

    // ========== 工具函数 ==========

    void swap(DynamicArray& other) noexcept {
        // 交换所有成员变量
        T* temp_data = data_;
        data_ = other.data_;
        other.data_ = temp_data;

        size_t temp_size = size_;
        size_ = other.size_;
        other.size_ = temp_size;

        size_t temp_capacity = capacity_;
        capacity_ = other.capacity_;
        other.capacity_ = temp_capacity;
    }

    // ========== 迭代器支持 ==========

    // 前向迭代器
    class Iterator {
    private:
        T* ptr_;
    public:
        explicit Iterator(T* ptr) : ptr_(ptr) {}

        T& operator*() const { return *ptr_; }
        T* operator->() const { return ptr_; }

        Iterator& operator++() {
            ++ptr_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++ptr_;
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const Iterator& other) const {
            return ptr_ != other.ptr_;
        }
    };

    class ConstIterator {
    private:
        const T* ptr_;
    public:
        explicit ConstIterator(const T* ptr) : ptr_(ptr) {}

        const T& operator*() const { return *ptr_; }
        const T* operator->() const { return ptr_; }

        ConstIterator& operator++() {
            ++ptr_;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator temp = *this;
            ++ptr_;
            return temp;
        }

        bool operator==(const ConstIterator& other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const ConstIterator& other) const {
            return ptr_ != other.ptr_;
        }
    };

    Iterator begin() { return Iterator(data_); }
    Iterator end() { return Iterator(data_ + size_); }

    ConstIterator begin() const { return ConstIterator(data_); }
    ConstIterator end() const { return ConstIterator(data_ + size_); }

    ConstIterator cbegin() const { return ConstIterator(data_); }
    ConstIterator cend() const { return ConstIterator(data_ + size_); }
};

// 全局 swap 函数
template<typename T>
void swap(DynamicArray<T>& lhs, DynamicArray<T>& rhs) noexcept {
    lhs.swap(rhs);
}
