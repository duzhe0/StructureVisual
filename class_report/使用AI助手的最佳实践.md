# StructureVisual 项目状态文档

> **文档性质**：项目唯一事实源（Single Source of Truth）  
> **更新原则**：每次任务完成后必须更新此文档  
> **使用方式**：AI助手在回答任何问题前必须先阅读此文档

---

## 📋 项目概述

**项目名称**：StructureVisual（数据结构可视化模拟器）  
**项目目标**：去除所有STL标准库和模板使用，满足课程设计要求  
**核心约束**：不能使用类库、模板等（用户界面除外）

---

## 🎯 大任务（Milestones）

### Milestone 1: 自定义容器类实现与替换
**状态**：进行中  
**负责人**：AI + 人类协作  
**开始时间**：2025年12月  
**目标**：实现非模板的自定义容器类，替换项目中的STL容器

#### 输入条件（Prerequisites）
- ✅ 项目代码已分析完成
- ✅ STL使用情况已梳理（见`不满足编码要求的地方.md`）
- ✅ 代码整理完成（未使用代码已移至`unused`文件夹）

#### 输出结果（Deliverables）
- 自定义容器类实现（.h/.cpp文件）
- 项目代码中的STL替换完成
- 编译通过，功能正常

---

## 📝 小任务（Tasks）

### Task 1.1: MyVectorQString 实现与替换
**状态**：✅ 已完成  
**负责人**：AI  
**完成时间**：2025年12月

#### 子任务（Subtasks）
- ✅ **Subtask 1.1.1**: 设计 MyVectorQString 接口（头文件）
  - 输出：`MyVectorInt.h` 接口设计
  - 参考：`MyVectorQString.h` 的设计风格
  
- ✅ **Subtask 1.1.2**: 实现 MyVectorQString 基本功能
  - 输出：`MyVectorQString.cpp` 基本实现
  - 功能：构造函数、析构函数、push_back、at、size、operator[]
  
- ✅ **Subtask 1.1.3**: 实现 MyVectorQString 高级功能
  - 输出：完整的 `MyVectorQString.cpp`
  - 功能：拷贝构造、赋值运算符、clear、resize、empty、front、back、pop_back、reserve、异常处理
  
- ✅ **Subtask 1.1.4**: 在 GraphModel 中替换 std::vector<QString>
  - 输出：`GraphModel.cpp/h` 修改完成
  - 替换位置：`getNeighbors()` 方法及其所有调用处
  - 循环方式：基于范围的for循环 → 索引循环

#### 备注（Context）
- 参考文档：`getNeighbors替换总结.md`
- 替换了6个算法中的调用（DFS, BFS, Dijkstra, Prim, 拓扑排序, hasPath）

---

### Task 1.2: MyVectorInt 实现与替换
**状态**：✅ 已完成  
**负责人**：AI  
**完成时间**：2025年12月

#### 子任务（Subtasks）
- ✅ **Subtask 1.2.1**: 设计 MyVectorInt 接口（头文件）
  - 输出：`MyVectorInt.h` 接口设计
  - 参考：`MyVectorQString.h` 的设计风格
  
- ✅ **Subtask 1.2.2**: 实现 MyVectorInt 基本功能
  - 输出：`MyVectorInt.cpp` 基本实现
  - 功能：构造函数、析构函数、push_back、at、size、operator[]
  
- ✅ **Subtask 1.2.3**: 实现 MyVectorInt 高级功能
  - 输出：完整的 `MyVectorInt.cpp`
  - 功能：赋值运算符、clear、resize、empty、front、back、pop_back、reserve、异常处理
  
- ✅ **Subtask 1.2.4**: 在 SortModel 中替换 std::vector<int>
  - 输出：`SortModel.cpp/h` 修改完成
  - 替换位置：
    - 成员变量：`m_data`、`m_originalData`、`m_auxiliaryArray`
    - `SortStep` 结构体：`indices`、`values`、`newPositions`
    - 方法参数和局部变量（20+处）
    - `AlgorithmController.h` 中的 `setCustomData` 方法
  - 循环方式：基于范围的for循环 → 索引循环
  - 初始化列表：`{i, j}` → 显式创建 MyVectorInt 对象
  - std::swap：改为手动交换

#### 备注（Context）
- 参考文档：`MyVectorInt替换总结.md`、`MyVectorInt功能确认.md`
- 替换了33+处 `std::vector<int>` 使用
- 涉及所有排序算法（冒泡、选择、插入、快速、归并、堆、基数）

---

### Task 1.3: 编译错误修复
**状态**：✅ 已完成  
**负责人**：AI  
**完成时间**：2025年12月

#### 子任务（Subtasks）
- ✅ **Subtask 1.3.1**: 修复 VectorException 重复定义错误
  - 问题：`MyVectorInt.h` 和 `MyVectorQString.h` 都定义了 `VectorException` 类
  - 解决方案：创建单独的 `VectorException.h` 文件
  - 输出：`VectorException.h`、更新 `MyVectorInt.h`、`MyVectorQString.h`
  
- ✅ **Subtask 1.3.2**: 修复 begin()/end() 未声明错误
  - 问题：`SortModel.cpp:248` 使用了基于范围的for循环遍历 `MyVectorInt`
  - 解决方案：改为索引循环
  - 输出：`SortModel.cpp` 修改
  
- ✅ **Subtask 1.3.3**: 删除未使用的辅助函数
  - 问题：`makeVectorInt` 函数定义了但未使用
  - 解决方案：删除函数定义
  - 输出：`SortModel.cpp` 修改

#### 备注（Context）
- 参考文档：`编译错误修复总结.md`
- 所有编译错误已修复，项目可以正常编译

---

### Task 1.4: 代码整理
**状态**：✅ 已完成  
**负责人**：AI  
**完成时间**：2025年12月

#### 子任务（Subtasks）
- ✅ **Subtask 1.4.1**: 识别未使用的代码文件
  - 输出：未使用文件清单
  - 参考文档：`代码使用情况分析.md`
  
- ✅ **Subtask 1.4.2**: 创建 unused 文件夹并移动文件
  - 输出：`unused/` 文件夹，包含17个未使用文件
  - 文件类型：模板类、测试文件
  
- ✅ **Subtask 1.4.3**: 更新项目文件
  - 输出：`StructureVisual.pro` 更新
  - 操作：移除未使用文件的引用

#### 备注（Context）
- 参考文档：`代码整理总结.md`、`unused/README.md`
- 未使用的模板类已移至 `unused` 文件夹，不影响主程序编译

---

### Task 1.5: GraphModel 中其他STL容器替换
**状态**：✅ 已完成  
**负责人**：AI  
**优先级**：高

#### 子任务（Subtasks）
- ✅ **Subtask 1.5.1**: MyVectorVertexItemPtr 实现与替换
  - 输出：`MyVectorVertexItemPtr.h`、`MyVectorVertexItemPtr.cpp`
  - 替换位置：
    - `GraphModel.h`：`getAllVertices()` 方法返回类型
    - `GraphModel.cpp`：`getAllVertices()` 方法实现
    - `AlgorithmController.cpp`：调用处（使用 `empty()` 和 `operator[]`）
  - 所有权语义：不拥有指针所有权，只存储指针值
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.2**: MyVectorEdgeItemPtr 实现与替换
  - 输出：`MyVectorEdgeItemPtr.h`、`MyVectorEdgeItemPtr.cpp`
  - 替换位置：
    - `GraphModel.h`：`getAllEdges()` 方法返回类型
    - `GraphModel.cpp`：`getAllEdges()` 方法实现
  - 所有权语义：不拥有指针所有权，只存储指针值
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.3**: MyMapQStringToInt 实现与替换
  - 输出：`MyMapQStringToInt.h`、`MyMapQStringToInt.cpp`
  - 替换位置：
    - `GraphModel.cpp:557` - `distances` 变量（Dijkstra算法）
    - `GraphModel.cpp:856` - `inDegree` 变量（拓扑排序算法）
  - 实现方式：使用两个数组（键数组和值数组），线性查找
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.4**: MyMapQStringToQString 实现与替换
  - 输出：`MyMapQStringToQString.h`、`MyMapQStringToQString.cpp`
  - 替换位置：
    - `GraphModel.cpp:560` - `predecessors` 变量（Dijkstra算法）
    - `GraphModel.cpp:789` - `parent` 变量（Prim/Kruskal算法）
  - 实现方式：使用两个数组（键数组和值数组），线性查找
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.5**: MyMapQStringToQPointF 实现与替换
  - 输出：`MyMapQStringToQPointF.h`、`MyMapQStringToQPointF.cpp`
  - 替换位置：
    - `GraphModel.cpp:963` - `forces` 变量（力导向布局算法）
  - 实现方式：使用两个数组（键数组和值数组），线性查找
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.6**: MyMapQStringToVertexItemPtr 实现与替换
  - 输出：`MyMapQStringToVertexItemPtr.h`、`MyMapQStringToVertexItemPtr.cpp`
  - 替换位置：
    - `GraphModel.h:125` - `m_vertices` 成员变量
    - `GraphModel.cpp` - 所有 `m_vertices` 的使用（约30+处）
  - 实现方式：使用两个数组（键数组和值数组），线性查找
  - 所有权语义：不拥有指针所有权，只存储指针值
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.7**: MyMapPairToEdgeItemPtr 实现与替换
  - 输出：`MyPairQStringQString.h`、`MyMapPairToEdgeItemPtr.h`、`MyMapPairToEdgeItemPtr.cpp`
  - 替换位置：
    - `GraphModel.h:127` - `m_edges` 成员变量
    - `GraphModel.cpp` - 所有 `m_edges` 的使用（约40+处）
  - 实现方式：使用两个数组（键数组和值数组），线性查找
  - 键类型：`MyPairQStringQString`（自定义Pair类）
  - 所有权语义：不拥有指针所有权，只存储指针值
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.8**: MySetQString 实现与替换
  - 输出：`MySetQString.h`、`MySetQString.cpp`
  - 替换位置：
    - `GraphModel.h:144` - `m_visitedVertices` 成员变量
    - `GraphModel.cpp` - 所有 `std::set<QString>` 局部变量的使用（约5+处）
  - 实现方式：使用动态数组 + 线性查找，维护元素唯一性
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.9**: MySetPairQStringQString 实现与替换
  - 输出：`MySetPairQStringQString.h`、`MySetPairQStringQString.cpp`
  - 替换位置：
    - `GraphModel.h:145` - `m_visitedEdges` 成员变量
    - `GraphModel.cpp` - 所有 `std::set<std::pair<QString, QString>>` 局部变量的使用（约3+处）
  - 实现方式：使用动态数组 + 线性查找，维护元素唯一性
  - 元素类型：`MyPairQStringQString`（自定义Pair类）
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.10**: MyMapQStringToSetQString 实现与替换
  - 输出：`MyMapQStringToSetQString.h`、`MyMapQStringToSetQString.cpp`
  - 替换位置：
    - `GraphModel.h:131` - `m_adjacencyList` 成员变量
    - `GraphModel.cpp` - 所有 `m_adjacencyList` 的使用（约5+处）
  - 实现方式：使用两个数组（键数组和值数组），线性查找
  - 值类型：`MySetQString`（嵌套容器，深拷贝）
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.11**: MyQueueQString 实现与替换
  - 输出：`MyQueueQString.h`、`MyQueueQString.cpp`
  - 替换位置：
    - `GraphModel.h:149` - `m_bfsQueue` 成员变量
    - `GraphModel.cpp` - 所有 `std::queue<QString>` 局部变量的使用（约3+处）
      - `generateBFSSteps()` - `queue` 局部变量
      - `generateTopologicalSortSteps()` - `zeroInDegreeQueue` 局部变量
      - `hasPath()` - `queue` 局部变量
  - 实现方式：使用循环数组（circular buffer），维护 front 和 back 索引
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.12**: MyQueueAlgorithmStep 实现与替换
  - 输出：`MyQueueAlgorithmStep.h`、`MyQueueAlgorithmStep.cpp`
  - 替换位置：
    - `GraphModel.h:143` - `m_algorithmSteps` 成员变量
    - `GraphModel.cpp` - 所有 `m_algorithmSteps` 的使用（约9处）
      - `empty()` - 检查队列是否为空
      - `front()` - 获取队首元素
      - `pop()` - 移除队首元素
      - `push()` - 添加元素到队尾
  - 实现方式：使用循环数组（circular buffer），维护 front 和 back 索引
  - 注意事项：`AlgorithmStep` 结构体内部仍使用 `std::vector`，但队列容器本身已替换
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.13**: MyStackQString 实现与替换
  - 输出：`MyStackQString.h`、`MyStackQString.cpp`
  - 替换位置：
    - `GraphModel.h:152` - `m_dfsStack` 成员变量
    - `GraphModel.cpp` - 所有 `std::stack<QString>` 的使用（约2处）
      - `generateDFSSteps()` - `stack` 局部变量（约5处操作：push, top, pop, empty）
      - `resetAlgorithmState()` - `m_dfsStack.pop()` 循环清空
  - 实现方式：使用动态数组，在数组末尾操作（LIFO）
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.14**: AlgorithmStep 结构体和 GraphModel 中剩余 std::vector 替换
  - 输出：`MyVectorPairQStringQString.h`、`MyVectorPairQStringQString.cpp`
  - 替换位置：
    - `GraphModel.h:45-48` - `AlgorithmStep` 结构体中的 `std::vector<QString>` 和 `std::vector<std::pair<QString, QString>>`
    - `GraphModel.h:166-168` - `addAlgorithmStep()` 方法参数
    - `GraphModel.h:178` - `getAllEdgesSorted()` 返回类型
    - `GraphModel.cpp` - 所有相关使用（约10+处）
      - 局部变量：`edgesToRemove`、`path`、`pathEdges`、`topologicalOrder`
      - `addAlgorithmStep()` 调用：所有初始化列表语法改为显式构造
      - `std::reverse()` 和 `std::sort()` 替换为手动实现
  - 实现方式：
    - `MyVectorPairQStringQString` - 存储 `MyPairQStringQString` 的动态数组
    - 手动实现反转和排序算法（冒泡排序）
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.5.15**: MyVectorEdgeInfo 实现与替换
  - 输出：`EdgeInfo.h`、`MyVectorEdgeInfo.h`、`MyVectorEdgeInfo.cpp`
  - 替换位置：
    - `GraphModel.cpp:833` - `allEdges` 局部变量（Kruskal算法）
  - 实现方式：使用动态数组存储 `EdgeInfo` 结构体
  - 完成时间：2025年12月23日

#### 需要替换的STL容器
- `std::vector<QString>` → `MyVectorQString`（部分已完成，见 Task 1.1）
- `std::vector<VertexItem*>` → `MyVectorVertexItemPtr`（✅ 已完成，见 Subtask 1.5.1）
- `std::vector<EdgeItem*>` → `MyVectorEdgeItemPtr`（✅ 已完成，见 Subtask 1.5.2）
- `std::map<QString, int>` → `MyMapQStringToInt`（✅ 已完成，见 Subtask 1.5.3）
- `std::map<QString, QString>` → `MyMapQStringToQString`（✅ 已完成，见 Subtask 1.5.4）
- `std::map<QString, QPointF>` → `MyMapQStringToQPointF`（✅ 已完成，见 Subtask 1.5.5）
- `std::map<QString, VertexItem*>` → `MyMapQStringToVertexItemPtr`（✅ 已完成，见 Subtask 1.5.6）
- `std::map<std::pair<QString, QString>, EdgeItem*>` → `MyMapPairToEdgeItemPtr`（✅ 已完成，见 Subtask 1.5.7）
- `std::map<QString, std::set<QString>>` → `MyMapQStringToSetQString`（✅ 已完成，见 Subtask 1.5.10）
- `std::set<QString>` → `MySetQString`（✅ 已完成，见 Subtask 1.5.8）
- `std::set<std::pair<QString, QString>>` → `MySetPairQStringQString`（✅ 已完成，见 Subtask 1.5.9）
- `std::queue<QString>` → `MyQueueQString`（✅ 已完成，见 Subtask 1.5.11）
- `std::queue<AlgorithmStep>` → `MyQueueAlgorithmStep`（✅ 已完成，见 Subtask 1.5.12）
- `std::stack<QString>` → `MyStackQString`（✅ 已完成，见 Subtask 1.5.13）

#### 输入条件（Prerequisites）
- ✅ MyVectorQString 已完成（部分替换已完成）
- ⏳ 需要先实现其他自定义容器类

#### 输出结果（Deliverables）
- GraphModel.cpp/h 中所有STL容器替换完成
- 编译通过，功能正常

---

### Task 1.6: SortModel 中其他STL容器替换
**状态**：✅ 已完成  
**负责人**：AI  
**优先级**：中

#### 子任务（Subtasks）
- ✅ **Subtask 1.6.1**: MyVectorBarItemPtr 实现与替换
  - 输出：`MyVectorBarItemPtr.h`、`MyVectorBarItemPtr.cpp`
  - 替换位置：
    - `SortModel.h:106` - `m_barItems` 成员变量
    - `SortModel.h:59` - `getBarItems()` 方法返回类型
    - `SortModel.cpp` - 所有 `m_barItems` 的使用（约10+处）
      - `createBarItems()` - `push_back()` 操作
      - `clearBarItems()` - 基于范围的 for 循环遍历和 `clear()` 操作
      - `updateBarItems()` - `size()` 操作和索引访问
      - `processNextStep()` - `size()` 操作和索引访问
      - `animateSwap()` - `size()` 操作和索引访问，`std::swap()` 替换为手动交换
      - `animateMove()` - `size()` 操作和索引访问
  - 所有权语义：不拥有指针所有权，只存储指针值
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.6.2**: MyQueueSortStep 实现与替换
  - 输出：`MyQueueSortStep.h`、`MyQueueSortStep.cpp`
  - 替换位置：
    - `SortModel.h:118` - `m_algorithmSteps` 成员变量
    - `SortModel.cpp` - 所有 `m_algorithmSteps` 的使用（约5+处）
      - `executeAlgorithm()` - `empty()` 检查
      - `pauseAlgorithm()` - `empty()` 检查
      - `processNextStep()` - `empty()`, `front()`, `pop()` 操作
      - `generate*SortSteps()` - `push()` 操作（多个算法函数）
  - 实现方式：使用循环数组（circular buffer），维护 front 和 back 索引
  - 完成时间：2025年12月23日

- ✅ **Subtask 1.6.3**: MyVectorBool 实现与替换
  - 输出：`MyVectorBool.h`、`MyVectorBool.cpp`
  - 替换位置：
    - `SortModel.h:122` - `m_visited` 成员变量
    - `SortModel.cpp:608` - `m_visited.assign()` 调用
  - 实现方式：使用动态数组存储 `bool` 值，实现 `assign()` 方法
  - 完成时间：2025年12月23日

#### 需要替换的STL容器
- `std::vector<BarItem*>` → `MyVectorBarItemPtr`（✅ 已完成，见 Subtask 1.6.1）
- `std::queue<SortStep>` → `MyQueueSortStep`（✅ 已完成，见 Subtask 1.6.2）
- `std::vector<bool>` → `MyVectorBool`（✅ 已完成，见 Subtask 1.6.3）

#### 输入条件（Prerequisites）
- ✅ MyVectorInt 已完成
- ✅ MyVectorBarItemPtr 已完成（Subtask 1.6.1）
- ⏳ 需要先实现其他自定义容器类

#### 输出结果（Deliverables）
- SortModel.cpp/h 中所有STL容器替换完成
- 编译通过，功能正常

---

## ⚠️ 问题与风险（Issues & Risks）

### Issue 1: VectorException 重复定义
**状态**：✅ 已解决  
**发现时间**：2025年12月  
**解决方案**：创建单独的 `VectorException.h` 文件  
**参考**：`编译错误修复总结.md`

---

### Issue 2: 基于范围的for循环不兼容
**状态**：✅ 已解决  
**发现时间**：2025年12月  
**问题描述**：`MyVectorInt` 没有实现 `begin()`/`end()` 方法，无法使用基于范围的for循环  
**解决方案**：所有基于范围的for循环改为索引循环  
**影响范围**：`SortModel.cpp` 等多处  
**参考**：`编译错误修复总结.md`

---

### Issue 3: 大量STL容器需要替换
**状态**：⏳ 进行中  
**风险等级**：高  
**问题描述**：
- GraphModel 中仍有大量STL容器使用（std::map, std::set, std::queue, std::stack）
- SortModel 中仍有部分STL容器使用（std::vector<BarItem*>, std::queue<SortStep>）
- 需要实现多个自定义容器类

**缓解措施**：
- 分阶段实施，优先实现最常用的容器类
- 参考已完成的 MyVectorQString 和 MyVectorInt 的设计模式

---

### Issue 4: Qt Creator 编译警告
**状态**：✅ 已解决  
**发现时间**：2025年12月23日  
**问题描述**：
- `VisualItem.h:88`：缺少 `Q_INTERFACES(QGraphicsItem)` 宏，导致 `qobject_cast` 无法正常工作
- `VisualItem.cpp:397-398`：变量 `arrowDir` 和 `perpExtra` 已赋值但未使用
- `GraphModel.cpp:330-331`：变量 `k` 和 `c` 已定义但未使用

**解决方案**：
- 在 `VisualItem.h` 中添加 `Q_INTERFACES(QGraphicsItem)` 宏
- 删除 `VisualItem.cpp` 中未使用的变量 `arrowDir` 和 `perpExtra`
- 删除 `GraphModel.cpp` 中未使用的变量 `k` 和 `c`

**影响范围**：
- `VisualItem.h`、`VisualItem.cpp`、`GraphModel.cpp`

**修复时间**：2025年12月23日

---

## 📊 进度统计

### 已完成
- ✅ MyVectorQString 实现与替换（Task 1.1）
- ✅ MyVectorInt 实现与替换（Task 1.2）
- ✅ 编译错误修复（Task 1.3）
- ✅ 代码整理（Task 1.4）
- ✅ MyVectorVertexItemPtr 实现与替换（Task 1.5 Subtask 1.5.1）
- ✅ MyVectorEdgeItemPtr 实现与替换（Task 1.5 Subtask 1.5.2）
- ✅ MyMapQStringToInt 实现与替换（Task 1.5 Subtask 1.5.3）
- ✅ MyMapQStringToQString 实现与替换（Task 1.5 Subtask 1.5.4）
- ✅ MyMapQStringToQPointF 实现与替换（Task 1.5 Subtask 1.5.5）
- ✅ MyVectorBarItemPtr 实现与替换（Task 1.6 Subtask 1.6.1）
- ✅ MyQueueSortStep 实现与替换（Task 1.6 Subtask 1.6.2）
- ✅ MyVectorBool 实现与替换（Task 1.6 Subtask 1.6.3）

### 进行中
- （无）

### 未开始
- ⏳ 其他自定义容器类实现（MyMap, MySet, MyQueue, MyStack 系列）

---

## 🔧 技术决策与约束

### 决策 1: 不使用模板
**原因**：课程要求"不能使用类库、模板等（用户界面除外）"  
**影响**：需要为每种类型单独实现容器类  
**示例**：`MyVectorInt`、`MyVectorQString`、`MyVectorVertexItemPtr` 等

---

### 决策 2: 手动内存管理
**原因**：不使用STL，需要手动管理内存  
**实现方式**：使用 `new[]` 和 `delete[]`  
**要求**：必须提供完整的析构函数、拷贝构造函数、赋值运算符

---

### 决策 3: 统一异常处理
**原因**：不使用 `std::exception`  
**实现方式**：自定义 `VectorException` 类  
**位置**：`VectorException.h`

---

### 决策 4: 索引循环替代迭代器
**原因**：不使用模板，无法实现通用迭代器  
**实现方式**：所有循环使用索引访问（`for (size_t i = 0; i < vec.size(); ++i)`）  
**影响**：代码可读性略降，但符合约束要求

---

## 📚 参考文档

### 项目分析文档
- `不满足编码要求的地方.md` - STL和模板使用情况分析
- `项目使用的类库清单.md` - 类库使用清单
- `代码使用情况分析.md` - 代码使用情况分析
- `字符串类使用情况分析.md` - 字符串类使用情况

### 实现总结文档
- `getNeighbors替换总结.md` - MyVectorQString 替换总结
- `MyVectorInt替换总结.md` - MyVectorInt 替换总结
- `MyVectorInt功能确认.md` - MyVectorInt 功能确认
- `MyVectorQString功能确认.md` - MyVectorQString 功能确认
- `编译错误修复总结.md` - 编译错误修复总结
- `代码整理总结.md` - 代码整理总结

---

## 🔄 文档更新记录

| 日期 | 更新内容 | 更新人 |
|------|---------|--------|
| 2025-12-23 | 初始文档创建，记录已完成任务 | AI |
| 2025-12-23 | 添加编译错误修复任务 | AI |
| 2025-12-23 | 添加未完成任务清单 | AI |
| 2025-12-23 | 完成 MyVectorVertexItemPtr 实现与替换（Task 1.5 Subtask 1.5.1） | AI |
| 2025-12-23 | 修复 Qt Creator 编译警告（Issue 4）：Q_INTERFACES 宏和未使用变量 | AI |
| 2025-12-23 | 完成 MyVectorEdgeItemPtr 实现与替换（Task 1.5 Subtask 1.5.2） | AI |
| 2025-12-23 | 完成 MyMapQStringToInt 实现与替换（Task 1.5 Subtask 1.5.3） | AI |
| 2025-12-23 | 完成 MyMapQStringToQString 实现与替换（Task 1.5 Subtask 1.5.4） | AI |
| 2025-12-23 | 完成 MyMapQStringToQPointF 实现与替换（Task 1.5 Subtask 1.5.5） | AI |
| 2025-12-23 | 完成 MyMapQStringToVertexItemPtr 实现与替换（Task 1.5 Subtask 1.5.6） | AI |
| 2025-12-23 | 完成 MyMapPairToEdgeItemPtr 实现与替换（Task 1.5 Subtask 1.5.7） | AI |
| 2025-12-23 | 完成 MySetQString 实现与替换（Task 1.5 Subtask 1.5.8） | AI |
| 2025-12-23 | 完成 MySetPairQStringQString 实现与替换（Task 1.5 Subtask 1.5.9） | AI |
| 2025-12-23 | 完成 MyMapQStringToSetQString 实现与替换（Task 1.5 Subtask 1.5.10） | AI |
| 2025-12-23 | 完成 MyQueueQString 实现与替换（Task 1.5 Subtask 1.5.11） | AI |
| 2025-12-23 | 完成 MyQueueAlgorithmStep 实现与替换（Task 1.5 Subtask 1.5.12） | AI |
| 2025-12-23 | 完成 MyStackQString 实现与替换（Task 1.5 Subtask 1.5.13） | AI |
| 2025-12-23 | 完成 MyQueueQString 实现与替换（Task 1.5 Subtask 1.5.11） | AI |
| 2025-12-23 | 完成 AlgorithmStep 结构体和 GraphModel 中剩余 std::vector 替换（Task 1.5 Subtask 1.5.14） | AI |
| 2025-12-23 | 完成 MyVectorBarItemPtr 实现与替换（Task 1.6 Subtask 1.6.1） | AI |
| 2025-12-23 | 完成 MyQueueSortStep 实现与替换（Task 1.6 Subtask 1.6.2） | AI |
| 2025-12-23 | 完成 MyVectorBool 实现与替换（Task 1.6 Subtask 1.6.3） | AI |
| 2025-12-23 | 完成 MyVectorEdgeInfo 实现与替换（Task 1.5 Subtask 1.5.15） | AI |
| 2025-12-23 | 清理未使用的STL头文件（GraphModel.h, GraphModel.cpp, SortModel.cpp） | AI |
| 2025-12-23 | 清理未使用的STL头文件（GraphModel.h, GraphModel.cpp, SortModel.cpp） | AI |

---

*文档最后更新时间：2025年12月23日*  
*项目名称：StructureVisual（数据结构可视化模拟器）*

这是我暂存的提示词：你不需要读，但是可以读
你采用“增量记忆模式”与我协作。

规则如下：
1. 在本会话开始时，你需要【完整阅读一次】项目文档，并建立初始工作记忆。
2. 在后续轮次中：
   - 你不需要重新通读全文
   - 只需基于“上一轮状态 + 本轮新增信息”进行推理
3. 文档的维护方式是【增量更新】，而不是重写或重述。
4. 只有在我明确指出以下关键词时，你才需要回顾全文：
   - “回顾文档”
   - “整体状态”
   - “重新梳理”
   - “你是不是忘了什么”
5. 若你判断当前任务与文档已有结论发生冲突，需明确指出并请求确认，而不是自动回滚。

# 项目协作状态文档

## 1. 项目不变量（低频修改）
- 项目目标
- 技术约束
- 已确认的设计原则
- 已废弃的方案（明确不可再用）

## 2. 当前焦点（高频修改，核心）
- 当前正在处理的任务（最多 1–3 个）
- 当前阻塞点
- 本轮目标

## 3. 任务进度快照（中频修改）
- 已完成任务（简要）
- 进行中任务
- 未开始任务

## 4. 关键结论 / 决策记录（追加式）
- [时间] 决策 / 结论
- 原因（一句话）
