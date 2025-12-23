#ifndef MYPAIRQSTRINGQSTRING_H
#define MYPAIRQSTRINGQSTRING_H

#include <QString>

/**
 * @brief MyPairQStringQString - 用于替代 std::pair<QString, QString> 的自定义Pair类
 * 
 * 特点：
 * - 不使用模板，专门用于 QString, QString 的配对
 * - 提供 first 和 second 成员变量
 * - 支持比较运算符（用于Map的键比较）
 * - 支持拷贝构造和赋值
 */
class MyPairQStringQString {
public:
    QString first;
    QString second;
    
    // ========== 构造函数 ==========
    
    /**
     * @brief 默认构造函数
     */
    MyPairQStringQString() : first(), second() {}
    
    /**
     * @brief 构造函数
     * @param f 第一个QString
     * @param s 第二个QString
     */
    MyPairQStringQString(const QString& f, const QString& s) : first(f), second(s) {}
    
    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的MyPairQStringQString对象
     */
    MyPairQStringQString(const MyPairQStringQString& other) : first(other.first), second(other.second) {}
    
    // ========== 赋值运算符 ==========
    
    /**
     * @brief 赋值运算符
     * @param other 要赋值的MyPairQStringQString对象
     * @return 返回自身的引用
     */
    MyPairQStringQString& operator=(const MyPairQStringQString& other) {
        if (this != &other) {
            first = other.first;
            second = other.second;
        }
        return *this;
    }
    
    // ========== 比较运算符 ==========
    
    /**
     * @brief 小于运算符（用于Map的键比较）
     * @param other 要比较的MyPairQStringQString对象
     * @return true 如果当前对象小于other，否则返回false
     */
    bool operator<(const MyPairQStringQString& other) const {
        if (first < other.first) {
            return true;
        } else if (first == other.first) {
            return second < other.second;
        }
        return false;
    }
    
    /**
     * @brief 等于运算符
     * @param other 要比较的MyPairQStringQString对象
     * @return true 如果两个对象相等，否则返回false
     */
    bool operator==(const MyPairQStringQString& other) const {
        return first == other.first && second == other.second;
    }
    
    /**
     * @brief 不等于运算符
     * @param other 要比较的MyPairQStringQString对象
     * @return true 如果两个对象不相等，否则返回false
     */
    bool operator!=(const MyPairQStringQString& other) const {
        return !(*this == other);
    }
};

#endif // MYPAIRQSTRINGQSTRING_H

