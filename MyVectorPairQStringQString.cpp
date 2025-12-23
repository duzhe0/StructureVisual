#include "MyVectorPairQStringQString.h"
#include <cstring>  // 用于 memcpy（如果需要）

// ========== 私有辅助方法 ==========

void MyVectorPairQStringQString::checkIndex(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
}

void MyVectorPairQStringQString::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        // 如果新容量小于当前大小，则截断
        newCapacity = m_size;
    }
    
    if (newCapacity == 0) {
        newCapacity = 1;  // 至少分配1个元素的空间
    }
    
    // 分配新数组
    MyPairQStringQString* newData = new MyPairQStringQString[newCapacity];
    
    // 拷贝现有元素
    for (size_t i = 0; i < m_size; ++i) {
        newData[i] = m_data[i];  // 使用MyPairQStringQString的赋值运算符
    }
    
    // 删除旧数组
    delete[] m_data;
    
    // 更新指针和容量
    m_data = newData;
    m_capacity = newCapacity;
}

// ========== 构造函数和析构函数 ==========

MyVectorPairQStringQString::MyVectorPairQStringQString(size_t initialCapacity)
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
{
    // 确保容量至少为1
    if (m_capacity == 0) {
        m_capacity = 16;
    }
    
    // 分配内存
    m_data = new MyPairQStringQString[m_capacity];
}

MyVectorPairQStringQString::MyVectorPairQStringQString(const MyVectorPairQStringQString& other)
    : m_data(nullptr)
    , m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    // 分配内存
    if (m_capacity > 0) {
        m_data = new MyPairQStringQString[m_capacity];
        
        // 拷贝元素
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];  // 使用MyPairQStringQString的赋值运算符
        }
    } else {
        m_data = nullptr;
        m_capacity = 16;
        m_data = new MyPairQStringQString[m_capacity];
    }
}

MyVectorPairQStringQString::~MyVectorPairQStringQString()
{
    // 释放内存
    if (m_data != nullptr) {
        delete[] m_data;
        m_data = nullptr;
    }
    m_size = 0;
    m_capacity = 0;
}

// ========== 赋值运算符 ==========

MyVectorPairQStringQString& MyVectorPairQStringQString::operator=(const MyVectorPairQStringQString& other)
{
    // 防止自我赋值
    if (this == &other) {
        return *this;
    }
    
    // 如果容量不足，需要重新分配
    if (m_capacity < other.m_size) {
        // 释放旧内存
        delete[] m_data;
        
        // 分配新内存
        m_capacity = other.m_capacity;
        m_data = new MyPairQStringQString[m_capacity];
    }
    
    // 拷贝元素
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
    
    return *this;
}

// ========== 元素访问 ==========

MyPairQStringQString& MyVectorPairQStringQString::at(size_t index)
{
    checkIndex(index);
    return m_data[index];
}

const MyPairQStringQString& MyVectorPairQStringQString::at(size_t index) const
{
    checkIndex(index);
    return m_data[index];
}

MyPairQStringQString& MyVectorPairQStringQString::operator[](size_t index)
{
    return m_data[index];
}

const MyPairQStringQString& MyVectorPairQStringQString::operator[](size_t index) const
{
    return m_data[index];
}

MyPairQStringQString& MyVectorPairQStringQString::front()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

const MyPairQStringQString& MyVectorPairQStringQString::front() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

MyPairQStringQString& MyVectorPairQStringQString::back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

const MyPairQStringQString& MyVectorPairQStringQString::back() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

// ========== 容量相关 ==========

void MyVectorPairQStringQString::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

void MyVectorPairQStringQString::resize(size_t newSize, const MyPairQStringQString& value)
{
    if (newSize > m_size) {
        // 如果需要扩容
        if (newSize > m_capacity) {
            reallocate(newSize);
        }
        // 用value填充新元素
        for (size_t i = m_size; i < newSize; ++i) {
            m_data[i] = value;
        }
    }
    m_size = newSize;
}

void MyVectorPairQStringQString::clear()
{
    m_size = 0;
}

// ========== 修改操作 ==========

void MyVectorPairQStringQString::push_back(const MyPairQStringQString& value)
{
    // 如果容量不足，扩容
    if (m_size >= m_capacity) {
        reallocate(m_capacity == 0 ? 16 : m_capacity * 2);
    }
    
    // 添加元素
    m_data[m_size] = value;
    ++m_size;
}

void MyVectorPairQStringQString::pop_back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    --m_size;
}

void MyVectorPairQStringQString::swap(MyVectorPairQStringQString& other)
{
    // 交换所有成员变量
    MyPairQStringQString* tempData = m_data;
    m_data = other.m_data;
    other.m_data = tempData;
    
    size_t tempSize = m_size;
    m_size = other.m_size;
    other.m_size = tempSize;
    
    size_t tempCapacity = m_capacity;
    m_capacity = other.m_capacity;
    other.m_capacity = tempCapacity;
}

