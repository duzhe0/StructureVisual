#ifndef EDGEINFO_H
#define EDGEINFO_H

#include <QString>

/**
 * @brief EdgeInfo - 边信息结构体
 * 用于存储边的起点、终点和权重信息
 */
struct EdgeInfo {
    QString from;    // 起点
    QString to;      // 终点
    int weight;      // 权重
    
    /**
     * @brief 默认构造函数
     */
    EdgeInfo() : from(""), to(""), weight(0) {}
    
    /**
     * @brief 带参数的构造函数
     * @param f 起点
     * @param t 终点
     * @param w 权重
     */
    EdgeInfo(const QString &f, const QString &t, int w) : from(f), to(t), weight(w) {}
};

#endif // EDGEINFO_H

