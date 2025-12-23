#include "MyVectorEdgeInfo.h"

// ========== 私有辅助方法 ==========

void MyVectorEdgeInfo::checkIndex(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
}

void MyVectorEdgeInfo::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        // 如果新容量小于当前大小，则截断
        newCapacity = m_size;
    }
    
    if (newCapacity == 0) {
        newCapacity = 1;  // 至少分配1个元素的空间
    }
    
    // 分配新数组
    EdgeInfo* newData = new EdgeInfo[newCapacity];
    
    // 拷贝现有元素（EdgeInfo 有拷贝构造函数）
    for (size_t i = 0; i < m_size; ++i) {
        newData[i] = m_data[i];
    }
    
    // 删除旧数组
    delete[] m_data;
    
    // 更新指针和容量
    m_data = newData;
    m_capacity = newCapacity;
}

// ========== 构造函数和析构函数 ==========

MyVectorEdgeInfo::MyVectorEdgeInfo(size_t initialCapacity)
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
{
    // 确保容量至少为1
    if (m_capacity == 0) {
        m_capacity = 16;
    }
    
    // 分配内存
    m_data = new EdgeInfo[m_capacity];
}

MyVectorEdgeInfo::MyVectorEdgeInfo(const MyVectorEdgeInfo& other)
    : m_data(nullptr)
    , m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    // 分配内存
    if (m_capacity > 0) {
        m_data = new EdgeInfo[m_capacity];
        
        // 拷贝元素
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    } else {
        m_data = nullptr;
        m_capacity = 16;
        m_data = new EdgeInfo[m_capacity];
    }
}

MyVectorEdgeInfo::~MyVectorEdgeInfo()
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

MyVectorEdgeInfo& MyVectorEdgeInfo::operator=(const MyVectorEdgeInfo& other)
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
        m_data = new EdgeInfo[m_capacity];
    }
    
    // 拷贝元素
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
    
    return *this;
}

// ========== 元素访问 ==========

EdgeInfo& MyVectorEdgeInfo::at(size_t index)
{
    checkIndex(index);
    return m_data[index];
}

const EdgeInfo& MyVectorEdgeInfo::at(size_t index) const
{
    checkIndex(index);
    return m_data[index];
}

EdgeInfo& MyVectorEdgeInfo::operator[](size_t index)
{
    return m_data[index];
}

const EdgeInfo& MyVectorEdgeInfo::operator[](size_t index) const
{
    return m_data[index];
}

EdgeInfo& MyVectorEdgeInfo::front()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

const EdgeInfo& MyVectorEdgeInfo::front() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

EdgeInfo& MyVectorEdgeInfo::back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

const EdgeInfo& MyVectorEdgeInfo::back() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

// ========== 容量相关 ==========

void MyVectorEdgeInfo::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

void MyVectorEdgeInfo::resize(size_t newSize, const EdgeInfo& value)
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
    
    // 如果新大小小于当前大小，只需要更新大小（EdgeInfo 的析构函数会在 delete[] 时自动调用）
    m_size = newSize;
}

// ========== 修改操作 ==========

void MyVectorEdgeInfo::push_back(const EdgeInfo& value)
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

void MyVectorEdgeInfo::pop_back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    --m_size;
    // EdgeInfo 的析构函数会在 delete[] 时自动调用
}

void MyVectorEdgeInfo::clear()
{
    m_size = 0;
    // 不释放内存，只重置大小
}

void MyVectorEdgeInfo::insert(size_t index, const EdgeInfo& value)
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

void MyVectorEdgeInfo::erase(size_t index)
{
    checkIndex(index);
    
    // 将index之后的元素向前移动
    for (size_t i = index; i < m_size - 1; ++i) {
        m_data[i] = m_data[i + 1];
    }
    
    --m_size;
}

// ========== 辅助方法 ==========

void MyVectorEdgeInfo::swap(MyVectorEdgeInfo& other)
{
    // 交换指针和大小
    EdgeInfo* tempData = m_data;
    size_t tempSize = m_size;
    size_t tempCapacity = m_capacity;
    
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    
    other.m_data = tempData;
    other.m_size = tempSize;
    other.m_capacity = tempCapacity;
}

