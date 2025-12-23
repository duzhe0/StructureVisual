#ifndef MYVECTORINT_H
#define MYVECTORINT_H
#include <cstddef>   // for size_t
#include "VectorException.h"

/**
 * @brief MyVectorInt - 用于替代 std::vector<int> 的自定义动态数组类
 * 
 * 特点：
 * - 不使用模板，专门用于 int 类型
 * - 手动内存管理（new[]/delete[]）
 * - 自动扩容机制（容量不足时2倍扩容）
 * - 提供完整的拷贝语义和异常安全
 */
class MyVectorInt {
private:
    int* m_data;              // 数据数组指针
    size_t m_size;            // 当前元素数量
    size_t m_capacity;         // 当前容量
    
    /**
     * @brief 重新分配内存（扩容）
     * @param newCapacity 新的容量大小
     */
    void reallocate(size_t newCapacity);
    
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
    explicit MyVectorInt(size_t initialCapacity = 16);
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 MyVectorInt 对象
     */
    MyVectorInt(const MyVectorInt& other);
    
    /**
     * @brief 析构函数
     */
    ~MyVectorInt();
    
    // ========== 赋值运算符 ==========
    
    /**
     * @brief 赋值运算符
     * @param other 要赋值的 MyVectorInt 对象
     * @return 返回自身的引用
     */
    MyVectorInt& operator=(const MyVectorInt& other);
    
    // ========== 元素访问 ==========
    
    /**
     * @brief 访问指定索引的元素（带边界检查）
     * @param index 索引位置
     * @return 元素的引用
     * @throw VectorException 如果索引越界
     */
    int& at(size_t index);
    const int& at(size_t index) const;
    
    /**
     * @brief 访问指定索引的元素（不检查边界，快速访问）
     * @param index 索引位置
     * @return 元素的引用
     * @warning 不进行边界检查，需确保索引有效
     */
    int& operator[](size_t index);
    const int& operator[](size_t index) const;
    
    /**
     * @brief 获取第一个元素
     * @return 第一个元素的引用
     * @throw VectorException 如果容器为空
     */
    int& front();
    const int& front() const;
    
    /**
     * @brief 获取最后一个元素
     * @return 最后一个元素的引用
     * @throw VectorException 如果容器为空
     */
    int& back();
    const int& back() const;
    
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
    
    /**
     * @brief 调整大小
     * @param newSize 新的大小
     * @param value 如果新大小大于当前大小，用此值填充新元素（默认为0）
     */
    void resize(size_t newSize, int value = 0);
    
    // ========== 修改操作 ==========
    
    /**
     * @brief 在末尾添加元素
     * @param value 要添加的 int 值
     */
    void push_back(int value);
    
    /**
     * @brief 移除最后一个元素
     * @throw VectorException 如果容器为空
     */
    void pop_back();
    
    /**
     * @brief 清空所有元素（不释放内存）
     */
    void clear();
    
    /**
     * @brief 在指定位置插入元素
     * @param index 插入位置
     * @param value 要插入的值
     * @throw VectorException 如果索引越界
     */
    void insert(size_t index, int value);
    
    /**
     * @brief 删除指定位置的元素
     * @param index 要删除的位置
     * @throw VectorException 如果索引越界
     */
    void erase(size_t index);
    
    // ========== 辅助方法 ==========
    
    /**
     * @brief 交换两个 MyVectorInt 对象的内容
     * @param other 要交换的对象
     */
    void swap(MyVectorInt& other);
    
    /**
     * @brief 获取数据指针（用于与C风格API交互）
     * @return 数据指针
     */
    int* data() { return m_data; }
    const int* data() const { return m_data; }
};

#endif // MYVECTORINT_H

