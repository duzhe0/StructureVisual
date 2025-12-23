#include "MyVectorBool.h"

// ========== 私有辅助方法 ==========

void MyVectorBool::checkIndex(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
}

void MyVectorBool::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        // 如果新容量小于当前大小，则截断
        newCapacity = m_size;
    }
    
    if (newCapacity == 0) {
        newCapacity = 1;  // 至少分配1个元素的空间
    }
    
    // 分配新数组
    bool* newData = new bool[newCapacity];
    
    // 拷贝现有元素
    for (size_t i = 0; i < m_size; ++i) {
        newData[i] = m_data[i];  // bool是基本类型，直接赋值
    }
    
    // 删除旧数组
    delete[] m_data;
    
    // 更新指针和容量
    m_data = newData;
    m_capacity = newCapacity;
}

// ========== 构造函数和析构函数 ==========

MyVectorBool::MyVectorBool(size_t initialCapacity)
    : m_data(nullptr)
    , m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
{
    // 确保容量至少为1
    if (m_capacity == 0) {
        m_capacity = 16;
    }
    
    // 分配内存
    m_data = new bool[m_capacity];
}

MyVectorBool::MyVectorBool(size_t count, bool value)
    : m_data(nullptr)
    , m_size(count)
    , m_capacity(count > 0 ? count : 16)
{
    // 确保容量至少为1
    if (m_capacity == 0) {
        m_capacity = 16;
    }
    
    // 分配内存
    m_data = new bool[m_capacity];
    
    // 用指定值填充
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = value;
    }
}

MyVectorBool::MyVectorBool(const MyVectorBool& other)
    : m_data(nullptr)
    , m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    // 分配内存
    if (m_capacity > 0) {
        m_data = new bool[m_capacity];
        
        // 拷贝元素
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];  // bool是基本类型，直接赋值
        }
    } else {
        m_data = nullptr;
        m_capacity = 16;
        m_data = new bool[m_capacity];
    }
}

MyVectorBool::~MyVectorBool()
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

MyVectorBool& MyVectorBool::operator=(const MyVectorBool& other)
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
        m_data = new bool[m_capacity];
    }
    
    // 拷贝元素
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
    
    return *this;
}

// ========== 元素访问 ==========

bool& MyVectorBool::at(size_t index)
{
    checkIndex(index);
    return m_data[index];
}

const bool& MyVectorBool::at(size_t index) const
{
    checkIndex(index);
    return m_data[index];
}

bool& MyVectorBool::operator[](size_t index)
{
    return m_data[index];
}

const bool& MyVectorBool::operator[](size_t index) const
{
    return m_data[index];
}

bool& MyVectorBool::front()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

const bool& MyVectorBool::front() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[0];
}

bool& MyVectorBool::back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

const bool& MyVectorBool::back() const
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    return m_data[m_size - 1];
}

// ========== 容量相关 ==========

void MyVectorBool::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

void MyVectorBool::resize(size_t newSize, bool value)
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
    
    // 如果新大小小于当前大小，只需要更新大小（bool是基本类型，无需析构）
    m_size = newSize;
}

// ========== 修改操作 ==========

void MyVectorBool::push_back(bool value)
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

void MyVectorBool::pop_back()
{
    if (m_size == 0) {
        throw VectorException("Vector is empty");
    }
    --m_size;
    // bool是基本类型，无需调用析构函数
}

void MyVectorBool::clear()
{
    m_size = 0;
    // 不释放内存，只重置大小
}

void MyVectorBool::assign(size_t count, bool value)
{
    clear();
    resize(count, value);
}

void MyVectorBool::insert(size_t index, bool value)
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

void MyVectorBool::erase(size_t index)
{
    checkIndex(index);
    
    // 将index之后的元素向前移动
    for (size_t i = index; i < m_size - 1; ++i) {
        m_data[i] = m_data[i + 1];
    }
    
    --m_size;
}

// ========== 辅助方法 ==========

void MyVectorBool::swap(MyVectorBool& other)
{
    // 交换指针和大小
    bool* tempData = m_data;
    size_t tempSize = m_size;
    size_t tempCapacity = m_capacity;
    
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    
    other.m_data = tempData;
    other.m_size = tempSize;
    other.m_capacity = tempCapacity;
}

