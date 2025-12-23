#include "MyVectorQString.h"
#include <QDebug>
#include <QCoreApplication>

// 简单的测试程序
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    qDebug() << "=== MyVectorQString 基本功能测试 ===";
    
    // 测试1：默认构造函数
    qDebug() << "\n测试1：默认构造函数";
    MyVectorQString vec1;
    qDebug() << "size:" << vec1.size() << "capacity:" << vec1.capacity();
    qDebug() << "empty:" << vec1.empty();
    
    // 测试2：push_back
    qDebug() << "\n测试2：push_back";
    vec1.push_back("Hello");
    vec1.push_back("World");
    vec1.push_back("Test");
    qDebug() << "size:" << vec1.size();
    qDebug() << "capacity:" << vec1.capacity();
    
    // 测试3：at访问
    qDebug() << "\n测试3：at访问";
    for (size_t i = 0; i < vec1.size(); ++i) {
        qDebug() << "vec1.at(" << i << ") =" << vec1.at(i);
    }
    
    // 测试4：operator[]
    qDebug() << "\n测试4：operator[]访问";
    for (size_t i = 0; i < vec1.size(); ++i) {
        qDebug() << "vec1[" << i << "] =" << vec1[i];
    }
    
    // 测试5：front和back
    qDebug() << "\n测试5：front和back";
    qDebug() << "front:" << vec1.front();
    qDebug() << "back:" << vec1.back();
    
    // 测试6：拷贝构造函数
    qDebug() << "\n测试6：拷贝构造函数";
    MyVectorQString vec2(vec1);
    qDebug() << "vec2.size():" << vec2.size();
    for (size_t i = 0; i < vec2.size(); ++i) {
        qDebug() << "vec2[" << i << "] =" << vec2[i];
    }
    
    // 测试7：赋值运算符
    qDebug() << "\n测试7：赋值运算符";
    MyVectorQString vec3;
    vec3.push_back("Before");
    vec3 = vec1;
    qDebug() << "vec3.size():" << vec3.size();
    for (size_t i = 0; i < vec3.size(); ++i) {
        qDebug() << "vec3[" << i << "] =" << vec3[i];
    }
    
    // 测试8：clear
    qDebug() << "\n测试8：clear";
    vec3.clear();
    qDebug() << "vec3.size() after clear:" << vec3.size();
    qDebug() << "vec3.empty():" << vec3.empty();
    
    // 测试9：resize
    qDebug() << "\n测试9：resize";
    MyVectorQString vec4;
    vec4.push_back("A");
    vec4.push_back("B");
    qDebug() << "Before resize, size:" << vec4.size();
    vec4.resize(5, "Default");
    qDebug() << "After resize(5), size:" << vec4.size();
    for (size_t i = 0; i < vec4.size(); ++i) {
        qDebug() << "vec4[" << i << "] =" << vec4[i];
    }
    
    // 测试10：异常处理（边界检查）
    qDebug() << "\n测试10：异常处理";
    try {
        vec1.at(100);  // 应该抛出异常
        qDebug() << "ERROR: 应该抛出异常但没有！";
    } catch (const VectorException& e) {
        qDebug() << "成功捕获异常:" << e.what();
    }
    
    qDebug() << "\n=== 所有测试完成 ===";
    
    return 0;
}

