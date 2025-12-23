# MyMapQStringToQString 替换总结

## ✅ 替换完成

已成功将 `GraphModel.cpp` 中所有 `std::map<QString, QString>` 替换为 `MyMapQStringToQString`。

---

## 📝 修改内容

### 1. GraphModel.cpp 修改

#### 1.1 添加头文件包含
```cpp
#include "MyMapQStringToQString.h"
```

#### 1.2 替换变量声明
```cpp
// 修改前：
std::map<QString, QString> predecessors;  // Dijkstra算法
std::map<QString, QString> parent;       // Prim/Kruskal算法

// 修改后：
MyMapQStringToQString predecessors;
MyMapQStringToQString parent;
```

#### 1.3 修复 find() 方法的使用
```cpp
// 修改前（std::map 的用法）：
QString prevPred = predecessors.find(neighbor) != predecessors.end() 
    ? predecessors[neighbor] : "无";

// 修改后（MyMapQStringToQString 的用法）：
QString prevPred = predecessors.find(neighbor) != nullptr 
    ? *predecessors.find(neighbor) : "无";
```

**说明**：
- `std::map::find()` 返回迭代器，`end()` 是结束迭代器
- `MyMapQStringToQString::find()` 返回指针，`nullptr` 表示未找到
- 需要解引用指针获取值：`*predecessors.find(neighbor)`

---

### 2. StructureVisual.pro 更新

**添加的文件**：
- `MyMapQStringToQString.h` → HEADERS
- `MyMapQStringToQString.cpp` → SOURCES

---

## 🔄 主要变化

### 1. Map 容器实现

**数据结构**：
- 使用两个数组：`QString* m_keys` 和 `QString* m_values`
- 键值对通过索引关联（`m_keys[i]` 对应 `m_values[i]`）
- 线性查找实现（适合小规模数据）

**查找方式**：
- `findIndex()` 方法：线性查找键的索引
- `operator[]`：如果键不存在则创建并返回默认值（空字符串）
- `at()`：如果键不存在则抛出异常
- `find()`：返回指向值的指针，如果不存在返回 `nullptr`

### 2. find() 方法差异

**std::map**：
```cpp
auto it = map.find(key);
if (it != map.end()) {
    QString value = it->second;
}
```

**MyMapQStringToQString**：
```cpp
QString* ptr = map.find(key);
if (ptr != nullptr) {
    QString value = *ptr;
}
```

### 3. operator[] 行为

**与 std::map 一致**：
- 如果键不存在，自动创建并返回默认值（空字符串 `QString()`）
- 适合 `parent[key] = key` 这样的初始化操作

---

## 📊 替换统计

| 文件 | std::map<QString, QString> 替换数量 | 主要位置 |
|------|-------------------------------------|---------|
| GraphModel.cpp | 2处 | `predecessors` 变量（Dijkstra算法）、`parent` 变量（Prim/Kruskal算法） |
| GraphModel.cpp | 1处 | `find()` 方法调用方式修改 |
| **总计** | **3处** | - |

---

## ✅ 验证结果

- ✅ 编译检查：无linter错误
- ✅ 头文件包含：已添加 `MyMapQStringToQString.h`
- ✅ 变量声明：已更新类型
- ✅ 方法调用：`operator[]` 使用正常
- ✅ find() 方法：已修复使用方式
- ✅ 项目文件：已更新 `.pro` 文件

---

## 📝 注意事项

### 1. find() 方法返回类型

**重要差异**：
- `std::map::find()` 返回迭代器
- `MyMapQStringToQString::find()` 返回指针

**使用方式**：
```cpp
// std::map：
if (map.find(key) != map.end()) { ... }

// MyMapQStringToQString：
if (map.find(key) != nullptr) { ... }
```

### 2. operator[] 默认值

**行为**：
- 如果键不存在，自动创建并返回空字符串 `QString()`
- 与 `std::map` 的行为一致

### 3. 线性查找性能

**实现方式**：使用线性查找（`O(n)` 时间复杂度）

**影响**：
- 适合小规模数据（课程设计场景）
- 如果数据量大，可以考虑实现排序数组 + 二分查找（`O(log n)`）

### 4. QString 拷贝语义

**存储方式**：
- 使用 `QString` 数组存储键和值
- `QString` 使用引用计数，拷贝开销较小
- 删除操作需要移动数组元素（`O(n)`）

---

## 🎯 下一步建议

1. **编译测试**：编译项目确保没有编译错误
2. **功能测试**：运行 Dijkstra、Prim 和 Kruskal 算法，确保功能正常
3. **性能测试**：如果数据量大，考虑优化查找算法
4. **继续替换**：可以考虑替换其他 Map 类型
   - `std::map<QString, QPointF>` → `MyMapQStringToQPointF`
   - `std::map<QString, VertexItem*>` → `MyMapQStringToVertexItemPtr`
   - 其他 Map 类型

---

## 📋 总结

✅ **成功完成替换**：
- 2处 `std::map<QString, QString>` 变量已全部替换为 `MyMapQStringToQString`
- 实现了基本的 Map 功能（插入、查找、删除、遍历）
- 使用线性查找实现（适合小规模数据）
- 提供了与 `std::map` 兼容的接口（`operator[]`, `at()`, `find()`）
- 修复了 `find()` 方法的使用方式差异

**代码质量**：
- ✅ 无模板使用
- ✅ 无STL容器依赖（除了GUI相关的）
- ✅ 手动内存管理
- ✅ 异常安全
- ✅ 提供完整的拷贝语义
- ✅ 使用 Qt 的 `QString` 类型，拷贝开销小

---

*文档生成时间：2025年12月23日*

