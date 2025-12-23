#include "MySetPairQStringQString.h"

// ========== 私有辅助方法 ==========

size_t MySetPairQStringQString::findIndex(const MyPairQStringQString& value) const
{
    for (size_t i = 0; i < m_size; ++i) {
        if (m_data[i] == value) {
            return i;
        }
    }
    return SIZE_MAX;  // 未找到
}

void MySetPairQStringQString::checkIndex(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
}

void MySetPairQStringQString::reallocate(size_t newCapacity)
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

MySetPairQStringQString::MySetPairQStringQString(size_t initialCapacity)
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

MySetPairQStringQString::MySetPairQStringQString(const MySetPairQStringQString& other)
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
        m_capacity = 16;
        m_data = new MyPairQStringQString[m_capacity];
    }
}

MySetPairQStringQString::~MySetPairQStringQString()
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

MySetPairQStringQString& MySetPairQStringQString::operator=(const MySetPairQStringQString& other)
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

MyPairQStringQString& MySetPairQStringQString::at(size_t index)
{
    checkIndex(index);
    return m_data[index];
}

const MyPairQStringQString& MySetPairQStringQString::at(size_t index) const
{
    checkIndex(index);
    return m_data[index];
}

// ========== 容量相关 ==========

void MySetPairQStringQString::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

// ========== 修改操作 ==========

bool MySetPairQStringQString::insert(const MyPairQStringQString& value)
{
    // 检查元素是否已存在
    size_t index = findIndex(value);
    if (index != SIZE_MAX) {
        // 元素已存在，不插入
        return false;
    }
    
    // 元素不存在，需要插入
    // 如果容量不足，需要扩容
    if (m_size >= m_capacity) {
        // 2倍扩容策略
        size_t newCapacity = m_capacity * 2;
        if (newCapacity == 0) {
            newCapacity = 16;
        }
        reallocate(newCapacity);
    }
    
    // 插入新元素
    m_data[m_size] = value;
    ++m_size;
    
    return true;
}

bool MySetPairQStringQString::erase(const MyPairQStringQString& value)
{
    size_t index = findIndex(value);
    
    if (index == SIZE_MAX) {
        // 元素不存在
        return false;
    }
    
    // 将index之后的元素向前移动
    for (size_t i = index; i < m_size - 1; ++i) {
        m_data[i] = m_data[i + 1];
    }
    
    --m_size;
    return true;
}

void MySetPairQStringQString::clear()
{
    m_size = 0;
    // 不释放内存，只重置大小
}

// ========== 查找操作 ==========

bool MySetPairQStringQString::find(const MyPairQStringQString& value) const
{
    return findIndex(value) != SIZE_MAX;
}

// ========== 辅助方法 ==========

void MySetPairQStringQString::swap(MySetPairQStringQString& other)
{
    // 交换指针和大小
    MyPairQStringQString* tempData = m_data;
    size_t tempSize = m_size;
    size_t tempCapacity = m_capacity;
    
    m_data = other.m_data;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    
    other.m_data = tempData;
    other.m_size = tempSize;
    other.m_capacity = tempCapacity;
}

