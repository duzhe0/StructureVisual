#ifndef MYMAPPAIRTOEDGEITEMPTR_H
#define MYMAPPAIRTOEDGEITEMPTR_H

#include <QString>
#include <cstddef>   // for size_t
#include "VectorException.h"
#include "MyPairQStringQString.h"

// 前向声明
class EdgeItem;

/**
 * @brief MyMapPairToEdgeItemPtr - 用于替代 std::map<std::pair<QString, QString>, EdgeItem*> 的自定义映射类
 * 
 * 特点：
 * - 不使用模板，专门用于 MyPairQStringQString -> EdgeItem* 的映射
 * - 手动内存管理（new[]/delete[]）
 * - 自动扩容机制（容量不足时2倍扩容）
 * - 提供完整的拷贝语义和异常安全
 * - 使用线性查找（简单实现，适合小规模数据）
 * - **不拥有指针所有权**：只存储指针，不负责删除指针指向的对象
 */
class MyMapPairToEdgeItemPtr {
private:
    MyPairQStringQString* m_keys;  // 键数组
    EdgeItem** m_values;           // 值数组（指针数组）
    size_t m_size;                 // 当前元素数量
    size_t m_capacity;            // 当前容量
    
    /**
     * @brief 重新分配内存（扩容）
     * @param newCapacity 新的容量大小
     */
    void reallocate(size_t newCapacity);
    
    /**
     * @brief 查找键的索引
     * @param key 要查找的键
     * @return 如果找到返回索引，否则返回 SIZE_MAX
     */
    size_t findIndex(const MyPairQStringQString& key) const;

public:
    // ========== 构造函数和析构函数 ==========
    
    /**
     * @brief 默认构造函数
     * @param initialCapacity 初始容量，默认为16
     */
    explicit MyMapPairToEdgeItemPtr(size_t initialCapacity = 16);
    
    /**
     * @brief 拷贝构造函数（浅拷贝：只拷贝指针值）
     * @param other 要拷贝的 MyMapPairToEdgeItemPtr 对象
     */
    MyMapPairToEdgeItemPtr(const MyMapPairToEdgeItemPtr& other);
    
    /**
     * @brief 析构函数（不删除指针指向的对象，只释放数组内存）
     */
    ~MyMapPairToEdgeItemPtr();
    
    // ========== 赋值运算符 ==========
    
    /**
     * @brief 赋值运算符（浅拷贝：只拷贝指针值）
     * @param other 要赋值的 MyMapPairToEdgeItemPtr 对象
     * @return 返回自身的引用
     */
    MyMapPairToEdgeItemPtr& operator=(const MyMapPairToEdgeItemPtr& other);
    
    // ========== 元素访问 ==========
    
    /**
     * @brief 访问指定键的元素（如果不存在则创建）
     * @param key 键
     * @return 值的引用（EdgeItem*&）
     */
    EdgeItem*& operator[](const MyPairQStringQString& key);
    
    /**
     * @brief 访问指定键的元素（只读，如果不存在则抛出异常）
     * @param key 键
     * @return 值的引用
     * @throw VectorException 如果键不存在
     */
    EdgeItem*& at(const MyPairQStringQString& key);
    EdgeItem* const& at(const MyPairQStringQString& key) const;
    
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
     * @brief 插入键值对
     * @param key 键
     * @param value 值（指针）
     * @return 如果键已存在返回false，否则返回true
     */
    bool insert(const MyPairQStringQString& key, EdgeItem* value);
    
    /**
     * @brief 删除指定键的元素
     * @param key 要删除的键
     * @return 如果找到并删除返回true，否则返回false
     */
    bool erase(const MyPairQStringQString& key);
    
    /**
     * @brief 清空所有元素（不释放内存，不删除指针指向的对象）
     */
    void clear();
    
    // ========== 查找操作 ==========
    
    /**
     * @brief 查找指定键的元素
     * @param key 要查找的键
     * @return 如果找到返回指向值的指针，否则返回nullptr
     */
    EdgeItem** find(const MyPairQStringQString& key);
    EdgeItem* const* find(const MyPairQStringQString& key) const;
    
    /**
     * @brief 检查是否包含指定键
     * @param key 要检查的键
     * @return 如果包含返回true，否则返回false
     */
    bool contains(const MyPairQStringQString& key) const;
    
    /**
     * @brief 获取指定索引的键（用于遍历）
     * @param index 索引
     * @return 键的引用
     * @throw VectorException 如果索引越界
     */
    MyPairQStringQString& keyAt(size_t index);
    const MyPairQStringQString& keyAt(size_t index) const;
    
    /**
     * @brief 获取指定索引的值（用于遍历）
     * @param index 索引
     * @return 值的引用（EdgeItem*&）
     * @throw VectorException 如果索引越界
     */
    EdgeItem*& valueAt(size_t index);
    EdgeItem* const& valueAt(size_t index) const;
    
    // ========== 辅助方法 ==========
    
    /**
     * @brief 交换两个 MyMapPairToEdgeItemPtr 对象的内容
     * @param other 要交换的对象
     */
    void swap(MyMapPairToEdgeItemPtr& other);
    
    /**
     * @brief 模拟 end() 方法，用于兼容 find() != end() 的用法
     * @return 返回一个特殊值，用于与 find() 比较
     * @note find() 返回 nullptr 表示未找到，可以用 find() != nullptr 代替 find() != end()
     */
    static EdgeItem** end() { return nullptr; }
};

#endif // MYMAPPAIRTOEDGEITEMPTR_H

