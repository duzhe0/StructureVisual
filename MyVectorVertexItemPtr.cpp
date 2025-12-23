#include "MyVectorVertexItemPtr.h"
#include "VisualItem.h"  // 包含 VertexItem 的完整定义

// ========== 私有辅助方法 ==========

void MyVectorVertexItemPtr::checkIndex(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
}

void MyVectorVertexItemPtr::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        // 如果新容量小于当前大小，则截断
        newCapacity = m_size;
    }
    
    if (newCapacity == 0) {
        newCapacity = 1;  // 至少分配1个元素的空间
    }
    
    // 分配新数组（指针数组）
    VertexItem** newData = new VertexItem*[newCapacity];
    
    // 拷贝现有元素（浅拷贝：只拷贝指针值）
    for (size_t i = 0; i < m_size; ++i) {
        newData[i] = m_data[i];  // 只拷贝指针值，不拷贝指向的对象
    }
    
    // 初始化剩余位置为 nullptr
    for (size_t i = m_size; i < newCapacity; ++i) {
        newData[i] = nullptr;
    }
    
    // 删除旧数组（只释放数组内存，不删除指针指向的对象）
    delete[] m_data;
    
    // 更新指针和容量
    m_data = newData;
    m_capacity = newCapacity;
}

// ========== 构造函数和析构函数 ==========

MyVectorVertexItemPtr::MyVectorVertexItemPtr(size_t initialCapacity)
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
{
    // 确保容量至少为1
    if (m_capacity == 0) {
        m_capacity = 16;
    }
    
    // 分配内存（指针数组）
    m_data = new VertexItem*[m_capacity];
    
    // 初始化为 nullptr
    for (size_t i = 0; i < m_capacity; ++i) {
        m_data[i] = nullptr;
    }
}

MyVectorVertexItemPtr::MyVectorVertexItemPtr(const MyVectorVertexItemPtr& other)
    : m_data(nullptr)
    , m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    // 分配内存
    if (m_capacity > 0) {
        m_data = new VertexItem*[m_capacity];
        
        // 拷贝元素（浅拷贝：只拷贝指针值）
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];  // 只拷贝指针值，不拷贝指向的对象
        }
        
        // 初始化剩余位置为 nullptr
        for (size_t i = m_size; i < m_capacity; ++i) {
            m_data[i] = nullptr;
        }
    } else {
        m_data = nullptr;
        m_capacity = 16;
        m_data = new VertexItem*[m_capacity];
        for (size_t i = 0; i < m_capacity; ++i) {
            m_data[i] = nullptr;
        }
    }
}

MyVectorVertexItemPtr::~MyVectorVertexItemPtr()
{
    // 释放内存（只释放数组内存，不删除指针指向的对象）
    if (m_data != nullptr) {
        delete[] m_data;
        m_data = nullptr;
    }
    m_size = 0;
    m_capacity = 0;
}

// ========== 赋值运算符 ==========

MyVectorVertexItemPtr& MyVectorVertexItemPtr::operator=(const MyVectorVertexItemPtr& other)
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
        m_data = new VertexItem*[m_capacity];
    }
    
    // 拷贝元素（浅拷贝：只拷贝指针值）
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];  // 只拷贝指针值，不拷贝指向的对象
    }
    
    // 初始化剩余位置为 nullptr
    for (size_t i = m_size; i < m_capacity; ++i) {
        m_data[i] = nullptr;
    }
    
    return *this;
}

// ========== 元素访问 ==========

VertexItem*& MyVectorVertexItemPtr::at(size_t index)
{
    checkIndex(index);
    return m_data[index];
}

VertexItem* const& MyVectorVertexItemPtr::at(size_t index) const
{
    checkIndex(index);
    return m_data[index];
}

VertexItem*& MyVectorVertexItemPtr::operator[](size_t index)
{
    return m_data[index];
}

VertexItem* const& MyVectorVertexItemPtr::operator[](size_t index) const
{
    return m_data[index];
}

VertexItem*& MyVectorVertexItemPtr::front()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

VertexItem* const& MyVectorVertexItemPtr::front() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

VertexItem*& MyVectorVertexItemPtr::back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

VertexItem* const& MyVectorVertexItemPtr::back() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

// ========== 容量相关 ==========

void MyVectorVertexItemPtr::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

void MyVectorVertexItemPtr::resize(size_t newSize, VertexItem* value)
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
    
    // 如果新大小小于当前大小，只需要更新大小（指针类型，无需析构）
    m_size = newSize;
}

// ========== 修改操作 ==========

void MyVectorVertexItemPtr::push_back(VertexItem* value)
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

void MyVectorVertexItemPtr::pop_back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    --m_size;
    // 指针类型，不删除指针指向的对象
}

void MyVectorVertexItemPtr::clear()
{
    m_size = 0;
    // 不释放内存，不删除指针指向的对象，只重置大小
}

void MyVectorVertexItemPtr::insert(size_t index, VertexItem* value)
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

void MyVectorVertexItemPtr::erase(size_t index)
{
    checkIndex(index);
    
    // 将index之后的元素向前移动
    for (size_t i = index; i < m_size - 1; ++i) {
        m_data[i] = m_data[i + 1];
    }
    
    --m_size;
}

// ========== 辅助方法 ==========

void MyVectorVertexItemPtr::swap(MyVectorVertexItemPtr& other)
{
    // 交换指针和大小
    VertexItem** tempData = m_data;
    size_t tempSize = m_size;
    size_t tempCapacity = m_capacity;
    
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    
    other.m_data = tempData;
    other.m_size = tempSize;
    other.m_capacity = tempCapacity;
}

