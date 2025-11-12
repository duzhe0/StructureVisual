# 数据结构与算法可视化模拟器

一个基于Qt 6的交互式数据结构和算法可视化学习工具，支持图算法和排序算法的动态演示。

## 🎯 项目概述

本项目旨在通过可视化方式帮助学习者理解各种数据结构和算法的执行过程。系统采用MVC架构设计，提供流畅的动画效果和直观的用户界面。

### ✨ 主要特性

- **图算法可视化**：支持DFS、BFS、Dijkstra、Prim、Kruskal、拓扑排序
- **排序算法可视化**：支持冒泡、选择、插入、快速、归并、堆、基数排序
- **交互式操作**：支持动态添加/删除顶点和边，实时调整参数
- **流畅动画**：基于QPropertyAnimation的高质量动画效果
- **多布局支持**：圆形布局、力导向布局、网格布局
- **完整控制**：开始、暂停、继续、停止、单步执行
- **中文界面**：完整的中文用户界面

## 🛠️ 技术栈

- **框架**：Qt 6.10.0
- **语言**：C++17
- **编译器**：MinGW 64-bit
- **架构**：MVC (Model-View-Controller)
- **图形**：QGraphicsView/QGraphicsScene
- **动画**：QPropertyAnimation

## 📋 系统要求

- Windows 10/11
- Qt 6.10.0 或更高版本
- MinGW 64-bit 编译器
- 至少 4GB RAM
- 支持OpenGL的显卡（推荐）

## 🚀 快速开始

### 环境准备

1. 安装Qt 6.10.0 MinGW 64-bit
2. 确保Qt Creator已正确配置
3. 克隆或下载项目到本地

### 编译运行

#### 方法1：运行主程序（推荐）
```bash
# 在Qt Creator中打开
StructureVisual.pro

# 点击"构建"按钮
# 点击"运行"按钮
```

#### 方法2：运行BFS演示程序
```bash
# 在Qt Creator中打开
demo_bfs.cpp

# 右键选择"运行"
```

#### 方法3：运行简化测试程序
```bash
# 在Qt Creator中打开
simple_visual_test.cpp

# 右键选择"运行"
```

## 📖 使用指南

### 主程序使用

#### 图算法模式
1. **创建图结构**：
   - 在"图操作"区域输入顶点标签（如"A"）
   - 点击"添加顶点"按钮
   - 输入边的起点和终点，设置权重，点击"添加边"

2. **选择算法**：
   - 在"算法选择"下拉菜单中选择算法
   - 设置起始顶点（可选）
   - 选择有向图/无向图

3. **运行算法**：
   - 点击"开始"按钮开始算法
   - 使用"暂停"、"继续"、"停止"控制执行
   - 调整动画速度滑块

4. **布局控制**：
   - 使用"圆形布局"、"力导向布局"、"网格布局"按钮
   - 不同布局适合不同类型的图

#### 排序算法模式
1. **生成数据**：
   - 设置数据大小（5-50个元素）
   - 设置数值范围
   - 点击"生成随机数据"

2. **选择排序算法**：
   - 从下拉菜单中选择排序算法
   - 观察不同算法的执行特点

3. **运行排序**：
   - 点击"开始"按钮
   - 观察柱状图的排序过程

### BFS演示程序使用

1. **启动程序**后，您会看到：
   - 左侧：控制面板和算法说明
   - 右侧：图形显示区域

2. **操作步骤**：
   - 点击"创建示例图"按钮
   - 系统自动创建3x3网格图
   - 点击"开始BFS算法"按钮
   - 观察BFS逐层访问过程

3. **学习要点**：
   - BFS从顶点A开始
   - 先访问所有相邻顶点（B、D）
   - 再访问下一层顶点（C、E、G）
   - 最后访问最远顶点（F、H、I）

## 🎮 快捷键

- `F5`：开始算法
- `Ctrl+N`：新建
- `Ctrl+O`：打开
- `Ctrl+S`：保存
- `Ctrl+I`：放大
- `Ctrl+O`：缩小
- `Ctrl+0`：适应窗口

## 📁 项目结构

```
StructureVisual/
├── 核心文件/
│   ├── main.cpp                 # 主程序入口
│   ├── mainwindow.h/cpp         # 主窗口
│   ├── mainwindow.ui            # 主窗口UI设计
│   └── StructureVisual.pro      # 主项目文件
├── 模型层/
│   ├── GraphModel.h/cpp         # 图数据模型
│   ├── SortModel.h/cpp          # 排序数据模型
│   └── model.h/cpp              # 基础模型
├── 视图层/
│   ├── VisualItem.h/cpp         # 可视化元素基类
│   └── 各种可视化项实现
├── 控制器层/
│   └── AlgorithmController.h/cpp # 算法控制器
├── 数据结构/
│   ├── Graph.h                  # 图接口
│   ├── ListGraph.h              # 邻接表图实现
│   ├── MatrixGraph.h            # 邻接矩阵图实现
│   ├── HashMap.h                # 哈希表实现
│   ├── dynamicarray.h/cpp       # 动态数组
│   └── simplestring.h/cpp       # 简单字符串
├── 演示程序/
│   ├── demo_bfs.cpp             # BFS算法演示
│   ├── simple_visual_test.cpp   # 简化可视化测试
│   └── test_*.cpp               # 各种测试程序
└── 文档/
    ├── README.md                # 项目说明
    ├── ARCHITECTURE.md          # 架构文档
    └── 其他文档...
```

## 🔧 开发指南

### 添加新算法

1. **图算法**：
   - 在`GraphAlgorithm`枚举中添加新算法类型
   - 在`GraphModel`中实现`generateXXXSteps()`函数
   - 在`GraphAlgorithmController`中添加算法名称

2. **排序算法**：
   - 在`SortAlgorithm`枚举中添加新算法类型
   - 在`SortModel`中实现算法逻辑
   - 在`SortAlgorithmController`中添加算法名称

### 自定义可视化

1. **继承VisualItem**：
   ```cpp
   class CustomItem : public VisualItem {
       // 实现boundingRect()和paint()方法
   };
   ```

2. **添加动画效果**：
   ```cpp
   void startCustomAnimation() {
       // 使用QPropertyAnimation创建动画
   }
   ```

## 🐛 故障排除

### 调试技巧

1. **使用qDebug()输出调试信息**
2. **检查QGraphicsScene的itemsBoundingRect()**
3. **使用Qt Creator的调试器**

## 📚 学习资源

- [Qt 6官方文档](https://doc.qt.io/qt-6/)
- [QGraphicsView框架](https://doc.qt.io/qt-6/qgraphicsview.html)
- [算法可视化最佳实践](https://visualgo.net/)

## 🤝 贡献指南

1. Fork项目
2. 创建特性分支
3. 提交更改
4. 推送到分支
5. 创建Pull Request

## 📞 联系方式

如有问题或建议，请通过以下方式联系：
- 创建Issue
- 发送邮件
- 提交Pull Request

## 🙏 致谢

感谢所有为这个项目做出贡献的开发者和学习者！

---

**注意**：本项目主要用于教育和学习目的，代码结构清晰，注释详细，适合算法学习和Qt开发入门。