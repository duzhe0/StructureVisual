#include "simplestring.h"
#include <cstring>  // 使用 strlen, strcpy, memcpy
#include <cstdio>
// ========== 私有辅助方法 ==========

void SimpleString::reallocate(size_t new_capacity) {
    if (new_capacity == 0) {
        new_capacity = 1;  // 确保至少分配1个字符
    }

    char* new_data = new char[new_capacity];

    // 拷贝现有数据（包括null终止符）
    if (data_ != nullptr) {
        size_t copy_length = (length_ < new_capacity - 1) ? length_ : new_capacity - 1;
        for (size_t i = 0; i <= copy_length; ++i) {  // 包括null终止符
            new_data[i] = data_[i];
        }
        delete[] data_;
    } else {
        new_data[0] = '\0';
    }

    data_ = new_data;
    capacity_ = new_capacity;

    // 如果新容量小于当前长度，调整长度
    if (length_ >= capacity_) {
        length_ = capacity_ - 1;
        data_[length_] = '\0';
    }
}

void SimpleString::copy_from(const char* str, size_t len) {
    if (len == 0) {
        if (data_ == nullptr) {
            reallocate(16);  // 默认初始容量
        }
        data_[0] = '\0';
        length_ = 0;
        return;
    }

    // 确保有足够容量（+1 用于null终止符）
    if (capacity_ < len + 1) {
        reallocate(len + 1);
    }

    // 拷贝字符
    for (size_t i = 0; i < len; ++i) {
        data_[i] = str[i];
    }
    data_[len] = '\0';
    length_ = len;
}

// ========== 构造函数和析构函数 ==========

SimpleString::SimpleString()
    : data_(nullptr), length_(0), capacity_(0) {
    reallocate(16);  // 默认初始容量
}

SimpleString::SimpleString(const char* str)
    : data_(nullptr), length_(0), capacity_(0) {
    if (str != nullptr) {
        // 计算字符串长度
        size_t len = 0;
        while (str[len] != '\0') {
            ++len;
        }
        copy_from(str, len);
    } else {
        reallocate(16);
        data_[0] = '\0';
    }
}

SimpleString::SimpleString(const SimpleString& other)
    : data_(nullptr), length_(other.length_), capacity_(0) {
    if (other.data_ != nullptr) {
        reallocate(other.capacity_);
        copy_from(other.data_, other.length_);
    } else {
        reallocate(16);
    }
}

SimpleString::SimpleString(SimpleString&& other) noexcept
    : data_(other.data_), length_(other.length_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.length_ = 0;
    other.capacity_ = 0;
}

SimpleString::SimpleString(const int i) {
        // 计算整数转换为字符串后需要的最大长度
        // INT_MAX通常是10位数字，加上符号和结束符
        const int bufferSize = 32; // 足够大的缓冲区
        char buffer[bufferSize];
        
        // 使用snprintf安全地转换整数为字符串
        snprintf(buffer, bufferSize, "%d", i);
        
        // 分配内存并复制字符串
        data_ = new char[strlen(buffer) + 1];
        strcpy(data_, buffer);
}

SimpleString::~SimpleString() {
    delete[] data_;
}

// ========== 赋值运算符 ==========

SimpleString& SimpleString::operator=(const SimpleString& other) {
    if (this != &other) {
        copy_from(other.data_, other.length_);
    }
    return *this;
}

SimpleString& SimpleString::operator=(SimpleString&& other) noexcept {
    if (this != &other) {
        delete[] data_;

        data_ = other.data_;
        length_ = other.length_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.length_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

SimpleString& SimpleString::operator=(const char* str) {
    if (str != nullptr) {
        size_t len = 0;
        while (str[len] != '\0') {
            ++len;
        }
        copy_from(str, len);
    } else {
        clear();
    }
    return *this;
}

// ========== 比较运算符 ==========

bool SimpleString::operator==(const SimpleString& other) const {
    if (length_ != other.length_) {
        return false;
    }

    for (size_t i = 0; i < length_; ++i) {
        if (data_[i] != other.data_[i]) {
            return false;
        }
    }
    return true;
}

bool SimpleString::operator==(const char* other) const {
    if (other == nullptr) return data_ == nullptr;
    if (data_ == nullptr) return false;
    return strcmp(data_, other) == 0;
}

bool SimpleString::operator!=(const SimpleString& other) const {
    return !(*this == other);
}

bool SimpleString::operator<(const SimpleString& other) const {
    size_t min_length = (length_ < other.length_) ? length_ : other.length_;

    for (size_t i = 0; i < min_length; ++i) {
        if (data_[i] < other.data_[i]) {
            return true;
        } else if (data_[i] > other.data_[i]) {
            return false;
        }
    }

    // 如果前面字符都相同，长度短的更小
    return length_ < other.length_;
}

bool SimpleString::operator>(const SimpleString& other) const {
    return other < *this;
}

bool SimpleString::operator<=(const SimpleString& other) const {
    return (*this < other) || (*this == other);
}

bool SimpleString::operator>=(const SimpleString& other) const {
    return (*this > other) || (*this == other);
}

// ========== 拼接运算符 ==========
SimpleString SimpleString::operator+(const SimpleString& other)const{
    // 创建一个足够大的缓冲区
    size_t len1 = std::strlen(this->c_str());
    size_t len2 = std::strlen(other.c_str());
    char* buffer = new char[len1 + len2 + 1];
    
    std::strcpy(buffer, this->c_str());
    std::strcat(buffer, other.c_str());
    
    SimpleString result(buffer);
    delete[] buffer;
    return result;
}
// ========== 转换运算符 ==========
SimpleString::operator const char*()const{
    if (data_ == nullptr) {
        return "";  // 返回空字符串而不是nullptr
    }
    return data_;
}

// ========== 修改操作 ==========

void SimpleString::clear() {
    if (data_ != nullptr) {
        data_[0] = '\0';
    }
    length_ = 0;
}

void SimpleString::append(const char* str) {
    if (str == nullptr || str[0] == '\0') {
        return;
    }
    const char* safe_strptr=nullptr;
    SimpleString deepcopy_str(str);
    if(data_==str){//reallocate会销毁原指针 防止销毁影响str 进行深复制
        safe_strptr=deepcopy_str.c_str();
    }else{
        safe_strptr=str;
    }

    // 计算要追加的字符串长度
    size_t append_len = 0;
    while (safe_strptr[append_len] != '\0') {
        ++append_len;
    }

    // 确保有足够容量
    size_t new_length = length_ + append_len;
    if (new_length + 1 > capacity_) {
        reallocate((new_length + 1) * 2);  // 2倍扩容
    }

    // 追加字符
    for (size_t i = 0; i < append_len; ++i) {
        data_[length_ + i] = safe_strptr[i];
    }
    data_[new_length] = '\0';
    length_ = new_length;
}

void SimpleString::append(const SimpleString& other) {
    append(other.data_);
}

// ========== 哈希支持 ==========

size_t SimpleString::hash() const {
    // 简单的哈希函数（FNV-1a变体）
    size_t hash_value = 2166136261u;  // FNV偏移基础值

    for (size_t i = 0; i < length_; ++i) {
        hash_value ^= static_cast<size_t>(data_[i]);
        hash_value *= 16777619u;  // FNV质数
    }

    return hash_value;
}

// ========== 工具函数 ==========

void SimpleString::swap(SimpleString& other) noexcept {
    char* temp_data = data_;
    data_ = other.data_;
    other.data_ = temp_data;

    size_t temp_length = length_;
    length_ = other.length_;
    other.length_ = temp_length;

    size_t temp_capacity = capacity_;
    capacity_ = other.capacity_;
    other.capacity_ = temp_capacity;
}

// 全局 swap 函数
void swap(SimpleString& lhs, SimpleString& rhs) noexcept {
    lhs.swap(rhs);
}
