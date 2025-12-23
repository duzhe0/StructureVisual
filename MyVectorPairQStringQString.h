#ifndef MYVECTORPAIRQSTRINGQSTRING_H
#define MYVECTORPAIRQSTRINGQSTRING_H

#include "MyPairQStringQString.h"
#include "VectorException.h"
#include <cstddef>   // for size_t

/**
 * @brief MyVectorPairQStringQString - 用于替代 std::vector<std::pair<QString, QString>> 的自定义动态数组类
 * 
 * 特点：
 * - 不使用模板，专门用于 MyPairQStringQString 类型
 * - 手动内存管理（new[]/delete[]）
 * - 自动扩容机制（容量不足时2倍扩容）
 * - 提供完整的拷贝语义和异常安全
 */
class MyVectorPairQStringQString {
private:
    MyPairQStringQString* m_data;          // 数据数组指针
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
    explicit MyVectorPairQStringQString(size_t initialCapacity = 16);
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 MyVectorPairQStringQString 对象
     */
    MyVectorPairQStringQString(const MyVectorPairQStringQString& other);
    
    /**
     * @brief 析构函数
     */
    ~MyVectorPairQStringQString();
    
    // ========== 赋值运算符 ==========
    
    /**
     * @brief 赋值运算符
     * @param other 要赋值的 MyVectorPairQStringQString 对象
     * @return 返回自身的引用
     */
    MyVectorPairQStringQString& operator=(const MyVectorPairQStringQString& other);
    
    // ========== 元素访问 ==========
    
    /**
     * @brief 访问指定索引的元素（带边界检查）
     * @param index 索引位置
     * @return 元素的引用
     * @throw VectorException 如果索引越界
     */
    MyPairQStringQString& at(size_t index);
    const MyPairQStringQString& at(size_t index) const;
    
    /**
     * @brief 访问指定索引的元素（不检查边界，快速访问）
     * @param index 索引位置
     * @return 元素的引用
     * @warning 不进行边界检查，需确保索引有效
     */
    MyPairQStringQString& operator[](size_t index);
    const MyPairQStringQString& operator[](size_t index) const;
    
    /**
     * @brief 获取第一个元素
     * @return 第一个元素的引用
     * @throw VectorException 如果容器为空
     */
    MyPairQStringQString& front();
    const MyPairQStringQString& front() const;
    
    /**
     * @brief 获取最后一个元素
     * @return 最后一个元素的引用
     * @throw VectorException 如果容器为空
     */
    MyPairQStringQString& back();
    const MyPairQStringQString& back() const;
    
    // ========== 容量相关 ==========
    
    /**
     * @brief 获取容器中元素的数量
     * @return 元素数量
     */
    size_t size() const { return m_size; }
    
    /**
     * @brief 获取容器的容量
     * @return 容量大小
     */
    size_t capacity() const { return m_capacity; }
    
    /**
     * @brief 判断容器是否为空
     * @return true 如果容器为空，false 否则
     */
    bool empty() const { return m_size == 0; }
    
    /**
     * @brief 预留容量（如果 newCapacity > capacity，则扩容）
     * @param newCapacity 新的容量大小
     */
    void reserve(size_t newCapacity);
    
    /**
     * @brief 改变容器大小
     * @param newSize 新的大小
     * @param value 如果新大小大于当前大小，用此值填充新元素
     */
    void resize(size_t newSize, const MyPairQStringQString& value = MyPairQStringQString());
    
    /**
     * @brief 清空容器（移除所有元素）
     */
    void clear();
    
    // ========== 修改操作 ==========
    
    /**
     * @brief 在容器末尾添加元素
     * @param value 要添加的元素
     */
    void push_back(const MyPairQStringQString& value);
    
    /**
     * @brief 移除容器末尾的元素
     * @throw VectorException 如果容器为空
     */
    void pop_back();
    
    /**
     * @brief 交换两个容器的内容
     * @param other 要交换的另一个容器
     */
    void swap(MyVectorPairQStringQString& other);
};

#endif // MYVECTORPAIRQSTRINGQSTRING_H

