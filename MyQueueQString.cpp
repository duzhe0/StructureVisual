#include "MyQueueQString.h"
#include <algorithm>
#include <cstring>

// ========== 构造函数和析构函数 ==========

MyQueueQString::MyQueueQString(size_t initialCapacity)
    : m_size(0)
    , m_capacity(initialCapacity > 0 ? initialCapacity : 16)
    , m_front(0)
    , m_back(0)
{
    m_data = new QString[m_capacity];
}

MyQueueQString::MyQueueQString(const MyQueueQString& other)
    : m_size(other.m_size)
    , m_capacity(other.m_capacity)
    , m_front(0)
    , m_back(other.m_size)
{
    m_data = new QString[m_capacity];
    
    // 将循环数组中的元素按顺序复制到新数组
    for (size_t i = 0; i < m_size; ++i) {
        size_t index = (other.m_front + i) % other.m_capacity;
        m_data[i] = other.m_data[index];
    }
}

MyQueueQString::~MyQueueQString()
{
    delete[] m_data;
}

MyQueueQString& MyQueueQString::operator=(const MyQueueQString& other)
{
    if (this != &other) {
        // 使用拷贝-交换惯用法
        MyQueueQString temp(other);
        swap(temp);
    }
    return *this;
}

// ========== 私有辅助方法 ==========

void MyQueueQString::reallocate(size_t newCapacity)
{
    if (newCapacity < m_size) {
        throw VectorException("New capacity is smaller than current size");
    }
    
    QString* newData = new QString[newCapacity];
    
    // 将循环数组中的元素按顺序复制到新数组
    for (size_t i = 0; i < m_size; ++i) {
        size_t index = (m_front + i) % m_capacity;
        newData[i] = m_data[index];
    }
    
    delete[] m_data;
    m_data = newData;
    m_capacity = newCapacity;
    m_front = 0;
    m_back = m_size;
}

void MyQueueQString::checkNotEmpty() const
{
    if (m_size == 0) {
        throw VectorException("Queue is empty");
    }
}

// ========== 队列操作 ==========

void MyQueueQString::push(const QString& value)
{
    // 如果容量不足，扩容
    if (m_size >= m_capacity) {
        reallocate(m_capacity == 0 ? 16 : m_capacity * 2);
    }
    
    // 在 back 位置插入元素
    m_data[m_back] = value;
    m_back = (m_back + 1) % m_capacity;
    ++m_size;
}

void MyQueueQString::pop()
{
    checkNotEmpty();
    
    // 移动 front 索引
    m_front = (m_front + 1) % m_capacity;
    --m_size;
}

QString& MyQueueQString::front()
{
    checkNotEmpty();
    return m_data[m_front];
}

const QString& MyQueueQString::front() const
{
    checkNotEmpty();
    return m_data[m_front];
}

QString& MyQueueQString::back()
{
    checkNotEmpty();
    // back 指向下一个插入位置，所以最后一个元素在 back-1 位置
    size_t lastIndex = (m_back + m_capacity - 1) % m_capacity;
    return m_data[lastIndex];
}

const QString& MyQueueQString::back() const
{
    checkNotEmpty();
    size_t lastIndex = (m_back + m_capacity - 1) % m_capacity;
    return m_data[lastIndex];
}

// ========== 容量和状态查询 ==========

void MyQueueQString::reserve(size_t newCapacity)
{
    if (newCapacity > m_capacity) {
        reallocate(newCapacity);
    }
}

void MyQueueQString::clear()
{
    // 重置索引和大小
    // QString 的析构函数会在 delete[] 时自动调用
    m_front = 0;
    m_back = 0;
    m_size = 0;
}

void MyQueueQString::swap(MyQueueQString& other)
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
    
    size_t tempFront = m_front;
    m_front = other.m_front;
    other.m_front = tempFront;
    
    size_t tempBack = m_back;
    m_back = other.m_back;
    other.m_back = tempBack;
}

