#ifndef MYSETQSTRING_H
#define MYSETQSTRING_H

#include <QString>
#include <cstddef>   // for size_t
#include "VectorException.h"

/**
 * @brief MySetQString - 用于替代 std::set<QString> 的自定义集合类
 * 
 * 特点：
 * - 不使用模板，专门用于 QString 类型
 * - 手动内存管理（new[]/delete[]）
 * - 自动扩容机制（容量不足时2倍扩容）
 * - 维护元素唯一性（插入前检查是否已存在）
 * - 使用线性查找实现（简单实现，适合小规模数据）
 * - 提供完整的拷贝语义和异常安全
 */
class MySetQString {
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
     * @brief 查找元素的索引
     * @param value 要查找的元素
     * @return 如果找到返回索引，否则返回 SIZE_MAX
     */
    size_t findIndex(const QString& value) const;
    
    /**
     * @brief 边界检查
     * @param index 索引
     * @throw VectorException 如果索引越界
     */
    void checkIndex(size_t index) const;

public:
    // ========== 构造函数和析构函数 ==========
    
    /**
     * @brief 默认构造函数
     * @param initialCapacity 初始容量，默认为16
     */
    explicit MySetQString(size_t initialCapacity = 16);
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 MySetQString 对象
     */
    MySetQString(const MySetQString& other);
    
    /**
     * @brief 析构函数
     */
    ~MySetQString();
    
    // ========== 赋值运算符 ==========
    
    /**
     * @brief 赋值运算符
     * @param other 要赋值的 MySetQString 对象
     * @return 返回自身的引用
     */
    MySetQString& operator=(const MySetQString& other);
    
    // ========== 元素访问 ==========
    
    /**
     * @brief 访问指定索引的元素（带边界检查）
     * @param index 索引位置
     * @return 元素的引用
     * @throw VectorException 如果索引越界
     */
    QString& at(size_t index);
    const QString& at(size_t index) const;
    
    // ========== 容量相关 ==========
    
    /**
     * @brief 获取当前元素数量
     * @return 元素数量
     */
    size_t size() const { return m_size; }
    
    /**
     * @brief 获取当前容量
     * @return 容量大小
     */
    size_t capacity() const { return m_capacity; }
    
    /**
     * @brief 判断容器是否为空
     * @return true 如果为空，false 否则
     */
    bool empty() const { return m_size == 0; }
    
    /**
     * @brief 预留容量（如果当前容量小于 newCapacity，则扩容）
     * @param newCapacity 新的容量大小
     */
    void reserve(size_t newCapacity);
    
    // ========== 修改操作 ==========
    
    /**
     * @brief 插入元素（如果元素已存在则不插入）
     * @param value 要插入的元素
     * @return 如果插入成功返回true，如果元素已存在返回false
     */
    bool insert(const QString& value);
    
    /**
     * @brief 删除指定元素
     * @param value 要删除的元素
     * @return 如果找到并删除返回true，否则返回false
     */
    bool erase(const QString& value);
    
    /**
     * @brief 清空所有元素（不释放内存）
     */
    void clear();
    
    // ========== 查找操作 ==========
    
    /**
     * @brief 查找指定元素
     * @param value 要查找的元素
     * @return 如果找到返回true，否则返回false
     */
    bool find(const QString& value) const;
    
    /**
     * @brief 检查是否包含指定元素（同find，语义更清晰）
     * @param value 要检查的元素
     * @return 如果包含返回true，否则返回false
     */
    bool contains(const QString& value) const { return find(value); }
    
    // ========== 辅助方法 ==========
    
    /**
     * @brief 交换两个 MySetQString 对象的内容
     * @param other 要交换的对象
     */
    void swap(MySetQString& other);
};

#endif // MYSETQSTRING_H

