# 系统架构文档

## 📐 整体架构

本项目采用经典的**MVC (Model-View-Controller)** 架构模式，结合Qt的图形框架，构建了一个可扩展的算法可视化系统。

```
┌─────────────────────────────────────────────────────────────┐
│                    用户界面层 (UI Layer)                      │
├─────────────────────────────────────────────────────────────┤
│  MainWindow  │  BFSDemoWindow  │  SimpleVisualTestWindow    │
├─────────────────────────────────────────────────────────────┤
│                    控制器层 (Controller Layer)                │
├─────────────────────────────────────────────────────────────┤
│  GraphAlgorithmController  │  SortAlgorithmController       │
├─────────────────────────────────────────────────────────────┤
│                    模型层 (Model Layer)                      │
├─────────────────────────────────────────────────────────────┤
│     GraphModel      │      SortModel      │    VisualItem    │
├─────────────────────────────────────────────────────────────┤
│                    数据结构层 (Data Structure Layer)          │
├─────────────────────────────────────────────────────────────┤
│  IGraph  │  ListGraph  │  MatrixGraph  │  HashMap  │  Array  │
└─────────────────────────────────────────────────────────────┘
```

## 🏗️ 核心组件详解

### 1. 模型层 (Model Layer)

#### GraphModel - 图数据模型
```cpp
class GraphModel : public QObject
{
    // 核心数据结构
    std::map<QString, VertexItem*> m_vertices;           // 顶点映射
    std::map<std::pair<QString, QString>, EdgeItem*> m_edges; // 边映射
    std::map<QString, std::set<QString>> m_adjacencyList; // 邻接表
    
    // 算法执行状态
    GraphAlgorithm m_currentAlgorithm;                   // 当前算法
    bool m_algorithmRunning;                             // 算法运行状态
    std::queue<AlgorithmStep> m_algorithmSteps;          // 算法步骤队列
    QTimer *m_algorithmTimer;                           // 算法定时器
};
```

**职责**：
- 管理图的顶点和边数据
- 执行图算法并生成步骤
- 维护算法执行状态
- 提供图操作接口（添加/删除顶点和边）

**关键方法**：
- `addVertex()` / `removeVertex()` - 顶点管理
- `addEdge()` / `removeEdge()` - 边管理
- `executeAlgorithm()` - 算法执行
- `generateXXXSteps()` - 算法步骤生成

#### SortModel - 排序数据模型
```cpp
class SortModel : public QObject
{
    // 数据存储
    std::vector<int> m_data;                            // 排序数据
    std::vector<BarItem*> m_bars;                       // 可视化柱状图
    
    // 算法状态
    SortAlgorithm m_currentAlgorithm;                   // 当前算法
    bool m_algorithmRunning;                            // 运行状态
    std::queue<SortStep> m_algorithmSteps;              // 步骤队列
};
```

**职责**：
- 管理排序数据
- 执行排序算法
- 生成排序步骤
- 维护数据状态

### 2. 视图层 (View Layer)

#### VisualItem - 可视化元素基类
```cpp
class VisualItem : public QObject, public QGraphicsItem
{
    // 状态管理
    VisualState m_state;                                // 可视化状态
    QColor m_baseColor;                                 // 基础颜色
    QString m_text;                                     // 显示文本
    
    // 动画系统
    QPropertyAnimation *m_highlightAnimation;           // 高亮动画
    QPropertyAnimation *m_pulseAnimation;               // 脉冲动画
    QGraphicsDropShadowEffect *m_shadowEffect;          // 阴影效果
};
```

**设计模式**：**模板方法模式**
- 定义通用的可视化接口
- 子类实现具体的绘制逻辑
- 统一的动画和状态管理

#### 具体可视化项

**VertexItem - 顶点可视化**
```cpp
class VertexItem : public VisualItem
{
    QString m_label;                                    // 顶点标签
    qreal m_radius;                                     // 顶点半径
    QColor m_borderColor;                               // 边框颜色
    int m_borderWidth;                                  // 边框宽度
};
```

**EdgeItem - 边可视化**
```cpp
class EdgeItem : public VisualItem
{
    VertexItem *m_fromVertex;                           // 起始顶点
    VertexItem *m_toVertex;                             // 目标顶点
    int m_weight;                                       // 边权重
    bool m_isDirected;                                  // 是否有向
};
```

**BarItem - 柱状图可视化**
```cpp
class BarItem : public VisualItem
{
    int m_value;                                        // 数值
    int m_index;                                        // 索引
    qreal m_barWidth;                                   // 柱宽
    qreal m_maxHeight;                                  // 最大高度
};
```

### 3. 控制器层 (Controller Layer)

#### AlgorithmController - 算法控制器基类
```cpp
class AlgorithmController : public QObject
{
    // 控制面板组件
    QWidget *m_controlPanel;                            // 控制面板
    QPushButton *m_startButton;                         // 开始按钮
    QPushButton *m_pauseButton;                         // 暂停按钮
    QSlider *m_speedSlider;                             // 速度滑块
    
    // 状态管理
    bool m_isRunning;                                   // 运行状态
    bool m_isPaused;                                    // 暂停状态
    int m_animationSpeed;                               // 动画速度
};
```

**设计模式**：**策略模式**
- 定义算法控制的通用接口
- 子类实现具体的算法控制逻辑
- 统一的用户界面和状态管理

#### GraphAlgorithmController - 图算法控制器
```cpp
class GraphAlgorithmController : public AlgorithmController
{
    GraphModel *m_graphModel;                           // 图模型引用
    
    // 图操作组件
    QComboBox *m_algorithmComboBox;                     // 算法选择
    QLineEdit *m_startVertexEdit;                       // 起始顶点输入
    QCheckBox *m_directedCheckBox;                      // 有向图选择
    
    // 图操作按钮
    QPushButton *m_addVertexButton;                     // 添加顶点
    QPushButton *m_addEdgeButton;                       // 添加边
    QPushButton *m_circularLayoutButton;                // 圆形布局
};
```

**职责**：
- 管理图算法的用户界面
- 处理用户输入和操作
- 协调模型和视图的交互
- 提供图操作功能

#### SortAlgorithmController - 排序算法控制器
```cpp
class SortAlgorithmController : public AlgorithmController
{
    SortModel *m_sortModel;                             // 排序模型引用
    
    // 数据管理组件
    QSpinBox *m_dataSizeSpinBox;                        // 数据大小
    QSpinBox *m_minValueSpinBox;                        // 最小值
    QSpinBox *m_maxValueSpinBox;                        // 最大值
    QPushButton *m_generateDataButton;                  // 生成数据
};
```

## 🔄 数据流和交互模式

### 1. 用户操作流程
```
用户操作 → 控制器 → 模型 → 视图更新 → 用户反馈
```

### 2. 算法执行流程
```
算法启动 → 步骤生成 → 定时器触发 → 视图动画 → 状态更新
```

### 3. 具体示例：BFS算法执行
```cpp
// 1. 用户点击开始按钮
void GraphAlgorithmController::onStartButtonClicked() {
    startAlgorithm();
}

// 2. 控制器调用模型执行算法
void GraphAlgorithmController::startAlgorithm() {
    m_graphModel->executeAlgorithm(GraphAlgorithm::BFS, startVertex);
}

// 3. 模型生成算法步骤
void GraphModel::executeAlgorithm(GraphAlgorithm algorithm, const QString &startVertex) {
    generateBFSSteps(startVertex);
    m_algorithmTimer->start();
}

// 4. 定时器触发步骤执行
void GraphModel::processNextStep() {
    if (!m_algorithmSteps.empty()) {
        AlgorithmStep step = m_algorithmSteps.front();
        m_algorithmSteps.pop();
        
        // 更新可视化状态
        updateVisualization(step);
        
        // 继续下一步
        m_algorithmTimer->start(step.delay);
    }
}
```

## 🎨 设计模式应用

### 1. MVC模式
- **Model**: GraphModel, SortModel - 数据管理和业务逻辑
- **View**: VisualItem及其子类 - 数据展示和用户交互
- **Controller**: AlgorithmController及其子类 - 用户输入处理和协调

### 2. 观察者模式
```cpp
// 模型发出信号
emit algorithmStarted(algorithm);
emit algorithmStepCompleted(step);
emit algorithmCompleted(algorithm);

// 控制器监听信号
connect(m_graphModel, &GraphModel::algorithmStarted, 
        this, &GraphAlgorithmController::onAlgorithmStarted);
```

### 3. 策略模式
```cpp
// 算法选择策略
switch (algorithm) {
    case GraphAlgorithm::DFS:
        generateDFSSteps(startVertex);
        break;
    case GraphAlgorithm::BFS:
        generateBFSSteps(startVertex);
        break;
    // ... 其他算法
}
```

### 4. 模板方法模式
```cpp
// VisualItem基类定义通用接口
class VisualItem {
    virtual QRectF boundingRect() const = 0;
    virtual void paint(QPainter *painter, ...) = 0;
};

// 子类实现具体逻辑
class VertexItem : public VisualItem {
    QRectF boundingRect() const override { /* 实现 */ }
    void paint(QPainter *painter, ...) override { /* 实现 */ }
};
```

### 5. 工厂模式
```cpp
// 根据算法类型创建不同的步骤
AlgorithmStep createStep(GraphAlgorithm algorithm, ...) {
    switch (algorithm) {
        case GraphAlgorithm::DFS:
            return createDFSStep(...);
        case GraphAlgorithm::BFS:
            return createBFSStep(...);
        // ...
    }
}
```

## 🔧 扩展性设计

### 1. 添加新算法
```cpp
// 1. 在枚举中添加新算法
enum class GraphAlgorithm {
    DFS, BFS, Dijkstra, Prim, Kruskal, TopologicalSort,
    NewAlgorithm  // 新增算法
};

// 2. 在模型中实现算法逻辑
void GraphModel::generateNewAlgorithmSteps(const QString &startVertex) {
    // 实现新算法的步骤生成逻辑
}

// 3. 在控制器中添加算法名称
QString GraphAlgorithmController::getCurrentAlgorithmName() const {
    switch (index) {
        case 6: return "新算法";
        // ...
    }
}
```

### 2. 添加新可视化元素
```cpp
// 1. 继承VisualItem基类
class NewVisualItem : public VisualItem {
public:
    explicit NewVisualItem(QGraphicsItem *parent = nullptr);
    
protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, ...) override;
    
private:
    // 新元素特有的属性
};
```

### 3. 添加新控制器
```cpp
// 1. 继承AlgorithmController基类
class NewAlgorithmController : public AlgorithmController {
public:
    explicit NewAlgorithmController(QObject *parent = nullptr);
    
    // 实现纯虚函数
    QWidget* createControlPanel() override;
    void startAlgorithm() override;
    // ... 其他方法
};
```

## 🎯 性能优化策略

### 1. 内存管理
- 使用智能指针管理动态分配的对象
- 及时清理不再使用的可视化元素
- 避免内存泄漏

### 2. 动画优化
- 使用QPropertyAnimation的硬件加速
- 合理设置动画帧率和持续时间
- 避免同时运行过多动画

### 3. 渲染优化
- 使用QGraphicsView的缓存机制
- 合理设置场景更新模式
- 避免频繁的重绘操作

## 🔍 调试和测试

### 1. 调试工具
- Qt Creator集成调试器
- qDebug()输出调试信息
- QGraphicsScene的调试功能

### 2. 测试策略
- 单元测试：测试各个组件的功能
- 集成测试：测试组件间的交互
- 用户测试：验证用户体验

### 3. 日志系统
```cpp
void GraphModel::logMessage(const QString &message) {
    qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss") 
             << "GraphModel:" << message;
}
```

## 📈 未来扩展方向

### 1. 功能扩展
- 添加更多图算法（A*、Floyd-Warshall等）
- 支持更多排序算法（桶排序、计数排序等）
- 添加树结构可视化
- 支持动态数据结构（栈、队列、链表）

### 2. 技术改进
- 使用OpenGL加速渲染
- 添加3D可视化支持
- 实现分布式算法演示
- 添加算法复杂度分析

### 3. 用户体验
- 添加音效和语音提示
- 支持触摸屏操作
- 实现算法步骤回放
- 添加学习进度跟踪

---

这个架构文档详细说明了系统的设计思路、实现细节和扩展方法，帮助开发者深入理解代码结构，便于后续的维护和扩展。
