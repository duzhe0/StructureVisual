#include "MyVectorInt.h"

// ========== 私有辅助方法 ==========

void MyVectorInt::checkIndex(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
}

void MyVectorInt::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        // 如果新容量小于当前大小，则截断
        newCapacity = m_size;
    }
    
    if (newCapacity == 0) {
        newCapacity = 1;  // 至少分配1个元素的空间
    }
    
    // 分配新数组
    int* newData = new int[newCapacity];
    
    // 拷贝现有元素
    for (size_t i = 0; i < m_size; ++i) {
        newData[i] = m_data[i];  // int是基本类型，直接赋值
    }
    
    // 删除旧数组
    delete[] m_data;
    
    // 更新指针和容量
    m_data = newData;
    m_capacity = newCapacity;
}

// ========== 构造函数和析构函数 ==========

MyVectorInt::MyVectorInt(size_t initialCapacity)
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
{
    // 确保容量至少为1
    if (m_capacity == 0) {
        m_capacity = 16;
    }
    
    // 分配内存
    m_data = new int[m_capacity];
}

MyVectorInt::MyVectorInt(const MyVectorInt& other)
    : m_data(nullptr)
    , m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    // 分配内存
    if (m_capacity > 0) {
        m_data = new int[m_capacity];
        
        // 拷贝元素
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];  // int是基本类型，直接赋值
        }
    } else {
        m_data = nullptr;
        m_capacity = 16;
        m_data = new int[m_capacity];
    }
}

MyVectorInt::~MyVectorInt()
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

MyVectorInt& MyVectorInt::operator=(const MyVectorInt& other)
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
        m_data = new int[m_capacity];
    }
    
    // 拷贝元素
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
    
    return *this;
}

// ========== 元素访问 ==========

int& MyVectorInt::at(size_t index)
{
    checkIndex(index);
    return m_data[index];
}

const int& MyVectorInt::at(size_t index) const
{
    checkIndex(index);
    return m_data[index];
}

int& MyVectorInt::operator[](size_t index)
{
    return m_data[index];
}

const int& MyVectorInt::operator[](size_t index) const
{
    return m_data[index];
}

int& MyVectorInt::front()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

const int& MyVectorInt::front() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

int& MyVectorInt::back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

const int& MyVectorInt::back() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

// ========== 容量相关 ==========

void MyVectorInt::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

void MyVectorInt::resize(size_t newSize, int value)
{
    if (newSize > m_capacity) {
        // 需要扩容，使用2倍扩容策略
        size_t newCapacity = m_capacity * 2;
        while (newCapacity < newSize) {
            newCapacity *= 2;
        }
        reallocate(newCapacity);
    }
    
    // 如果新大小大于当前大小，用value填充
    while (m_size < newSize) {
        m_data[m_size] = value;
        ++m_size;
    }
    
    // 如果新大小小于当前大小，只需要更新大小（int是基本类型，无需析构）
    m_size = newSize;
}

// ========== 修改操作 ==========

void MyVectorInt::push_back(int value)
{
    // 如果容量不足，需要扩容
    if (m_size >= m_capacity) {
        // 2倍扩容策略
        size_t newCapacity = m_capacity * 2;
        if (newCapacity == 0) {
            newCapacity = 16;  // 如果容量为0，设置为16
        }
        reallocate(newCapacity);
    }
    
    // 添加新元素
    m_data[m_size] = value;
    ++m_size;
}

void MyVectorInt::pop_back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    --m_size;
    // int是基本类型，无需调用析构函数
}

void MyVectorInt::clear()
{
    m_size = 0;
    // 不释放内存，只重置大小
}

void MyVectorInt::insert(size_t index, int value)
{
    checkIndex(index);
    
    // 确保有足够容量
    if (m_size >= m_capacity) {
        size_t newCapacity = m_capacity * 2;
        if (newCapacity == 0) {
            newCapacity = 16;
        }
        reallocate(newCapacity);
    }
    
    // 将index之后的元素向后移动
    for (size_t i = m_size; i > index; --i) {
        m_data[i] = m_data[i - 1];
    }
    
    // 插入新元素
    m_data[index] = value;
    ++m_size;
}

void MyVectorInt::erase(size_t index)
{
    checkIndex(index);
    
    // 将index之后的元素向前移动
    for (size_t i = index; i < m_size - 1; ++i) {
        m_data[i] = m_data[i + 1];
    }
    
    --m_size;
}

// ========== 辅助方法 ==========

void MyVectorInt::swap(MyVectorInt& other)
{
    // 交换指针和大小
    int* tempData = m_data;
    size_t tempSize = m_size;
    size_t tempCapacity = m_capacity;
    
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    
    other.m_data = tempData;
    other.m_size = tempSize;
    other.m_capacity = tempCapacity;
}

