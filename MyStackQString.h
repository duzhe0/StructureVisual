#ifndef MYSTACKQSTRING_H
#define MYSTACKQSTRING_H

#include <QString>
#include <cstddef>   // for size_t
#include "VectorException.h"

/**
 * @brief MyStackQString - 用于替代 std::stack<QString> 的自定义栈类
 * 
 * 特点：
 * - 不使用模板，专门用于 QString 类型
 * - 手动内存管理（new[]/delete[]）
 * - 使用动态数组实现，支持高效入栈和出栈
 * - 自动扩容机制（容量不足时2倍扩容）
 * - 提供完整的拷贝语义和异常安全
 */
class MyStackQString {
private:
    QString* m_data;          // 数据数组指针
    size_t m_size;            // 当前元素数量
    size_t m_capacity;         // 当前容量
    
    /**
     * @brief 重新分配内存（扩容）
     * @param newCapacity 新的容量大小
     */
    void reallocate(size_t newCapacity);
    
    /**
     * @brief 检查栈是否为空（用于异常检查）
     * @throw VectorException 如果栈为空
     */
    void checkNotEmpty() const;

public:
    // ========== 构造函数和析构函数 ==========
    
    /**
     * @brief 默认构造函数
     * @param initialCapacity 初始容量，默认为16
     */
    explicit MyStackQString(size_t initialCapacity = 16);
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 MyStackQString 对象
     */
    MyStackQString(const MyStackQString& other);
    
    /**
     * @brief 析构函数
     */
    ~MyStackQString();
    
    /**
     * @brief 赋值运算符
     * @param other 要赋值的 MyStackQString 对象
     * @return 返回自身的引用
     */
    MyStackQString& operator=(const MyStackQString& other);
    
    // ========== 栈操作 ==========
    
    /**
     * @brief 将元素压入栈顶
     * @param value 要压入的 QString 值
     */
    void push(const QString& value);
    
    /**
     * @brief 移除栈顶元素
     * @throw VectorException 如果栈为空
     */
    void pop();
    
    /**
     * @brief 访问栈顶元素（非const版本）
     * @return 栈顶元素的引用
     * @throw VectorException 如果栈为空
     */
    QString& top();
    
    /**
     * @brief 访问栈顶元素（const版本）
     * @return 栈顶元素的const引用
     * @throw VectorException 如果栈为空
     */
    const QString& top() const;
    
    // ========== 容量和状态查询 ==========
    
    /**
     * @brief 获取栈中元素的数量
     * @return 元素数量
     */
    size_t size() const { return m_size; }
    
    /**
     * @brief 获取栈的容量
     * @return 容量大小
     */
    size_t capacity() const { return m_capacity; }
    
    /**
     * @brief 判断栈是否为空
     * @return true 如果栈为空，false 否则
     */
    bool empty() const { return m_size == 0; }
    
    /**
     * @brief 预留容量（如果 newCapacity > capacity，则扩容）
     * @param newCapacity 新的容量大小
     */
    void reserve(size_t newCapacity);
    
    /**
     * @brief 清空栈（移除所有元素）
     */
    void clear();
    
    /**
     * @brief 交换两个栈的内容
     * @param other 要交换的另一个栈
     */
    void swap(MyStackQString& other);
};

#endif // MYSTACKQSTRING_H

