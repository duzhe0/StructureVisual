#ifndef SIMPLESTRING_H
#define SIMPLESTRING_H

#include <cstddef>  // size_t

class SimpleString {
private:
    char* data_;
    size_t length_;
    size_t capacity_;

    // 内部辅助方法
    void reallocate(size_t new_capacity);
    void copy_from(const char* str, size_t len);

public:
    // ========== 构造函数和析构函数 ==========
    SimpleString();                             // 默认构造函数
    SimpleString(const char* str);              // C风格字符串构造函数
    SimpleString(const SimpleString& other);    // 拷贝构造函数
    SimpleString(SimpleString&& other) noexcept;// 移动构造函数
    ~SimpleString();                            // 析构函数

    // ========== 赋值运算符 ==========
    SimpleString& operator=(const SimpleString& other);     // 拷贝赋值
    SimpleString& operator=(SimpleString&& other) noexcept; // 移动赋值
    SimpleString& operator=(const char* str);               // C字符串赋值

    // ========== 访问方法 ==========
    const char* c_str() const { return data_; }
    size_t length() const { return length_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return length_ == 0; }

    // ========== 比较运算符 ==========
    bool operator==(const SimpleString& other) const;
    bool operator==(const char* other) const;
    bool operator!=(const SimpleString& other) const;
    bool operator<(const SimpleString& other) const;
    bool operator>(const SimpleString& other) const;
    bool operator<=(const SimpleString& other)const;
    bool operator>=(const SimpleString& other)const;

    // ========== 转换运算符 ==========
    operator const char*()const;
    // ========== 修改操作 ==========
    void clear();
    void append(const char* str);
    void append(const SimpleString& other);

    // ========== 哈希支持 ==========
    size_t hash() const;

    // ========== 工具函数 ==========
    void swap(SimpleString& other) noexcept;
};

// 全局 swap 函数
void swap(SimpleString& lhs, SimpleString& rhs) noexcept;

// 哈希函数对象（用于 HashMap）
struct SimpleStringHash {
    size_t operator()(const SimpleString& str) const {
        return str.hash();
    }
};

#endif // SIMPLESTRING_H
