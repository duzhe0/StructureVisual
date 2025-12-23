#include "MyStackQString.h"
#include <algorithm>
#include <cstring>

// ========== 构造函数和析构函数 ==========

MyStackQString::MyStackQString(size_t initialCapacity)
    : m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
{
    m_data = new QString[m_capacity];
}

MyStackQString::MyStackQString(const MyStackQString& other)
    : m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    m_data = new QString[m_capacity];
    
    // 复制所有元素
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
}

MyStackQString::~MyStackQString()
{
    delete[] m_data;
}

MyStackQString& MyStackQString::operator=(const MyStackQString& other)
{
    if (this != &other) {
        // 使用拷贝-交换惯用法
        MyStackQString temp(other);
        swap(temp);
    }
    return *this;
}

// ========== 私有辅助方法 ==========

void MyStackQString::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        throw VectorException("New capacity is smaller than current size");
    }
    
    QString* newData = new QString[newCapacity];
    
    // 复制现有元素
    for (size_t i = 0; i < m_size; ++i) {
        newData[i] = m_data[i];
    }
    
    delete[] m_data;
    m_data = newData;
    m_capacity = newCapacity;
}

void MyStackQString::checkNotEmpty() const
{
    if (m_size == 0) {
        throw VectorException("Stack is empty");
    }
}

// ========== 栈操作 ==========

void MyStackQString::push(const QString& value)
{
    // 如果容量不足，扩容
    if (m_size >= m_capacity) {
        reallocate(m_capacity == 0 ? 16 : m_capacity * 2);
    }
    
    // 在栈顶（数组末尾）插入元素
    m_data[m_size] = value;
    ++m_size;
}

void MyStackQString::pop()
{
    checkNotEmpty();
    
    // 只需要减少大小，不需要实际删除元素
    // QString 的析构函数会在 delete[] 时自动调用
    --m_size;
}

QString& MyStackQString::top()
{
    checkNotEmpty();
    return m_data[m_size - 1];
}

const QString& MyStackQString::top() const
{
    checkNotEmpty();
    return m_data[m_size - 1];
}

// ========== 容量和状态查询 ==========

void MyStackQString::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

void MyStackQString::clear()
{
    // 重置大小
    // QString 的析构函数会在 delete[] 时自动调用
    m_size = 0;
}

void MyStackQString::swap(MyStackQString& other)
{
    // 交换所有成员变量
    QString* tempData = m_data;
    m_data = other.m_data;
    other.m_data = tempData;
    
    size_t tempSize = m_size;
    m_size = other.m_size;
    other.m_size = tempSize;
    
    size_t tempCapacity = m_capacity;
    m_capacity = other.m_capacity;
    other.m_capacity = tempCapacity;
}

