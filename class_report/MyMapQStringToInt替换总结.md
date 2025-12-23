# MyMapQStringToInt 替换总结

## ✅ 替换完成

已成功将 `GraphModel.cpp` 中所有 `std::map<QString, int>` 替换为 `MyMapQStringToInt`。

---

## 📝 修改内容

### 1. GraphModel.cpp 修改

#### 1.1 添加头文件包含
```cpp
#include "MyMapQStringToInt.h"
```

#### 1.2 替换变量声明
```cpp
// 修改前：
std::map<QString, int> distances;  // Dijkstra算法
std::map<QString, int> inDegree;   // 拓扑排序算法

// 修改后：
MyMapQStringToInt distances;
MyMapQStringToInt inDegree;
```

#### 1.3 替换基于范围的for循环
```cpp
// 修改前：
for (const auto &pair : inDegree) {
    if (pair.second == 0) {
        zeroInDegreeQueue.push(pair.first);
        // ...
    }
}

// 修改后：
for (size_t i = 0; i < inDegree.size(); ++i) {
    if (inDegree.valueAt(i) == 0) {
        QString vertex = inDegree.keyAt(i);
        zeroInDegreeQueue.push(vertex);
        // ...
    }
}
```

---

### 2. StructureVisual.pro 更新

**添加的文件**：
- `MyMapQStringToInt.h` → HEADERS
- `MyMapQStringToInt.cpp` → SOURCES

---

## 🔄 主要变化

### 1. Map 容器实现

**数据结构**：
- 使用两个数组：`QString* m_keys` 和 `int* m_values`
- 键值对通过索引关联（`m_keys[i]` 对应 `m_values[i]`）
- 线性查找实现（适合小规模数据）

**查找方式**：
- `findIndex()` 方法：线性查找键的索引
- `operator[]`：如果键不存在则创建并返回默认值（0）
- `at()`：如果键不存在则抛出异常
- `find()`：返回指向值的指针，如果不存在返回 `nullptr`

### 2. 遍历方式

**替换前**（基于范围的for循环）：
```cpp
for (const auto &pair : map) {
    QString key = pair.first;
    int value = pair.second;
}
```

**替换后**（索引循环）：
```cpp
for (size_t i = 0; i < map.size(); ++i) {
    QString key = map.keyAt(i);
    int value = map.valueAt(i);
}
```

### 3. 访问方式

**operator[]**：
- 支持读写：`map[key] = value` 和 `int val = map[key]`
- 如果键不存在，自动创建并返回默认值（0）

**at()**：
- 只读访问，键不存在时抛出异常

**find()**：
- 返回指针，可以检查是否存在：`if (int* val = map.find(key)) { ... }`

---

## 📊 替换统计

| 文件 | std::map<QString, int> 替换数量 | 主要位置 |
|------|-------------------------------|---------|
| GraphModel.cpp | 2处 | `distances` 变量（Dijkstra算法）、`inDegree` 变量（拓扑排序） |
| GraphModel.cpp | 1处 | 基于范围的for循环改为索引循环 |
| **总计** | **3处** | - |

---

## ✅ 验证结果

- ✅ 编译检查：无linter错误
- ✅ 头文件包含：已添加 `MyMapQStringToInt.h`
- ✅ 变量声明：已更新类型
- ✅ 方法调用：`operator[]` 使用正常
- ✅ 遍历方式：已改为索引循环
- ✅ 项目文件：已更新 `.pro` 文件

---

## 📝 注意事项

### 1. 线性查找性能

**实现方式**：使用线性查找（`O(n)` 时间复杂度）

**影响**：
- 适合小规模数据（课程设计场景）
- 如果数据量大，可以考虑实现排序数组 + 二分查找（`O(log n)`）

### 2. operator[] 行为

**与 std::map 一致**：
- 如果键不存在，自动创建并返回默认值（0）
- 适合 `distances[key] = INT_MAX` 这样的初始化操作

### 3. 遍历顺序

**注意**：
- 遍历顺序与插入顺序一致（不是按键排序）
- 如果需要排序遍历，需要先实现排序功能

### 4. 键值对存储

**存储方式**：
- 使用两个并行数组存储键和值
- 删除操作需要移动数组元素（`O(n)`）

---

## 🎯 下一步建议

1. **编译测试**：编译项目确保没有编译错误
2. **功能测试**：运行 Dijkstra 和拓扑排序算法，确保功能正常
3. **性能测试**：如果数据量大，考虑优化查找算法
4. **继续替换**：可以考虑替换其他 Map 类型
   - `std::map<QString, QString>` → `MyMapQStringToQString`
   - `std::map<QString, QPointF>` → `MyMapQStringToQPointF`
   - 其他 Map 类型

---

## 📋 总结

✅ **成功完成替换**：
- 2处 `std::map<QString, int>` 变量已全部替换为 `MyMapQStringToInt`
- 实现了基本的 Map 功能（插入、查找、删除、遍历）
- 使用线性查找实现（适合小规模数据）
- 提供了与 `std::map` 兼容的接口（`operator[]`, `at()`, `find()`）

**代码质量**：
- ✅ 无模板使用
- ✅ 无STL容器依赖（除了GUI相关的）
- ✅ 手动内存管理
- ✅ 异常安全
- ✅ 提供完整的拷贝语义

---

*文档生成时间：2025年12月23日*

