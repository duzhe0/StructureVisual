#ifndef VECTOREXCEPTION_H
#define VECTOREXCEPTION_H

/**
 * @brief VectorException - 自定义异常类，用于替代 std::exception
 * 
 * 用于 MyVectorInt 和 MyVectorQString 等自定义容器类
 */
class VectorException {
private:
    const char* m_message;
public:
    VectorException(const char* msg) : m_message(msg) {}
    const char* what() const { return m_message; }
};

#endif // VECTOREXCEPTION_H

