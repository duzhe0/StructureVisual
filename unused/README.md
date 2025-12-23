# unused 文件夹说明

本文件夹包含项目中**未被主程序使用**的代码文件。

## 📁 文件分类

### 1. 模板类文件（未在主程序中使用）

这些文件定义了模板类，但主程序（mainwindow, GraphModel, SortModel等）都没有使用它们。

| 文件 | 说明 | 原因 |
|------|------|------|
| `Graph.h` | 图接口类（模板） | GraphModel使用了自己的实现 |
| `IGraph.h` | 图接口基类 | Graph.h的依赖，未被使用 |
| `ListGraph.h` | 邻接表图实现（模板） | 未被主程序使用 |
| `MatrixGraph.h` | 矩阵图实现（模板） | 未被主程序使用 |
| `HashMap.h` | 哈希映射类（模板） | 只在测试文件中使用 |
| `dynamicarray.h` | 动态数组类（模板） | 只被HashMap.h使用，但HashMap未被主程序使用 |
| `dynamicarray.cpp` | 动态数组模板实例化 | 未被主程序使用 |
| `Iterator.h` | 迭代器接口（模板） | 只被模板类使用 |
| `Iterable.h` | 可迭代接口（模板） | 只被模板类使用 |

**注意**：这些模板类可能是早期设计的一部分，但后来GraphModel改用Qt的QMap和STL实现，所以这些模板类被保留但未集成到主程序中。

### 2. 测试文件（未编译）

这些是独立的测试程序，不在 `StructureVisual.pro` 文件中，需要单独编译运行。

| 文件 | 说明 |
|------|------|
| `test_MyVectorQString.cpp` | MyVectorQString类的测试程序 |
| `test_dynamicarray.cpp` | DynamicArray类的测试程序 |
| `test_hashmap.cpp` | HashMap类的测试程序 |
| `test_simplestring.cpp` | SimpleString类的测试程序 |
| `demo_bfs.cpp` | BFS算法演示程序 |
| `simple_test.cpp` | 简单测试程序 |
| `simple_visual_test.cpp` | 可视化测试程序 |
| `minimal_test.cpp` | 最小测试程序 |

## 🔄 移动记录

**移动时间**：2025年12月23日

**移动原因**：
1. 这些文件未被主程序使用，但仍在 `.pro` 文件中
2. 为了保持项目整洁，将它们移动到 `unused` 文件夹
3. 如果将来需要，可以轻松找到并恢复

## 📝 项目文件更新

已更新 `StructureVisual.pro`：
- ✅ 从 `HEADERS` 中移除了未使用的模板类头文件
- ✅ 从 `SOURCES` 中移除了未使用的源文件
- ✅ 添加了 `MyVectorQString.h` 和 `MyVectorQString.cpp`

## ⚠️ 注意事项

1. **不要删除这些文件**：它们可能在未来需要，或者用于参考
2. **依赖关系**：这些文件之间有依赖关系，如果恢复使用，需要一起恢复
3. **编译测试**：移动后项目应该仍然可以正常编译

## 🔍 如何恢复使用

如果需要恢复使用这些文件：

1. 将文件从 `unused` 文件夹移回项目根目录
2. 在 `StructureVisual.pro` 中添加相应的文件引用
3. 确保所有依赖关系正确

---

*文档创建时间：2025年12月23日*

