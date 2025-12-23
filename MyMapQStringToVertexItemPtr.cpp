#include "MyMapQStringToVertexItemPtr.h"
#include "VisualItem.h"  // 包含 VertexItem 的完整定义

// ========== 私有辅助方法 ==========

size_t MyMapQStringToVertexItemPtr::findIndex(const QString& key) const
{
    for (size_t i = 0; i < m_size; ++i) {
        if (m_keys[i] == key) {
            return i;
        }
    }
    return SIZE_MAX;  // 未找到
}

void MyMapQStringToVertexItemPtr::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        // 如果新容量小于当前大小，则截断
        newCapacity = m_size;
    }
    
    if (newCapacity == 0) {
        newCapacity = 1;  // 至少分配1个元素的空间
    }
    
    // 分配新数组
    QString* newKeys = new QString[newCapacity];
    VertexItem** newValues = new VertexItem*[newCapacity];
    
    // 拷贝现有元素（浅拷贝：只拷贝指针值）
    for (size_t i = 0; i < m_size; ++i) {
        newKeys[i] = m_keys[i];
        newValues[i] = m_values[i];  // 只拷贝指针值，不拷贝指向的对象
    }
    
    // 初始化剩余位置为 nullptr
    for (size_t i = m_size; i < newCapacity; ++i) {
        newValues[i] = nullptr;
    }
    
    // 删除旧数组（只释放数组内存，不删除指针指向的对象）
    delete[] m_keys;
    delete[] m_values;
    
    // 更新指针和容量
    m_keys = newKeys;
    m_values = newValues;
    m_capacity = newCapacity;
}

// ========== 构造函数和析构函数 ==========

MyMapQStringToVertexItemPtr::MyMapQStringToVertexItemPtr(size_t initialCapacity)
    : m_keys(nullptr)
    , m_values(nullptr)
    , m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
{
    // 确保容量至少为1
    if (m_capacity == 0) {
        m_capacity = 16;
    }
    
    // 分配内存
    m_keys = new QString[m_capacity];
    m_values = new VertexItem*[m_capacity];
    
    // 初始化为 nullptr
    for (size_t i = 0; i < m_capacity; ++i) {
        m_values[i] = nullptr;
    }
}

MyMapQStringToVertexItemPtr::MyMapQStringToVertexItemPtr(const MyMapQStringToVertexItemPtr& other)
    : m_keys(nullptr)
    , m_values(nullptr)
    , m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    // 分配内存
    if (m_capacity > 0) {
        m_keys = new QString[m_capacity];
        m_values = new VertexItem*[m_capacity];
        
        // 拷贝元素（浅拷贝：只拷贝指针值）
        for (size_t i = 0; i < m_size; ++i) {
            m_keys[i] = other.m_keys[i];
            m_values[i] = other.m_values[i];  // 只拷贝指针值，不拷贝指向的对象
        }
        
        // 初始化剩余位置为 nullptr
        for (size_t i = m_size; i < m_capacity; ++i) {
            m_values[i] = nullptr;
        }
    } else {
        m_capacity = 16;
        m_keys = new QString[m_capacity];
        m_values = new VertexItem*[m_capacity];
        for (size_t i = 0; i < m_capacity; ++i) {
            m_values[i] = nullptr;
        }
    }
}

MyMapQStringToVertexItemPtr::~MyMapQStringToVertexItemPtr()
{
    // 释放内存（只释放数组内存，不删除指针指向的对象）
    if (m_keys != nullptr) {
        delete[] m_keys;
        m_keys = nullptr;
    }
    if (m_values != nullptr) {
        delete[] m_values;
        m_values = nullptr;
    }
    m_size = 0;
    m_capacity = 0;
}

// ========== 赋值运算符 ==========

MyMapQStringToVertexItemPtr& MyMapQStringToVertexItemPtr::operator=(const MyMapQStringToVertexItemPtr& other)
{
    // 防止自我赋值
    if (this == &other) {
        return *this;
    }
    
    // 如果容量不足，需要重新分配
    if (m_capacity < other.m_size) {
        // 释放旧内存
        delete[] m_keys;
        delete[] m_values;
        
        // 分配新内存
        m_capacity = other.m_capacity;
        m_keys = new QString[m_capacity];
        m_values = new VertexItem*[m_capacity];
    }
    
    // 拷贝元素（浅拷贝：只拷贝指针值）
    m_size = other.m_size;
    for (size_t i = 0; i < m_size; ++i) {
        m_keys[i] = other.m_keys[i];
        m_values[i] = other.m_values[i];  // 只拷贝指针值，不拷贝指向的对象
    }
    
    // 初始化剩余位置为 nullptr
    for (size_t i = m_size; i < m_capacity; ++i) {
        m_values[i] = nullptr;
    }
    
    return *this;
}

// ========== 元素访问 ==========

VertexItem*& MyMapQStringToVertexItemPtr::operator[](const QString& key)
{
    // 查找键
    size_t index = findIndex(key);
    
    if (index != SIZE_MAX) {
        // 键已存在，返回值的引用
        return m_values[index];
    }
    
    // 键不存在，需要插入
    // 如果容量不足，需要扩容
    if (m_size >= m_capacity) {
        // 2倍扩容策略
        size_t newCapacity = m_capacity * 2;
        if (newCapacity == 0) {
            newCapacity = 16;
        }
        reallocate(newCapacity);
    }
    
    // 插入新键值对
    m_keys[m_size] = key;
    m_values[m_size] = nullptr;  // 默认值为 nullptr
    ++m_size;
    
    return m_values[m_size - 1];
}

VertexItem*& MyMapQStringToVertexItemPtr::at(const QString& key)
{
    size_t index = findIndex(key);
    if (index == SIZE_MAX) {
        throw VectorException("Key not found");
    }
    return m_values[index];
}

VertexItem* const& MyMapQStringToVertexItemPtr::at(const QString& key) const
{
    size_t index = findIndex(key);
    if (index == SIZE_MAX) {
        throw VectorException("Key not found");
    }
    return m_values[index];
}

// ========== 容量相关 ==========

void MyMapQStringToVertexItemPtr::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

// ========== 修改操作 ==========

bool MyMapQStringToVertexItemPtr::insert(const QString& key, VertexItem* value)
{
    // 查找键
    size_t index = findIndex(key);
    
    if (index != SIZE_MAX) {
        // 键已存在，更新值
        m_values[index] = value;
        return false;
    }
    
    // 键不存在，需要插入
    // 如果容量不足，需要扩容
    if (m_size >= m_capacity) {
        // 2倍扩容策略
        size_t newCapacity = m_capacity * 2;
        if (newCapacity == 0) {
            newCapacity = 16;
        }
        reallocate(newCapacity);
    }
    
    // 插入新键值对
    m_keys[m_size] = key;
    m_values[m_size] = value;
    ++m_size;
    
    return true;
}

bool MyMapQStringToVertexItemPtr::erase(const QString& key)
{
    size_t index = findIndex(key);
    
    if (index == SIZE_MAX) {
        // 键不存在
        return false;
    }
    
    // 将index之后的元素向前移动
    for (size_t i = index; i < m_size - 1; ++i) {
        m_keys[i] = m_keys[i + 1];
        m_values[i] = m_values[i + 1];
    }
    
    --m_size;
    return true;
}

void MyMapQStringToVertexItemPtr::clear()
{
    m_size = 0;
    // 不释放内存，不删除指针指向的对象，只重置大小
}

// ========== 查找操作 ==========

VertexItem** MyMapQStringToVertexItemPtr::find(const QString& key)
{
    size_t index = findIndex(key);
    if (index == SIZE_MAX) {
        return nullptr;
    }
    return &m_values[index];
}

VertexItem* const* MyMapQStringToVertexItemPtr::find(const QString& key) const
{
    size_t index = findIndex(key);
    if (index == SIZE_MAX) {
        return nullptr;
    }
    return &m_values[index];
}

bool MyMapQStringToVertexItemPtr::contains(const QString& key) const
{
    return findIndex(key) != SIZE_MAX;
}

QString& MyMapQStringToVertexItemPtr::keyAt(size_t index)
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
    return m_keys[index];
}

const QString& MyMapQStringToVertexItemPtr::keyAt(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
    return m_keys[index];
}

VertexItem*& MyMapQStringToVertexItemPtr::valueAt(size_t index)
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
    return m_values[index];
}

VertexItem* const& MyMapQStringToVertexItemPtr::valueAt(size_t index) const
{
    if (index >= m_size) {
        throw VectorException("Index out of bounds");
    }
    return m_values[index];
}

// ========== 辅助方法 ==========

void MyMapQStringToVertexItemPtr::swap(MyMapQStringToVertexItemPtr& other)
{
    // 交换指针和大小
    QString* tempKeys = m_keys;
    VertexItem** tempValues = m_values;
    size_t tempSize = m_size;
    size_t tempCapacity = m_capacity;
    
    m_keys = other.m_keys;
    m_values = other.m_values;
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    
    other.m_keys = tempKeys;
    other.m_values = tempValues;
    other.m_size = tempSize;
    other.m_capacity = tempCapacity;
}

