#ifndef MYQUEUEALGORITHMSTEP_H
#define MYQUEUEALGORITHMSTEP_H

#include <cstddef>   // for size_t
#include "VectorException.h"

// 前向声明
struct AlgorithmStep;

/**
 * @brief MyQueueAlgorithmStep - 用于替代 std::queue<AlgorithmStep> 的自定义队列类
 * 
 * 特点：
 * - 不使用模板，专门用于 AlgorithmStep 类型
 * - 手动内存管理（new[]/delete[]）
 * - 使用循环数组（circular buffer）实现，支持高效入队和出队
 * - 自动扩容机制（容量不足时2倍扩容）
 * - 提供完整的拷贝语义和异常安全
 */
class MyQueueAlgorithmStep {
private:
    AlgorithmStep* m_data;          // 数据数组指针（循环数组）
    size_t m_size;                 // 当前元素数量
    size_t m_capacity;              // 当前容量
    size_t m_front;                 // 队首索引
    size_t m_back;                  // 队尾索引（下一个插入位置）
    
    /**
     * @brief 重新分配内存（扩容）
     * @param newCapacity 新的容量大小
     */
    void reallocate(size_t newCapacity);
    
    /**
     * @brief 检查队列是否为空（用于异常检查）
     * @throw VectorException 如果队列为空
     */
    void checkNotEmpty() const;

public:
    // ========== 构造函数和析构函数 ==========
    
    /**
     * @brief 默认构造函数
     * @param initialCapacity 初始容量，默认为16
     */
    explicit MyQueueAlgorithmStep(size_t initialCapacity = 16);
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 MyQueueAlgorithmStep 对象
     */
    MyQueueAlgorithmStep(const MyQueueAlgorithmStep& other);
    
    /**
     * @brief 析构函数
     */
    ~MyQueueAlgorithmStep();
    
    /**
     * @brief 赋值运算符
     * @param other 要赋值的 MyQueueAlgorithmStep 对象
     * @return 返回自身的引用
     */
    MyQueueAlgorithmStep& operator=(const MyQueueAlgorithmStep& other);
    
    // ========== 队列操作 ==========
    
    /**
     * @brief 将元素加入队尾
     * @param value 要加入的 AlgorithmStep 值
     */
    void push(const AlgorithmStep& value);
    
    /**
     * @brief 移除队首元素
     * @throw VectorException 如果队列为空
     */
    void pop();
    
    /**
     * @brief 访问队首元素（非const版本）
     * @return 队首元素的引用
     * @throw VectorException 如果队列为空
     */
    AlgorithmStep& front();
    
    /**
     * @brief 访问队首元素（const版本）
     * @return 队首元素的const引用
     * @throw VectorException 如果队列为空
     */
    const AlgorithmStep& front() const;
    
    /**
     * @brief 访问队尾元素（非const版本）
     * @return 队尾元素的引用
     * @throw VectorException 如果队列为空
     */
    AlgorithmStep& back();
    
    /**
     * @brief 访问队尾元素（const版本）
     * @return 队尾元素的const引用
     * @throw VectorException 如果队列为空
     */
    const AlgorithmStep& back() const;
    
    // ========== 容量和状态查询 ==========
    
    /**
     * @brief 获取队列中元素的数量
     * @return 元素数量
     */
    size_t size() const { return m_size; }
    
    /**
     * @brief 获取队列的容量
     * @return 容量大小
     */
    size_t capacity() const { return m_capacity; }
    
    /**
     * @brief 判断队列是否为空
     * @return true 如果队列为空，false 否则
     */
    bool empty() const { return m_size == 0; }
    
    /**
     * @brief 预留容量（如果 newCapacity > capacity，则扩容）
     * @param newCapacity 新的容量大小
     */
    void reserve(size_t newCapacity);
    
    /**
     * @brief 清空队列（移除所有元素）
     */
    void clear();
    
    /**
     * @brief 交换两个队列的内容
     * @param other 要交换的另一个队列
     */
    void swap(MyQueueAlgorithmStep& other);
};

#endif // MYQUEUEALGORITHMSTEP_H

