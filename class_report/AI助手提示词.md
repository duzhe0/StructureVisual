# 自定义容器类实现与替换任务提示词模板

> **用途**：用于指导AI助手实现和替换STL容器的标准流程  
> **适用场景**：实现 `MyVector*`、`MyMap*`、`MySet*`、`MyQueue*`、`MyStack*` 等自定义容器类并替换项目中的STL容器

---

## 📋 任务模板

### 任务描述

请实现 `{自定义容器类名}` 类，用于替代项目中的 `{STL容器类型}`，并完成代码替换。

**替换目标**：
- `{STL容器类型}` → `{自定义容器类名}`

**替换位置**：
- `{文件路径1}`：{使用位置描述}
- `{文件路径2}`：{使用位置描述}
- ...（列出所有需要替换的文件和位置）

---

模板基于已有的 MyVectorInt 和 MyVectorQString 实现模式，可直接用于：

### MyVector* 系列（✅ 已完成）
- ✅ MyVectorInt - `std::vector<int>` → `MyVectorInt`
- ✅ MyVectorQString - `std::vector<QString>` → `MyVectorQString`
- ✅ MyVectorVertexItemPtr - `std::vector<VertexItem*>` → `MyVectorVertexItemPtr`
- ✅ MyVectorEdgeItemPtr - `std::vector<EdgeItem*>` → `MyVectorEdgeItemPtr`
- ✅ MyVectorBarItemPtr - `std::vector<BarItem*>` → `MyVectorBarItemPtr`（Task 1.6）
- ✅ MyVectorPairQStringQString - `std::vector<std::pair<QString, QString>>` → `MyVectorPairQStringQString`（Task 1.5.14）
- ✅ MyVectorBool - `std::vector<bool>` → `MyVectorBool`（Task 1.6）

### MyMap* 系列（✅ 已完成）

#### 1. MyMapQStringToInt ✅
- **STL类型**：`std::map<QString, int>`
- **自定义类名**：`MyMapQStringToInt`
- **关键词替换**：
  - `{KeyType}` → `QString`
  - `{ValueType}` → `int`
  - `{KeyTypeName}` → `QString`
  - `{ValueTypeName}` → `Int`
- **使用位置**：
  - `GraphModel.cpp:557` - `distances` 变量（Dijkstra算法）
- **特殊说明**：值类型是基本类型 `int`，直接存储值

#### 2. MyMapQStringToQString ✅
- **STL类型**：`std::map<QString, QString>`
- **自定义类名**：`MyMapQStringToQString`
- **关键词替换**：
  - `{KeyType}` → `QString`
  - `{ValueType}` → `QString`
  - `{KeyTypeName}` → `QString`
  - `{ValueTypeName}` → `QString`
- **使用位置**：
  - `GraphModel.cpp:559` - `predecessors` 变量（Dijkstra算法）
  - `GraphModel.cpp:788` - `parent` 变量（Prim算法）
- **特殊说明**：键和值都是 `QString`，需要实现 `QString` 的拷贝语义

#### 3. MyMapQStringToQPointF ✅
- **STL类型**：`std::map<QString, QPointF>`
- **自定义类名**：`MyMapQStringToQPointF`
- **关键词替换**：
  - `{KeyType}` → `QString`
  - `{ValueType}` → `QPointF`
  - `{KeyTypeName}` → `QString`
  - `{ValueTypeName}` → `QPointF`
- **使用位置**：
  - `GraphModel.cpp:960` - `forces` 变量（力导向布局算法）
- **特殊说明**：值类型是 Qt 的 `QPointF`，需要包含 `<QPointF>`

#### 4. MyMapQStringToVertexItemPtr ✅
- **STL类型**：`std::map<QString, VertexItem*>`
- **自定义类名**：`MyMapQStringToVertexItemPtr`
- **关键词替换**：
  - `{KeyType}` → `QString`
  - `{ValueType}` → `VertexItem*`
  - `{KeyTypeName}` → `QString`
  - `{ValueTypeName}` → `VertexItemPtr`
- **使用位置**：
  - `GraphModel.h:125` - `m_vertices` 成员变量
- **特殊说明**：值类型是指针，不拥有指针所有权，只存储指针值

#### 5. MyMapPairToEdgeItemPtr ✅
- **STL类型**：`std::map<std::pair<QString, QString>, EdgeItem*>`
- **自定义类名**：`MyMapPairToEdgeItemPtr`
- **关键词替换**：
  - `{KeyType}` → `std::pair<QString, QString>`（需要先实现 `MyPairQStringQString`）
  - `{ValueType}` → `EdgeItem*`
  - `{KeyTypeName}` → `PairQStringQString`
  - `{ValueTypeName}` → `EdgeItemPtr`
- **使用位置**：
  - `GraphModel.h:126` - `m_edges` 成员变量
- **特殊说明**：
  - 键类型是 `std::pair`，需要先实现 `MyPairQStringQString` 类
  - 值类型是指针，不拥有指针所有权

#### 6. MyMapQStringToSetQString ✅（复杂，需先实现 MySetQString）
- **STL类型**：`std::map<QString, std::set<QString>>`
- **自定义类名**：`MyMapQStringToSetQString`
- **关键词替换**：
  - `{KeyType}` → `QString`
  - `{ValueType}` → `MySetQString`（需要先实现）
  - `{KeyTypeName}` → `QString`
  - `{ValueTypeName}` → `SetQString`
- **使用位置**：
  - `GraphModel.h:127` - `m_adjacencyList` 成员变量
- **特殊说明**：
  - 值类型是 `MySetQString`，需要先实现 `MySetQString` 类
  - 这是嵌套容器，实现复杂度较高

### MySet* 系列（✅ 已完成）

#### 1. MySetQString ✅
- **STL类型**：`std::set<QString>`
- **自定义类名**：`MySetQString`
- **关键词替换**：
  - `{ElementType}` → `QString`
  - `{ElementTypeName}` → `QString`
- **使用位置**：
  - `GraphModel.h:144` - `m_visitedVertices` 成员变量（算法状态跟踪）
  - `GraphModel.h:129` - `m_adjacencyList` 的值类型（邻接表）
  - `GraphModel.cpp:53` - `m_adjacencyList[label] = std::set<QString>()`（初始化）
  - `GraphModel.cpp:121, 124, 148, 153` - `m_adjacencyList[from].insert(to)` / `erase(to)`（邻接表操作）
  - `GraphModel.cpp:565` - `unvisited` 变量（Dijkstra算法）
  - `GraphModel.cpp:707, 823` - `mstEdges` 变量（最小生成树算法）
  - `GraphModel.cpp:1116` - `processedEdges` 变量（文件保存）
- **特殊说明**：
  - 元素类型是 `QString`，需要实现 `QString` 的比较和拷贝语义
  - Set 需要维护元素唯一性，插入时检查是否已存在
  - 可以使用数组 + 线性查找实现（简单实现，适合小规模数据）
  - 或者使用排序数组 + 二分查找实现（性能更好，但需要维护排序）

#### 2. MySetPairQStringQString ✅
- **STL类型**：`std::set<std::pair<QString, QString>>`
- **自定义类名**：`MySetPairQStringQString`
- **关键词替换**：
  - `{ElementType}` → `MyPairQStringQString`（需要先实现 `MyPairQStringQString`）
  - `{ElementTypeName}` → `PairQStringQString`
- **使用位置**：
  - `GraphModel.h:145` - `m_visitedEdges` 成员变量（算法状态跟踪）
- **特殊说明**：
  - 元素类型是 `MyPairQStringQString`，需要先实现 `MyPairQStringQString` 类
  - Set 需要维护元素唯一性，需要 `MyPairQStringQString` 实现比较运算符
  - 可以使用数组 + 线性查找实现

### MyQueue* 系列（✅ 已完成）
- ✅ `std::queue<QString>` → `MyQueueQString`
- ✅ `std::queue<AlgorithmStep>` → `MyQueueAlgorithmStep`
- ✅ `std::queue<SortStep>` → `MyQueueSortStep`（Task 1.6）

### MyStack* 系列（✅ 已完成）
- ✅ `std::stack<QString>` → `MyStackQString`

### MyPair* 系列（✅ 已完成）
- ✅ `std::pair<QString, QString>` → `MyPairQStringQString`（已完成，见 MyMapPairToEdgeItemPtr 实现）

---

## 📝 MyMap* 系列实现关键词替换模板

使用以下模板时，将 `{关键词}` 替换为对应的值：

### 类名和类型定义
```cpp
// 类名
class MyMap{KeyTypeName}To{ValueTypeName} {
    // 例如：MyMapQStringToInt → class MyMapQStringToInt

// 键类型
{KeyType} m_keys[...];  // 例如：QString m_keys[...];

// 值类型
{ValueType} m_values[...];  // 例如：int m_values[...];
```

### 方法签名
```cpp
// 插入方法
void insert(const {KeyType}& key, const {ValueType}& value);
void insert(const {KeyType}& key, {ValueType} value);  // 如果是指针类型

// 查找方法
{ValueType}* find(const {KeyType}& key);
const {ValueType}* find(const {KeyType}& key) const;

// 访问方法
{ValueType}& operator[](const {KeyType}& key);
{ValueType}& at(const {KeyType}& key);
```

### 特殊处理
- **指针类型**（如 `VertexItem*`）：
  - 值类型使用 `{ValueType}*`（如 `VertexItem**` 存储指针数组）
  - 不拥有指针所有权，析构函数不 `delete` 指针
  - 拷贝语义：浅拷贝（只拷贝指针值）

- **基本类型**（如 `int`）：
  - 值类型直接存储值
  - 拷贝语义：深拷贝（拷贝值）

- **Qt类型**（如 `QString`, `QPointF`）：
  - 需要包含相应的 Qt 头文件
  - 使用 Qt 的拷贝语义

- **Pair类型**（如 `std::pair<QString, QString>`）：
  - 需要先实现 `MyPairQStringQString` 类
  - 键类型使用自定义 Pair 类

- **嵌套容器**（如 `std::map<QString, std::set<QString>>`）：
  - 需要先实现值类型的容器类（如 `MySetQString`）
  - 值类型使用自定义容器类

---

## 📝 MySet* 系列实现关键词替换模板

使用以下模板时，将 `{关键词}` 替换为对应的值：

### 类名和类型定义
```cpp
// 类名
class MySet{ElementTypeName} {
    // 例如：MySetQString → class MySetQString

// 元素类型数组
{ElementType} m_data[...];  // 例如：QString m_data[...];
size_t m_size;              // 当前元素数量
size_t m_capacity;          // 当前容量
```

### 方法签名
```cpp
// 插入方法
bool insert(const {ElementType}& value);  // 返回true如果插入成功，false如果已存在

// 删除方法
bool erase(const {ElementType}& value);   // 返回true如果删除成功，false如果不存在

// 查找方法
bool find(const {ElementType}& value) const;  // 返回true如果找到，false否则
bool contains(const {ElementType}& value) const;  // 同find，语义更清晰

// 容量相关
size_t size() const;
bool empty() const;
void clear();

// 遍历方法（用于替代基于范围的for循环）
{ElementType}& at(size_t index);  // 获取指定索引的元素
const {ElementType}& at(size_t index) const;
```

### 特殊处理
- **QString类型**（如 `MySetQString`）：
  - 元素类型使用 `QString`
  - 需要包含 `<QString>` 头文件
  - 使用 `QString` 的比较运算符（`<`, `==`）
  - 拷贝语义：深拷贝（拷贝值）

- **Pair类型**（如 `MySetPairQStringQString`）：
  - 元素类型使用 `MyPairQStringQString`
  - 需要先实现 `MyPairQStringQString` 类
  - 需要 `MyPairQStringQString` 实现比较运算符（`<`, `==`）
  - 拷贝语义：深拷贝（拷贝值）

### 实现策略
- **简单实现（线性查找）**：
  - 使用动态数组存储元素
  - 插入时线性查找是否已存在
  - 删除时线性查找并移动元素
  - 时间复杂度：O(n) 查找，O(n) 插入/删除
  - 适合小规模数据（< 100个元素）

- **优化实现（排序数组 + 二分查找）**：
  - 使用排序的动态数组存储元素
  - 插入时二分查找位置，保持排序
  - 删除时二分查找并移动元素
  - 时间复杂度：O(log n) 查找，O(n) 插入/删除（需要移动元素）
  - 适合中等规模数据（100-1000个元素）

### 注意事项
- Set 需要维护元素唯一性，插入前必须检查是否已存在
- 如果使用排序数组实现，需要维护数组的排序状态
- 遍历时可以使用索引循环：`for (size_t i = 0; i < set.size(); ++i) { set.at(i); }`
- 不支持基于范围的 for 循环，需要使用索引循环替代

---

## 🎯 实现优先级建议

1. **优先级1（基础类型）**：
   - `MyMapQStringToInt` - 最简单，值类型是基本类型
   - `MyMapQStringToQString` - 键值都是 QString

2. **优先级2（Qt类型）**：
   - `MyMapQStringToQPointF` - 值类型是 Qt 类型

3. **优先级3（指针类型）**：
   - `MyMapQStringToVertexItemPtr` - 成员变量，使用频繁
   - `MyMapPairToEdgeItemPtr` - 需要先实现 `MyPairQStringQString`

4. **优先级4（Set类型）**：
   - `MySetQString` - 基础Set类型，用于算法状态跟踪和邻接表
   - `MySetPairQStringQString` - 需要先实现 `MyPairQStringQString`

5. **优先级5（嵌套容器）**：
   - `MyMapQStringToSetQString` - 需要先实现 `MySetQString`

6. **优先级6（队列和栈）**：
   - `MyQueueQString` - BFS算法和拓扑排序需要
   - `MyQueueAlgorithmStep` - 算法步骤队列
   - `MyStackQString` - DFS算法需要

基于此模板开始实现MyStackQString

## 🎯 实现步骤

### Step 1: 分析现有使用情况

**任务**：分析项目中 `{STL容器类型}` 的使用情况

**操作**：
1. 使用 `grep` 或 `codebase_search` 查找所有 `{STL容器类型}` 的使用位置
2. 分析每个使用位置的上下文：
   - 是成员变量、局部变量还是方法参数？
   - 使用了哪些STL方法（push_back, insert, find, erase等）？
   - 是否有基于范围的for循环？
   - 是否有初始化列表语法？
   - 是否有STL算法（std::swap, std::find等）？

**输出**：
- 列出所有使用位置清单
- 分析需要实现的方法清单

---

### Step 2: 设计自定义容器类接口

**任务**：设计 `{自定义容器类名}.h` 头文件

**参考实现**：
- `MyVectorInt.h` - 基础Vector实现参考
- `MyVectorQString.h` - QString类型Vector实现参考

**设计原则**：
1. **不使用模板**：为特定类型专门实现
2. **手动内存管理**：使用 `new[]`/`delete[]`（对于指针类型，注意指针的所有权）
3. **统一异常处理**：使用 `VectorException`（或相应的异常类）
4. **完整拷贝语义**：实现拷贝构造函数、赋值运算符
5. **边界检查**：提供 `at()` 方法进行边界检查，`operator[]` 不检查（可选）

**必须实现的方法**（根据使用情况选择）：
- **构造函数**：默认构造、拷贝构造
- **析构函数**：释放内存
- **赋值运算符**：`operator=`
- **元素访问**：`at()`, `operator[]`, `front()`, `back()`, `data()`
- **容量相关**：`size()`, `capacity()`, `empty()`, `reserve()`, `resize()`
- **修改操作**：`push_back()`, `pop_back()`, `clear()`, `insert()`, `erase()`
- **辅助方法**：`swap()`

**特殊考虑**（针对指针类型）：
- 指针类型容器需要明确所有权语义：
  - 如果容器拥有指针所有权：析构函数需要 `delete` 每个指针
  - 如果容器不拥有所有权：析构函数只释放数组，不 `delete` 指针
- 拷贝语义：
  - 深拷贝：拷贝指针指向的对象
  - 浅拷贝：只拷贝指针值（通常用于不拥有所有权的情况）

**输出**：
- `{自定义容器类名}.h` 头文件

---

### Step 3: 实现自定义容器类

**任务**：实现 `{自定义容器类名}.cpp` 源文件

**实现要点**：
1. **内存管理**：
   - 默认构造函数：分配初始容量（通常16）
   - 拷贝构造函数：深拷贝所有元素
   - 析构函数：释放内存（注意指针类型的所有权）
   - 赋值运算符：使用拷贝-交换惯用法

2. **扩容机制**：
   - 当 `size >= capacity` 时，容量翻倍（`newCapacity = capacity * 2`）
   - 使用 `reallocate()` 辅助方法

3. **异常安全**：
   - 所有可能越界的操作使用 `checkIndex()`
   - 内存分配失败时抛出异常

4. **边界检查**：
   - `at()` 方法：调用 `checkIndex()` 后返回引用
   - `operator[]`：不检查边界（可选，根据需求）

**参考实现**：
- `MyVectorInt.cpp` - 基础实现参考
- `MyVectorQString.cpp` - QString类型实现参考

**输出**：
- `{自定义容器类名}.cpp` 源文件

---

### Step 4: 更新项目文件

**任务**：更新 `StructureVisual.pro` 文件

**操作**：
1. 在 `HEADERS` 部分添加：`{自定义容器类名}.h`
2. 在 `SOURCES` 部分添加：`{自定义容器类名}.cpp`

**输出**：
- 更新后的 `StructureVisual.pro` 文件

---

### Step 5: 替换代码中的STL容器

**任务**：将所有 `{STL容器类型}` 替换为 `{自定义容器类名}`

**替换规则**：

#### 5.1 头文件修改
- 添加 `#include "{自定义容器类名}.h"`
- 移除 `#include <vector>` 或其他STL头文件（如果不再需要）

#### 5.2 类型声明替换
```cpp
// 修改前：
{STL容器类型} variableName;

// 修改后：
{自定义容器类名} variableName;
```

#### 5.3 方法签名替换
```cpp
// 修改前：
void methodName(const {STL容器类型}& param);
{STL容器类型} returnMethod();

// 修改后：
void methodName(const {自定义容器类名}& param);
{自定义容器类名} returnMethod();
```

#### 5.4 初始化列表替换
```cpp
// 修改前（如果使用初始化列表）：
{STL容器类型} vec = {value1, value2, value3};

// 修改后：
{自定义容器类名} vec;
vec.push_back(value1);
vec.push_back(value2);
vec.push_back(value3);
```

#### 5.5 基于范围的for循环替换
```cpp
// 修改前：
for (const auto& item : container) {
    // ...
}

// 修改后：
for (size_t i = 0; i < container.size(); ++i) {
    const auto& item = container.at(i);
    // ...
}
```

#### 5.6 STL算法替换
```cpp
// std::swap 替换：
// 修改前：
std::swap(a, b);

// 修改后：
auto temp = a;
a = b;
b = temp;

// std::find 替换：
// 修改前：
auto it = std::find(vec.begin(), vec.end(), value);

// 修改后：
size_t index = SIZE_MAX;
for (size_t i = 0; i < vec.size(); ++i) {
    if (vec[i] == value) {
        index = i;
        break;
    }
}
```

#### 5.7 构造函数调用替换
```cpp
// 修改前：
{STL容器类型} vec(size);

// 修改后：
{自定义容器类名} vec;
vec.resize(size, defaultValue);

// 修改前：
{STL容器类型} vec(size, value);

// 修改后：
{自定义容器类名} vec;
vec.resize(size, value);
```

**输出**：
- 所有相关文件的修改完成

---

### Step 6: 编译检查

**任务**：检查编译错误

**操作**：
1. 运行编译命令
2. 修复所有编译错误：
   - 未声明的标识符
   - 类型不匹配
   - 缺少头文件包含
   - 方法未定义

**常见错误及修复**：
- **错误**：`begin()`/`end()` 未声明
  - **原因**：使用了基于范围的for循环
  - **修复**：改为索引循环

- **错误**：初始化列表不兼容
  - **原因**：使用了 `{value1, value2}` 语法
  - **修复**：改为显式创建对象并 `push_back()`

- **错误**：`std::swap` 未定义
  - **原因**：使用了STL算法
  - **修复**：改为手动交换

**输出**：
- 编译通过，无错误

---

### Step 7: 功能验证

**任务**：验证替换后的功能正常

**操作**：
1. 运行程序
2. 测试相关功能：
   - {列出需要测试的功能点}
3. 检查内存泄漏（如果可能）

**输出**：
- 功能验证通过

---

### Step 8: 更新项目文档

**任务**：更新 `class_report/使用AI助手的最佳实践.md`

**操作**：
1. 在相应任务下添加子任务完成记录
2. 更新进度统计
3. 记录替换总结（可选，创建单独的总结文档）

**输出**：
- 文档更新完成

---

## 📝 替换总结模板

创建 `class_report/{自定义容器类名}替换总结.md` 文档，包含：

```markdown
# {自定义容器类名} 替换总结

## ✅ 替换完成

已成功将 {文件列表} 中所有 `{STL容器类型}` 替换为 `{自定义容器类名}`。

---

## 📝 修改内容

### 1. {文件名1} 修改
- {修改点1}
- {修改点2}

### 2. {文件名2} 修改
- {修改点1}
- {修改点2}

---

## 🔄 主要变化

### 1. {变化类型1}
{描述变化}

### 2. {变化类型2}
{描述变化}

---

## 📊 替换统计

| 文件 | {STL容器类型} 替换数量 | 主要位置 |
|------|---------------------|---------|
| {文件1} | {数量} | {位置描述} |
| {文件2} | {数量} | {位置描述} |
| **总计** | **{总数量}** | - |

---

## ✅ 验证结果

- ✅ 编译检查：无linter错误
- ✅ 头文件包含：已添加 `{自定义容器类名}.h`
- ✅ 方法声明：已更新所有方法签名
- ✅ 方法实现：已更新所有实现
- ✅ 所有调用处：已全部替换
- ✅ 项目文件：已更新 `.pro` 文件

---

## 📝 注意事项

{记录特殊注意事项}

---

*文档生成时间：{日期}*
```

---

## 🎯 示例：MyVectorVertexItemPtr 实现任务

### 任务描述

请实现 `MyVectorVertexItemPtr` 类，用于替代项目中的 `std::vector<VertexItem*>`，并完成代码替换。

**替换目标**：
- `std::vector<VertexItem*>` → `MyVectorVertexItemPtr`

**替换位置**：
- `GraphModel.h`：`getAllVertices()` 方法返回类型
- `GraphModel.cpp`：`getAllVertices()` 方法实现

---

### 实现要点（针对指针类型）

**所有权语义**：
- `GraphModel` 拥有 `VertexItem*` 的所有权
- `MyVectorVertexItemPtr` **不拥有**指针所有权（只是存储指针）
- 析构函数：只释放数组内存，**不** `delete` 指针

**拷贝语义**：
- 浅拷贝：只拷贝指针值，不拷贝指向的对象
- 拷贝构造函数和赋值运算符：只复制指针数组

**参考实现**：
- 参考 `MyVectorInt.h` 和 `MyVectorInt.cpp` 的结构
- 将 `int` 替换为 `VertexItem*`
- 注意前向声明：在头文件中使用前向声明 `class VertexItem;`

---

## ⚠️ 注意事项

1. **前向声明**：如果容器存储的是指针类型，头文件中使用前向声明，避免循环依赖
2. **所有权明确**：指针类型容器必须明确所有权语义
3. **异常安全**：确保内存分配失败时正确抛出异常
4. **边界检查**：所有可能越界的操作都要检查
5. **代码风格**：保持与现有代码风格一致

---

## 📚 参考文档

- `MyVectorInt.h` / `MyVectorInt.cpp` - Vector基础实现
- `MyVectorQString.h` / `MyVectorQString.cpp` - QString类型Vector实现
- `class_report/MyVectorInt替换总结.md` - 替换总结示例
- `class_report/getNeighbors替换总结.md` - 替换总结示例
- `class_report/使用AI助手的最佳实践.md` - 项目状态文档

---

*模板创建时间：2025年12月23日*  
*最后更新时间：2025年12月23日*

---

## 🎯 Task 1.6: SortModel 中STL容器替换任务提示词

### 任务描述

请实现以下自定义容器类，用于替代 SortModel 中的 STL 容器，并完成代码替换。

**替换目标**：
- `std::vector<BarItem*>` → `MyVectorBarItemPtr`
- `std::queue<SortStep>` → `MyQueueSortStep`
- `std::vector<bool>` → `MyVectorBool`（可选，优先级较低）

**替换位置**：
- `SortModel.h:106` - `m_barItems` 成员变量（`std::vector<BarItem*>`）
- `SortModel.h:59` - `getBarItems()` 方法返回类型（`std::vector<BarItem*>`）
- `SortModel.h:118` - `m_algorithmSteps` 成员变量（`std::queue<SortStep>`）
- `SortModel.h:122` - `m_visited` 成员变量（`std::vector<bool>`，可选）
- `SortModel.cpp` - 所有相关使用位置（约15+处）

---

### Subtask 1: MyVectorBarItemPtr 实现与替换

#### 替换目标
- `std::vector<BarItem*>` → `MyVectorBarItemPtr`

#### 替换位置
- `SortModel.h:106` - `m_barItems` 成员变量
- `SortModel.h:59` - `getBarItems()` 方法返回类型
- `SortModel.cpp` - 所有 `m_barItems` 的使用（约10+处）：
  - `createBarItems()` - `push_back()` 操作
  - `clearBarItems()` - 基于范围的 for 循环遍历和 `clear()` 操作
  - `updateBarItems()` - `size()` 操作和索引访问
  - `processNextStep()` - `size()` 操作和索引访问
  - `animateSwap()` - `size()` 操作和索引访问
  - `animateMove()` - `size()` 操作和索引访问

#### 使用的STL方法
- `push_back(BarItem*)` - 添加元素
- `clear()` - 清空容器
- `size()` - 获取元素数量
- `operator[]` 或 `at()` - 索引访问
- 基于范围的 for 循环：`for (BarItem *bar : m_barItems)`

#### 实现要点

**所有权语义**：
- `SortModel` 拥有 `BarItem*` 的所有权（在 `clearBarItems()` 中会 `delete` 每个指针）
- `MyVectorBarItemPtr` **不拥有**指针所有权（只是存储指针）
- 析构函数：只释放数组内存，**不** `delete` 指针

**拷贝语义**：
- 浅拷贝：只拷贝指针值，不拷贝指向的对象
- 拷贝构造函数和赋值运算符：只复制指针数组

**参考实现**：
- 参考 `MyVectorVertexItemPtr.h` 和 `MyVectorVertexItemPtr.cpp` 的结构
- 将 `VertexItem*` 替换为 `BarItem*`
- 注意前向声明：在头文件中使用前向声明 `class BarItem;`

**需要实现的方法**：
- 构造函数：默认构造、拷贝构造
- 析构函数：释放内存（不 delete 指针）
- 赋值运算符：`operator=`
- 元素访问：`at()`, `operator[]`
- 容量相关：`size()`, `empty()`, `clear()`
- 修改操作：`push_back()`
- 辅助方法：`swap()`

**特殊处理**：
- 基于范围的 for 循环需要改为索引循环：
  ```cpp
  // 修改前：
  for (BarItem *bar : m_barItems) {
      // ...
  }
  
  // 修改后：
  for (size_t i = 0; i < m_barItems.size(); ++i) {
      BarItem *bar = m_barItems[i];
      // ...
  }
  ```

---

### Subtask 2: MyQueueSortStep 实现与替换

#### 替换目标
- `std::queue<SortStep>` → `MyQueueSortStep`

#### 替换位置
- `SortModel.h:118` - `m_algorithmSteps` 成员变量
- `SortModel.cpp` - 所有 `m_algorithmSteps` 的使用（约5+处）：
  - `executeAlgorithm()` - `empty()` 检查
  - `pauseAlgorithm()` - `empty()` 检查
  - `processNextStep()` - `empty()`, `front()`, `pop()` 操作
  - `generate*SortSteps()` - `push()` 操作（多个算法函数）

#### 使用的STL方法
- `empty()` - 检查队列是否为空
- `front()` - 获取队首元素
- `pop()` - 移除队首元素
- `push(SortStep)` - 添加元素到队尾

#### 实现要点

**实现方式**：
- 使用循环数组（circular buffer）实现，维护 `front` 和 `back` 索引
- 自动扩容机制（容量不足时2倍扩容）
- 提供完整的拷贝语义和异常安全

**参考实现**：
- 参考 `MyQueueAlgorithmStep.h` 和 `MyQueueAlgorithmStep.cpp` 的结构
- 将 `AlgorithmStep` 替换为 `SortStep`
- 注意前向声明：在头文件中使用前向声明 `struct SortStep;`
- 在源文件中包含 `SortModel.h` 以获取 `SortStep` 的完整定义

**需要实现的方法**：
- 构造函数：默认构造、拷贝构造
- 析构函数：释放内存
- 赋值运算符：`operator=`
- 队列操作：`push()`, `pop()`, `front()`, `back()`
- 容量相关：`size()`, `empty()`, `clear()`, `reserve()`
- 辅助方法：`swap()`

**特殊处理**：
- `SortStep` 结构体定义在 `SortModel.h` 中，需要前向声明
- 拷贝构造函数和赋值运算符需要深拷贝 `SortStep` 对象（包括内部的 `MyVectorInt` 成员）

---

### Subtask 3: MyVectorBool 实现与替换（可选）

#### 替换目标
- `std::vector<bool>` → `MyVectorBool`

#### 替换位置
- `SortModel.h:122` - `m_visited` 成员变量
- `SortModel.cpp:604` - `m_visited.assign(m_data.size(), false)` 调用

#### 使用的STL方法
- `assign(size, value)` - 分配并初始化元素
- `size()` - 获取元素数量（可能通过索引访问）

#### 实现要点

**实现方式**：
- 使用动态数组存储 `bool` 值
- 实现 `assign(size_t count, bool value)` 方法
- 提供 `at()` 和 `operator[]` 访问方法

**参考实现**：
- 参考 `MyVectorInt.h` 和 `MyVectorInt.cpp` 的结构
- 将 `int` 替换为 `bool`
- 注意 `bool` 是基本类型，直接存储值

**需要实现的方法**：
- 构造函数：默认构造、拷贝构造、带参数的构造 `MyVectorBool(size_t count, bool value)`
- 析构函数：释放内存
- 赋值运算符：`operator=`
- 元素访问：`at()`, `operator[]`
- 容量相关：`size()`, `empty()`, `clear()`, `resize()`
- 修改操作：`push_back()`, `assign()`
- 辅助方法：`swap()`

**特殊处理**：
- `assign(size_t count, bool value)` 方法：
  ```cpp
  void assign(size_t count, bool value) {
      clear();
      resize(count, value);
  }
  ```

---

## 🎯 实现步骤（按优先级）

### 优先级1：MyVectorBarItemPtr
1. **Step 1**: 分析 `SortModel.cpp` 中 `m_barItems` 的所有使用位置
2. **Step 2**: 设计 `MyVectorBarItemPtr.h` 头文件（参考 `MyVectorVertexItemPtr.h`）
3. **Step 3**: 实现 `MyVectorBarItemPtr.cpp` 源文件（参考 `MyVectorVertexItemPtr.cpp`）
4. **Step 4**: 更新 `StructureVisual.pro` 文件
5. **Step 5**: 替换 `SortModel.h` 和 `SortModel.cpp` 中的代码
   - 替换类型声明
   - 替换方法返回类型
   - 将基于范围的 for 循环改为索引循环
6. **Step 6**: 编译检查
7. **Step 7**: 功能验证
8. **Step 8**: 更新项目文档

### 优先级2：MyQueueSortStep
1. **Step 1**: 分析 `SortModel.cpp` 中 `m_algorithmSteps` 的所有使用位置
2. **Step 2**: 设计 `MyQueueSortStep.h` 头文件（参考 `MyQueueAlgorithmStep.h`）
3. **Step 3**: 实现 `MyQueueSortStep.cpp` 源文件（参考 `MyQueueAlgorithmStep.cpp`）
4. **Step 4**: 更新 `StructureVisual.pro` 文件
5. **Step 5**: 替换 `SortModel.h` 和 `SortModel.cpp` 中的代码
   - 替换类型声明
   - 替换方法调用（`empty()`, `front()`, `pop()`, `push()`）
6. **Step 6**: 编译检查
7. **Step 7**: 功能验证
8. **Step 8**: 更新项目文档

### 优先级3：MyVectorBool（可选）
1. **Step 1**: 分析 `SortModel.cpp` 中 `m_visited` 的所有使用位置
2. **Step 2**: 设计 `MyVectorBool.h` 头文件（参考 `MyVectorInt.h`）
3. **Step 3**: 实现 `MyVectorBool.cpp` 源文件（参考 `MyVectorInt.cpp`）
4. **Step 4**: 更新 `StructureVisual.pro` 文件
5. **Step 5**: 替换 `SortModel.h` 和 `SortModel.cpp` 中的代码
   - 替换类型声明
   - 替换 `assign()` 方法调用
6. **Step 6**: 编译检查
7. **Step 7**: 功能验证
8. **Step 8**: 更新项目文档

---

## ⚠️ 注意事项

1. **前向声明**：
   - `MyVectorBarItemPtr.h` 中使用 `class BarItem;` 前向声明
   - `MyQueueSortStep.h` 中使用 `struct SortStep;` 前向声明

2. **头文件包含**：
   - `MyVectorBarItemPtr.cpp` 需要包含 `BarItem.h`（如果存在）或 `SortModel.h`
   - `MyQueueSortStep.cpp` 需要包含 `SortModel.h` 以获取 `SortStep` 的完整定义

3. **所有权语义**：
   - `MyVectorBarItemPtr` 不拥有指针所有权，析构函数不 `delete` 指针
   - `SortModel::clearBarItems()` 负责删除所有 `BarItem*` 指针

4. **基于范围的 for 循环**：
   - 必须改为索引循环，因为自定义容器不支持迭代器

5. **拷贝语义**：
   - `MyVectorBarItemPtr`：浅拷贝（只拷贝指针值）
   - `MyQueueSortStep`：深拷贝（拷贝 `SortStep` 对象，包括内部的 `MyVectorInt` 成员）

6. **编译顺序**：
   - 先实现 `MyVectorBarItemPtr`，编译通过后再实现 `MyQueueSortStep`
   - `MyVectorBool` 可以最后实现（可选）

---

## 📚 参考文档

- `MyVectorVertexItemPtr.h` / `MyVectorVertexItemPtr.cpp` - 指针类型Vector实现参考
- `MyQueueAlgorithmStep.h` / `MyQueueAlgorithmStep.cpp` - 队列实现参考
- `MyVectorInt.h` / `MyVectorInt.cpp` - 基本类型Vector实现参考
- `class_report/使用AI助手的最佳实践.md` - 项目状态文档（Task 1.6）

---

*任务提示词创建时间：2025年12月23日*

