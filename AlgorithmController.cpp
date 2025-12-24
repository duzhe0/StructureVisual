#include "AlgorithmController.h"
#include "GraphModel.h"
#include "SortModel.h"
#include "MatrixRepresentationDialog.h"
#include "AdjacencyListRepresentationDialog.h"
#include "DijkstraTableDialog.h"
#include <QApplication>
#include <QDateTime>
#include <QLineEdit>
#include <QTextCursor>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
//代码阅读了图的一半，排序还没看 应该差不多

// ==================== AlgorithmController 基类实现 ====================

AlgorithmController::AlgorithmController(QObject *parent)
    : QObject(parent)
    , m_controlPanel(nullptr)
    , m_startButton(nullptr)
    , m_pauseButton(nullptr)
    , m_resumeButton(nullptr)
    , m_stopButton(nullptr)
    , m_stepButton(nullptr)
    , m_resetButton(nullptr)
    , m_logTextEdit(nullptr)
    , m_isRunning(false)
    , m_isPaused(false)
{
}

void AlgorithmController::setupControlPanel()//ok
{
    m_controlPanel = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(m_controlPanel);
    
    // 算法控制按钮组
    QGroupBox *controlGroup = new QGroupBox("算法控制");
    QHBoxLayout *buttonLayout = new QHBoxLayout(controlGroup);
    
    m_startButton = new QPushButton("开始");
    m_pauseButton = new QPushButton("暂停");
    m_resumeButton = new QPushButton("继续");
    m_stopButton = new QPushButton("停止");
    m_stepButton = new QPushButton("单步");
    m_resetButton = new QPushButton("重置");
    
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_pauseButton);
    buttonLayout->addWidget(m_resumeButton);
    buttonLayout->addWidget(m_stopButton);
    buttonLayout->addWidget(m_stepButton);
    buttonLayout->addWidget(m_resetButton);
    
    // 日志显示
    QGroupBox *logGroup = new QGroupBox("算法日志");
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    
    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setMaximumHeight(150);
    m_logTextEdit->setReadOnly(true);
    
    logLayout->addWidget(m_logTextEdit);
    
    // 组装主布局
    mainLayout->addWidget(controlGroup);
    mainLayout->addWidget(logGroup);
    mainLayout->addStretch();
    
    updateButtonStates();
}

void AlgorithmController::updateButtonStates()//ok
{
    if (!m_startButton) return;
    
    m_startButton->setEnabled(!m_isRunning);
    m_pauseButton->setEnabled(m_isRunning && !m_isPaused);
    m_resumeButton->setEnabled(m_isRunning && m_isPaused);//中断后继续开始
    m_stopButton->setEnabled(m_isRunning);
    m_stepButton->setEnabled(m_isRunning && m_isPaused);
    m_resetButton->setEnabled(!m_isRunning);
}

void AlgorithmController::logMessage(const QString &message)//ok
//写算法日志的函数
{
    if (m_logTextEdit) {
        m_logTextEdit->append(QString("[%1] %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(message));
        m_logTextEdit->moveCursor(QTextCursor::End);
    }
}

// ==================== GraphAlgorithmController 实现 ====================

GraphAlgorithmController::GraphAlgorithmController(QObject *parent)//ok
    : AlgorithmController(parent)
    , m_graphModel(nullptr)
    , m_dijkstraTableDialog(nullptr)
    , m_algorithmComboBox(nullptr)
    , m_startVertexEdit(nullptr)
    , m_directedCheckBox(nullptr)
    , m_circularLayoutButton(nullptr)
    , m_forceLayoutButton(nullptr)
    , m_gridLayoutButton(nullptr)
    , m_gridColumnsSpinBox(nullptr)
    , m_graphOperationsGroup(nullptr)
    , m_vertexLabelEdit(nullptr)
    , m_addVertexButton(nullptr)
    , m_removeVertexButton(nullptr)
    , m_edgeFromEdit(nullptr)
    , m_edgeToEdit(nullptr)
    , m_edgeWeightSpinBox(nullptr)
    , m_addEdgeButton(nullptr)
    , m_removeEdgeButton(nullptr)
{
    setupControlPanel();
}

QWidget* GraphAlgorithmController::createControlPanel()//ok
{
    return m_controlPanel;
}

void GraphAlgorithmController::setGraphModel(GraphModel *model)//ok
//先断开，后连接
{
    if (m_graphModel) {
        //断开从 m_graphModel 发出的所有信号到 this（当前控制器类对象）的所有槽函数的连接
        disconnect(m_graphModel, nullptr, this, nullptr);
    }
    
    m_graphModel = model;
    
    if (m_graphModel) {
        connect(m_graphModel, &GraphModel::algorithmStarted, this, &GraphAlgorithmController::onAlgorithmStarted);
        connect(m_graphModel, &GraphModel::algorithmStepCompleted, this, &GraphAlgorithmController::onAlgorithmStepCompleted);
        connect(m_graphModel, &GraphModel::algorithmCompleted, this, &GraphAlgorithmController::onAlgorithmCompleted);
        connect(m_graphModel, &GraphModel::algorithmPaused, this, &GraphAlgorithmController::onAlgorithmPaused);
        connect(m_graphModel, &GraphModel::algorithmResumed, this, &GraphAlgorithmController::onAlgorithmResumed);
        connect(m_graphModel, &GraphModel::algorithmStopped, this, &GraphAlgorithmController::onAlgorithmStopped);
        // 连接Dijkstra表格更新信号
        connect(m_graphModel, &GraphModel::dijkstraTableUpdate, this, &GraphAlgorithmController::onDijkstraTableUpdate);
    }
}

void GraphAlgorithmController::startAlgorithm()//ok
{
    if (!m_graphModel) return;
    
    QString startVertex = m_startVertexEdit->text().trimmed();
    if (startVertex.isEmpty() && m_graphModel->getVertexCount() > 0) {
        // 如果没有指定起始顶点，使用第一个顶点
        auto vertices = m_graphModel->getAllVertices();
        if (!vertices.empty()) {
            startVertex = vertices[0]->getLabel();
        }
    }
    
    int algorithmIndex = m_algorithmComboBox->currentIndex();
    //Index转换为枚举类
    GraphAlgorithm algorithm = static_cast<GraphAlgorithm>(algorithmIndex);
    
    // 无论切换到什么算法，都要先关闭之前的Dijkstra表格对话框（如果存在）
    if (m_dijkstraTableDialog) {
        // 先断开信号连接，防止对话框删除后仍有信号发送
        if (m_graphModel) {
            disconnect(m_graphModel, &GraphModel::dijkstraTableUpdate, this, &GraphAlgorithmController::onDijkstraTableUpdate);
        }
        
        // 关闭对话框（由于设置了WA_DeleteOnClose，会自动删除）
        // QPointer会在对象删除后自动变为nullptr
        m_dijkstraTableDialog->close();
        m_dijkstraTableDialog.clear();  // 使用clear()而不是直接赋值nullptr
        
        // 重新连接信号（如果模型还存在）
        if (m_graphModel) {
            connect(m_graphModel, &GraphModel::dijkstraTableUpdate, this, &GraphAlgorithmController::onDijkstraTableUpdate);
        }
    }
    
    // 检查图是否为空
    if (m_graphModel && m_graphModel->isEmpty()) {
        logMessage("错误：图为空，无法执行算法。请先添加顶点和边。");
        return;
    }
    
    // 如果是Dijkstra算法，创建并显示表格对话框
    if (algorithm == GraphAlgorithm::Dijkstra && m_graphModel) {
        // 创建新的表格对话框
        m_dijkstraTableDialog = new DijkstraTableDialog(startVertex, nullptr);
        m_dijkstraTableDialog->setAttribute(Qt::WA_DeleteOnClose);
        
        // 初始化表格：获取所有顶点
        MyVectorVertexItemPtr vertices = m_graphModel->getAllVertices();
        QStringList vertexList;
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertexList.append(vertices[i]->getLabel());
        }
        m_dijkstraTableDialog->initializeTable(vertexList);
        
        // 显示对话框
        m_dijkstraTableDialog->show();
    }
    
    m_graphModel->executeAlgorithm(algorithm, startVertex);
    logMessage(QString("开始执行算法: %1").arg(getCurrentAlgorithmName()));
}

void GraphAlgorithmController::pauseAlgorithm()//ok
{
    if (m_graphModel) {
        m_graphModel->pauseAlgorithm();
    }
}

void GraphAlgorithmController::resumeAlgorithm()//ok
{
    if (m_graphModel) {
        m_graphModel->resumeAlgorithm();
    }
}

void GraphAlgorithmController::stopAlgorithm()//ok
{
    if (m_graphModel) {
        m_graphModel->stopAlgorithm();
    }
}

void GraphAlgorithmController::stepAlgorithm()//ok
{
    if (m_graphModel) {
        m_graphModel->stepAlgorithm();
    }
}

void GraphAlgorithmController::resetAlgorithm()//ok
{
    if (m_graphModel) {
        m_graphModel->resetVisualization();
        logMessage("算法已重置");
    }
}

void GraphAlgorithmController::addVertex(const QString &label, const QPointF &position)//ok
{
    // 检查算法是否正在运行
    if (m_isRunning) {
        logMessage("错误：算法正在执行中，无法添加顶点。请先停止算法。");
        return;
    }
    
    if (m_graphModel) {
        if (m_graphModel->addVertex(label, position)) {
            logMessage(QString("添加顶点: %1").arg(label));
        } else {
            logMessage(QString("添加顶点失败: %1 (可能已存在)").arg(label));
        }
    }
}

void GraphAlgorithmController::removeVertex(const QString &label)//ok
{
    // 检查算法是否正在运行
    if (m_isRunning) {
        logMessage("错误：算法正在执行中，无法删除顶点。请先停止算法。");
        return;
    }
    
    if (m_graphModel) {
        if (m_graphModel->removeVertex(label)) {
            logMessage(QString("删除顶点: %1").arg(label));
        } else {
            logMessage(QString("删除顶点失败: %1 (不存在)").arg(label));
        }
    }
}

void GraphAlgorithmController::addEdge(const QString &from, const QString &to, int weight)//ok
{
    // 检查算法是否正在运行
    if (m_isRunning) {
        logMessage("错误：算法正在执行中，无法添加边。请先停止算法。");
        return;
    }
    
    if (m_graphModel) {
        if (m_graphModel->addEdge(from, to, weight)) {
            logMessage(QString("添加边: %1 -> %2 (权重: %3)").arg(from).arg(to).arg(weight));
        } else {
            logMessage(QString("添加边失败: %1 -> %2").arg(from).arg(to));
        }
    }
}

void GraphAlgorithmController::removeEdge(const QString &from, const QString &to)//ok
{
    // 检查算法是否正在运行
    if (m_isRunning) {
        logMessage("错误：算法正在执行中，无法删除边。请先停止算法。");
        return;
    }
    
    if (m_graphModel) {
        if (m_graphModel->removeEdge(from, to)) {
            logMessage(QString("删除边: %1 -> %2").arg(from).arg(to));
        } else {
            logMessage(QString("删除边失败: %1 -> %2").arg(from).arg(to));
        }
    }
}

void GraphAlgorithmController::applyCircularLayout()//ok
{
    if (m_graphModel) {
        m_graphModel->applyCircularLayout();
        logMessage("应用圆形布局");
    }
}

void GraphAlgorithmController::applyForceDirectedLayout()//ok
{
    if (m_graphModel) {
        m_graphModel->applyForceDirectedLayout();
        logMessage("应用力导向布局");
    }
}

void GraphAlgorithmController::applyGridLayout(int columns)//ok
{
    if (m_graphModel) {
        m_graphModel->applyGridLayout(columns);
        logMessage(QString("应用网格布局 (列数: %1)").arg(columns));
    }
}

void GraphAlgorithmController::setAlgorithm(int algorithmIndex)//ok
{
    if (m_algorithmComboBox) {
        m_algorithmComboBox->setCurrentIndex(algorithmIndex);
    }
}

QString GraphAlgorithmController::getCurrentAlgorithmName() const//ok
{
    if (!m_algorithmComboBox) return "";
    
    int index = m_algorithmComboBox->currentIndex();
    switch (index) {
        case 0: return "深度优先搜索 (DFS)";
        case 1: return "广度优先搜索 (BFS)";
        case 2: return "Dijkstra最短路径";
        case 3: return "Prim最小生成树";
        case 4: return "Kruskal最小生成树";
        case 5: return "拓扑排序";
        default: return "未知算法";
    }
}

void GraphAlgorithmController::setupControlPanel()//ok
{
    AlgorithmController::setupControlPanel();
    setupGraphControlPanel();
    setupGraphOperationsPanel();
    setupLayoutPanel();
    
    // 连接按钮信号
    connect(m_startButton, &QPushButton::clicked, this, &GraphAlgorithmController::onStartButtonClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &GraphAlgorithmController::onPauseButtonClicked);
    connect(m_resumeButton, &QPushButton::clicked, this, &GraphAlgorithmController::onResumeButtonClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &GraphAlgorithmController::onStopButtonClicked);
    connect(m_stepButton, &QPushButton::clicked, this, &GraphAlgorithmController::onStepButtonClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &GraphAlgorithmController::onResetButtonClicked);
}

void GraphAlgorithmController::setupGraphControlPanel()//ok
//窗口-窗口布局-子窗口-子窗口布局-子窗口组件
//链接 qt预设的组件信号 与 槽函数
{
    //layout方法默认返回基类指针 要用子类指针的功能需要向下转换
    //转换完做检查保证可用
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(m_controlPanel->layout());
    if (!mainLayout) return;
    
    // 算法选择组
    QGroupBox *algorithmGroup = new QGroupBox("算法选择");
    QVBoxLayout *algorithmLayout = new QVBoxLayout(algorithmGroup);
    
    m_algorithmComboBox = new QComboBox();
    m_algorithmComboBox->addItems({
        "深度优先搜索 (DFS)",
        "广度优先搜索 (BFS)",
        "Dijkstra最短路径",
        "Prim最小生成树",
        "Kruskal最小生成树",
        "拓扑排序"
    });
    
    m_startVertexEdit = new QLineEdit();
    m_startVertexEdit->setPlaceholderText("起始顶点标签");
    
    m_directedCheckBox = new QCheckBox("有向图");
    m_directedCheckBox->setChecked(true);
    
    algorithmLayout->addWidget(new QLabel("算法:"));
    algorithmLayout->addWidget(m_algorithmComboBox);
    algorithmLayout->addWidget(new QLabel("起始顶点:"));
    algorithmLayout->addWidget(m_startVertexEdit);
    algorithmLayout->addWidget(m_directedCheckBox);
    
    mainLayout->insertWidget(0, algorithmGroup);
    
    // connect(谁变化，发出的信号种类，要启动哪里的槽函数，槽函数的地址)
    connect(m_algorithmComboBox, &QComboBox::currentIndexChanged,
            this, &GraphAlgorithmController::onAlgorithmComboBoxChanged);
    connect(m_startVertexEdit, &QLineEdit::textChanged,
            this, &GraphAlgorithmController::onStartVertexChanged);
    connect(m_directedCheckBox, &QCheckBox::toggled,
            this, &GraphAlgorithmController::onDirectedCheckBoxToggled);//toggled 切换
    
    // 颜色状态说明组
    QGroupBox *colorLegendGroup = new QGroupBox("颜色说明");
    QHBoxLayout *colorLegendLayout = new QHBoxLayout(colorLegendGroup);
    colorLegendLayout->setSpacing(10);
    
    // 创建颜色标签
    auto createColorLabel = [](const QString &text, const QColor &color) -> QLabel* {
        QLabel *label = new QLabel();
        label->setText(QString("<span style='background-color: %1; color: black; padding: 2px 6px; border-radius: 3px;'>%2</span>")
                      .arg(color.name())
                      .arg(text));
        return label;
    };
    
    // 添加各个状态的颜色标签
    colorLegendLayout->addWidget(createColorLabel("正常", QColor(0, 0, 255)));      // Normal - 蓝色
    colorLegendLayout->addWidget(createColorLabel("已访问", QColor(100, 200, 100))); // Visited - 绿色
    colorLegendLayout->addWidget(createColorLabel("当前", QColor(255, 165, 0)));     // Current - 橙色
    colorLegendLayout->addWidget(createColorLabel("选中", QColor(255, 0, 0)));       // Selected - 红色
    colorLegendLayout->addWidget(createColorLabel("处理中", QColor(255, 255, 0)));  // Processing - 黄色
    
    colorLegendLayout->addStretch();
    
    mainLayout->insertWidget(1, colorLegendGroup);
}

void GraphAlgorithmController::setupGraphOperationsPanel()//ok
{
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(m_controlPanel->layout());
    if (!mainLayout) return;
    
    m_graphOperationsGroup = new QGroupBox("图操作");
    //网格布局 更精确定位 行坐标 列坐标
    QGridLayout *operationsLayout = new QGridLayout(m_graphOperationsGroup);
    
    // 顶点操作
    m_vertexLabelEdit = new QLineEdit();
    m_vertexLabelEdit->setPlaceholderText("顶点标签");
    
    m_addVertexButton = new QPushButton("添加顶点");
    m_removeVertexButton = new QPushButton("删除顶点");
    
    operationsLayout->addWidget(new QLabel("顶点:"), 0, 0);
    operationsLayout->addWidget(m_vertexLabelEdit, 0, 1);
    operationsLayout->addWidget(m_addVertexButton, 0, 2);
    operationsLayout->addWidget(m_removeVertexButton, 0, 3);
    
    // 边操作
    m_edgeFromEdit = new QLineEdit();
    m_edgeFromEdit->setPlaceholderText("起点");
    
    m_edgeToEdit = new QLineEdit();
    m_edgeToEdit->setPlaceholderText("终点");
    
    m_edgeWeightSpinBox = new QSpinBox();
    m_edgeWeightSpinBox->setRange(1, 100);
    m_edgeWeightSpinBox->setValue(1);
    
    m_addEdgeButton = new QPushButton("添加边");
    m_removeEdgeButton = new QPushButton("删除边");
    
    operationsLayout->addWidget(new QLabel("边:"), 1, 0);
    operationsLayout->addWidget(m_edgeFromEdit, 1, 1);
    operationsLayout->addWidget(new QLabel("->"), 1, 2);
    operationsLayout->addWidget(m_edgeToEdit, 1, 3);
    operationsLayout->addWidget(new QLabel("权重:"), 1, 4);
    operationsLayout->addWidget(m_edgeWeightSpinBox, 1, 5);
    operationsLayout->addWidget(m_addEdgeButton, 1, 6);
    operationsLayout->addWidget(m_removeEdgeButton, 1, 7);
    
    //把界面当作序列 后面的自动后移 插入第二个位置
    mainLayout->insertWidget(1, m_graphOperationsGroup);
    
    // 连接信号
    connect(m_addVertexButton, &QPushButton::clicked, [this]() {
        QString label = m_vertexLabelEdit->text().trimmed();//trimmed修剪
        if (!label.isEmpty()) {
            addVertex(label);
            m_vertexLabelEdit->clear();
        }
    });
    
    connect(m_removeVertexButton, &QPushButton::clicked, [this]() {
        QString label = m_vertexLabelEdit->text().trimmed();
        if (!label.isEmpty()) {
            removeVertex(label);
            //清除输入框内容
            m_vertexLabelEdit->clear();
        }
    });
    
    connect(m_addEdgeButton, &QPushButton::clicked, [this]() {
        QString from = m_edgeFromEdit->text().trimmed();
        QString to = m_edgeToEdit->text().trimmed();
        int weight = m_edgeWeightSpinBox->value();
        
        if (!from.isEmpty() && !to.isEmpty()) {
            addEdge(from, to, weight);
            m_edgeFromEdit->clear();
            m_edgeToEdit->clear();
        }
    });
    
    connect(m_removeEdgeButton, &QPushButton::clicked, [this]() {
        QString from = m_edgeFromEdit->text().trimmed();
        QString to = m_edgeToEdit->text().trimmed();
        
        if (!from.isEmpty() && !to.isEmpty()) {
            removeEdge(from, to);
            m_edgeFromEdit->clear();
            m_edgeToEdit->clear();
        }
    });
    
    // 图表示按钮
    m_showMatrixButton = new QPushButton("显示矩阵表示");
    m_showAdjacencyListButton = new QPushButton("显示邻接表表示");
    
    operationsLayout->addWidget(m_showMatrixButton, 2, 0, 1, 2);
    operationsLayout->addWidget(m_showAdjacencyListButton, 2, 2, 1, 2);
    
    connect(m_showMatrixButton, &QPushButton::clicked, this, &GraphAlgorithmController::onShowMatrixButtonClicked);
    connect(m_showAdjacencyListButton, &QPushButton::clicked, this, &GraphAlgorithmController::onShowAdjacencyListButtonClicked);
    
    // 初始化图操作按钮状态
    updateGraphOperationButtons();
}

void GraphAlgorithmController::setupLayoutPanel()//ok
//三个图布局没实现
//为什么目前的线不直接连接点的位置呢？
{
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(m_controlPanel->layout());
    if (!mainLayout) return;
    
    QGroupBox *layoutGroup = new QGroupBox("布局算法");
    QHBoxLayout *layoutLayout = new QHBoxLayout(layoutGroup);
    
    m_circularLayoutButton = new QPushButton("圆形布局");
    m_forceLayoutButton = new QPushButton("力导向布局");
    m_gridLayoutButton = new QPushButton("网格布局");
    
    m_gridColumnsSpinBox = new QSpinBox();
    m_gridColumnsSpinBox->setRange(1, 10);
    m_gridColumnsSpinBox->setValue(3);
    
    layoutLayout->addWidget(m_circularLayoutButton);
    layoutLayout->addWidget(m_forceLayoutButton);
    layoutLayout->addWidget(m_gridLayoutButton);
    layoutLayout->addWidget(new QLabel("列数:"));
    layoutLayout->addWidget(m_gridColumnsSpinBox);
    
    mainLayout->insertWidget(2, layoutGroup);
    
    // 连接信号
    connect(m_circularLayoutButton, &QPushButton::clicked, this, &GraphAlgorithmController::applyCircularLayout);
    connect(m_forceLayoutButton, &QPushButton::clicked, this, &GraphAlgorithmController::applyForceDirectedLayout);
    connect(m_gridLayoutButton, &QPushButton::clicked, [this]() {
        applyGridLayout(m_gridColumnsSpinBox->value());
    });
}

// 槽函数实现
void GraphAlgorithmController::onAlgorithmStarted()//ok
{
    m_isRunning = true;
    m_isPaused = false;
    updateButtonStates();
    updateGraphOperationButtons();
    //Controller发出信号，传递给Viewer 
    //为了可读性emit开头 功能等同于调用槽函数(不写emit)
    emit algorithmStarted();
}

void GraphAlgorithmController::onAlgorithmStepCompleted()//ok
{
    emit stepCompleted("算法步骤完成");
}

void GraphAlgorithmController::onAlgorithmCompleted()//ok
{
    m_isRunning = false;
    m_isPaused = false;
    updateButtonStates();
    updateGraphOperationButtons();
    logMessage("算法执行完成");
    emit algorithmCompleted();
}

void GraphAlgorithmController::onAlgorithmPaused()//ok
{
    m_isPaused = true;
    updateButtonStates();
    logMessage("算法已暂停");
    emit algorithmPaused();
}

void GraphAlgorithmController::onAlgorithmResumed()//ok
{
    m_isPaused = false;
    updateButtonStates();
    logMessage("算法已继续");
    emit algorithmResumed();
}

void GraphAlgorithmController::onAlgorithmStopped()//ok
{
    m_isRunning = false;
    m_isPaused = false;
    updateButtonStates();
    updateGraphOperationButtons();
    logMessage("算法已停止");
    
    // 注意：不在这里关闭Dijkstra表格对话框
    // 因为算法停止时可能还有信号在队列中，关闭对话框可能导致崩溃
    // 对话框会在切换算法或重新启动Dijkstra算法时自动关闭
    // 用户也可以手动关闭对话框
    
    emit algorithmStopped();
}

void GraphAlgorithmController::onStartButtonClicked()//ok
{
    startAlgorithm();
}

void GraphAlgorithmController::onPauseButtonClicked()//ok
{
    pauseAlgorithm();
}

void GraphAlgorithmController::onResumeButtonClicked()//ok
{
    resumeAlgorithm();
}

void GraphAlgorithmController::onStopButtonClicked()//ok
{
    stopAlgorithm();
}

void GraphAlgorithmController::onStepButtonClicked()//ok
{
    stepAlgorithm();
}

void GraphAlgorithmController::onResetButtonClicked()//ok
{
    resetAlgorithm();
}

void GraphAlgorithmController::updateGraphOperationButtons()
{
    // 根据算法运行状态启用/禁用图操作按钮
    bool enabled = !m_isRunning;
    
    if (m_addVertexButton) {
        m_addVertexButton->setEnabled(enabled);
    }
    if (m_removeVertexButton) {
        m_removeVertexButton->setEnabled(enabled);
    }
    if (m_addEdgeButton) {
        m_addEdgeButton->setEnabled(enabled);
    }
    if (m_removeEdgeButton) {
        m_removeEdgeButton->setEnabled(enabled);
    }
}

void GraphAlgorithmController::onAlgorithmComboBoxChanged(int index)//ok
{
    //消除警告用的，其实没用
    Q_UNUSED(index)
    //这里槽函数的签名声明了多余参数，其实不接受这个多余参数就可以了
    logMessage(QString("选择算法: %1").arg(getCurrentAlgorithmName()));
}

void GraphAlgorithmController::onStartVertexChanged()
{
    // 可以在这里添加起始顶点验证逻辑
}

void GraphAlgorithmController::onDirectedCheckBoxToggled(bool checked)//ok
{
    if (m_graphModel) {
        m_graphModel->setDirected(checked);
        logMessage(QString("图类型: %1").arg(checked ? "有向图" : "无向图"));
    }
}


void GraphAlgorithmController::onLayoutButtonClicked()
{
    // 这个函数可以用于处理布局按钮点击事件
    // 具体实现可以根据需要添加
}

void GraphAlgorithmController::onShowMatrixButtonClicked()
{
    if (!m_graphModel) {
        QMessageBox::warning(nullptr, "错误", "图模型未设置");
        return;
    }
    
    MatrixRepresentationDialog *dialog = new MatrixRepresentationDialog(m_graphModel, nullptr);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void GraphAlgorithmController::onShowAdjacencyListButtonClicked()
{
    if (!m_graphModel) {
        QMessageBox::warning(nullptr, "错误", "图模型未设置");
        return;
    }
    
    AdjacencyListRepresentationDialog *dialog = new AdjacencyListRepresentationDialog(m_graphModel, nullptr);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void GraphAlgorithmController::onDijkstraTableUpdate(const QString &vertex, int distance, const QString &predecessor, bool visited)
{
    // QPointer会自动检查对象是否已被删除，如果已删除则返回nullptr
    if (m_dijkstraTableDialog) {
        m_dijkstraTableDialog->updateVertex(vertex, distance, predecessor, visited);
    }
}

// ==================== SortAlgorithmController 实现 ====================

SortAlgorithmController::SortAlgorithmController(QObject *parent)
    : AlgorithmController(parent)
    , m_sortModel(nullptr)
    , m_algorithmComboBox(nullptr)
    , m_dataSizeSpinBox(nullptr)
    , m_minValueSpinBox(nullptr)
    , m_maxValueSpinBox(nullptr)
    , m_generateDataButton(nullptr)
    , m_clearDataButton(nullptr)
{
    setupControlPanel();
}

QWidget* SortAlgorithmController::createControlPanel()
{
    return m_controlPanel;
}

void SortAlgorithmController::setSortModel(SortModel *model)
{
    if (m_sortModel) {
        disconnect(m_sortModel, nullptr, this, nullptr);
    }
    
    m_sortModel = model;
    
    if (m_sortModel) {
        connect(m_sortModel, &SortModel::algorithmStarted, this, &SortAlgorithmController::onAlgorithmStarted);
        connect(m_sortModel, &SortModel::algorithmStepCompleted, this, &SortAlgorithmController::onAlgorithmStepCompleted);
        connect(m_sortModel, &SortModel::algorithmCompleted, this, &SortAlgorithmController::onAlgorithmCompleted);
        connect(m_sortModel, &SortModel::algorithmPaused, this, &SortAlgorithmController::onAlgorithmPaused);
        connect(m_sortModel, &SortModel::algorithmResumed, this, &SortAlgorithmController::onAlgorithmResumed);
        connect(m_sortModel, &SortModel::algorithmStopped, this, &SortAlgorithmController::onAlgorithmStopped);
    }
}

void SortAlgorithmController::startAlgorithm()
{
    if (!m_sortModel) return;
    
    // 检查数据是否为空
    if (m_sortModel->getData().empty()) {
        logMessage("错误：数据为空，无法执行算法。请先生成或输入数据。");
        return;
    }
    
    int algorithmIndex = m_algorithmComboBox->currentIndex();
    SortAlgorithm algorithm = static_cast<SortAlgorithm>(algorithmIndex);
    
    m_sortModel->executeAlgorithm(algorithm);
    logMessage(QString("开始执行算法: %1").arg(getCurrentAlgorithmName()));
}

void SortAlgorithmController::pauseAlgorithm()
{
    if (m_sortModel) {
        m_sortModel->pauseAlgorithm();
    }
}

void SortAlgorithmController::resumeAlgorithm()
{
    if (m_sortModel) {
        m_sortModel->resumeAlgorithm();
    }
}

void SortAlgorithmController::stopAlgorithm()
{
    if (m_sortModel) {
        m_sortModel->stopAlgorithm();
    }
}

void SortAlgorithmController::stepAlgorithm()
{
    if (m_sortModel) {
        m_sortModel->stepAlgorithm();
    }
}

void SortAlgorithmController::resetAlgorithm()
{
    if (m_sortModel) {
        m_sortModel->resetData();
        logMessage("算法已重置");
    }
}

void SortAlgorithmController::setRandomData(int size, int minValue, int maxValue)
{
    if (!m_sortModel) return;
    
    // 如果算法正在运行，先停止算法
    if (m_isRunning) {
        m_sortModel->stopAlgorithm();
        logMessage("算法已停止，正在生成新数据");
    }
    
    m_sortModel->setRandomData(size, minValue, maxValue);
    logMessage(QString("生成随机数据: %1 个元素，范围 [%2, %3]").arg(size).arg(minValue).arg(maxValue));
}

void SortAlgorithmController::setCustomData(const MyVectorInt &data)
{
    if (!m_sortModel) return;
    
    // 如果算法正在运行，先停止算法
    if (m_isRunning) {
        m_sortModel->stopAlgorithm();
        logMessage("算法已停止，正在设置新数据");
    }
    
    m_sortModel->setData(data);
    logMessage(QString("设置自定义数据: %1 个元素").arg(data.size()));
}

void SortAlgorithmController::clearData()
{
    if (m_sortModel) {
        m_sortModel->clearData();
        logMessage("清空数据");
    }
}

void SortAlgorithmController::setAlgorithm(int algorithmIndex)
{
    if (m_algorithmComboBox) {
        m_algorithmComboBox->setCurrentIndex(algorithmIndex);
    }
}

QString SortAlgorithmController::getCurrentAlgorithmName() const
{
    if (!m_algorithmComboBox) return "";
    
    int index = m_algorithmComboBox->currentIndex();
    switch (index) {
        case 0: return "冒泡排序";
        case 1: return "选择排序";
        case 2: return "插入排序";
        case 3: return "快速排序";
        case 4: return "归并排序";
        default: return "未知算法";
    }
}

void SortAlgorithmController::setupControlPanel()
{
    AlgorithmController::setupControlPanel();
    setupSortControlPanel();
    setupDataGenerationPanel();
    
    // 连接按钮信号
    connect(m_startButton, &QPushButton::clicked, this, &SortAlgorithmController::onStartButtonClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &SortAlgorithmController::onPauseButtonClicked);
    connect(m_resumeButton, &QPushButton::clicked, this, &SortAlgorithmController::onResumeButtonClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &SortAlgorithmController::onStopButtonClicked);
    connect(m_stepButton, &QPushButton::clicked, this, &SortAlgorithmController::onStepButtonClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &SortAlgorithmController::onResetButtonClicked);
}

void SortAlgorithmController::setupSortControlPanel()
{
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(m_controlPanel->layout());
    if (!mainLayout) return;
    
    // 算法选择组
    QGroupBox *algorithmGroup = new QGroupBox("算法选择");
    QVBoxLayout *algorithmLayout = new QVBoxLayout(algorithmGroup);
    
    m_algorithmComboBox = new QComboBox();
    m_algorithmComboBox->addItems({
        "冒泡排序",
        "选择排序",
        "插入排序",
        "快速排序",
        "归并排序"
    });
    
    algorithmLayout->addWidget(new QLabel("排序算法:"));
    algorithmLayout->addWidget(m_algorithmComboBox);
    
    mainLayout->insertWidget(0, algorithmGroup);
    
    // 连接信号
    connect(m_algorithmComboBox, &QComboBox::currentIndexChanged,
            this, &SortAlgorithmController::onAlgorithmComboBoxChanged);
}

void SortAlgorithmController::setupDataGenerationPanel()
{
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(m_controlPanel->layout());
    if (!mainLayout) return;
    
    QGroupBox *dataGroup = new QGroupBox("数据管理");
    QGridLayout *dataLayout = new QGridLayout(dataGroup);
    
    m_dataSizeSpinBox = new QSpinBox();
    m_dataSizeSpinBox->setRange(5, 50);
    m_dataSizeSpinBox->setValue(20);
    
    m_minValueSpinBox = new QSpinBox();
    m_minValueSpinBox->setRange(1, 100);
    m_minValueSpinBox->setValue(1);
    
    m_maxValueSpinBox = new QSpinBox();
    m_maxValueSpinBox->setRange(1, 100);
    m_maxValueSpinBox->setValue(100);
    
    m_generateDataButton = new QPushButton("生成随机数据");
    m_clearDataButton = new QPushButton("清空数据");
    
    dataLayout->addWidget(new QLabel("数据大小:"), 0, 0);
    dataLayout->addWidget(m_dataSizeSpinBox, 0, 1);
    dataLayout->addWidget(new QLabel("最小值:"), 0, 2);
    dataLayout->addWidget(m_minValueSpinBox, 0, 3);
    dataLayout->addWidget(new QLabel("最大值:"), 0, 4);
    dataLayout->addWidget(m_maxValueSpinBox, 0, 5);
    dataLayout->addWidget(m_generateDataButton, 1, 0, 1, 3);
    dataLayout->addWidget(m_clearDataButton, 1, 3, 1, 3);
    
    mainLayout->insertWidget(1, dataGroup);
    
    // 连接信号
    connect(m_generateDataButton, &QPushButton::clicked, this, &SortAlgorithmController::onGenerateDataButtonClicked);
    connect(m_clearDataButton, &QPushButton::clicked, this, &SortAlgorithmController::onClearDataButtonClicked);
}

// 槽函数实现
void SortAlgorithmController::onAlgorithmStarted()
{
    m_isRunning = true;
    m_isPaused = false;
    updateButtonStates();
    emit algorithmStarted();
}

void SortAlgorithmController::onAlgorithmStepCompleted()
{
    emit stepCompleted("排序步骤完成");
}

void SortAlgorithmController::onAlgorithmCompleted()
{
    m_isRunning = false;
    m_isPaused = false;
    updateButtonStates();
    logMessage("排序算法执行完成");
    emit algorithmCompleted();
}

void SortAlgorithmController::onAlgorithmPaused()
{
    m_isPaused = true;
    updateButtonStates();
    logMessage("排序算法已暂停");
    emit algorithmPaused();
}

void SortAlgorithmController::onAlgorithmResumed()
{
    m_isPaused = false;
    updateButtonStates();
    logMessage("排序算法已继续");
    emit algorithmResumed();
}

void SortAlgorithmController::onAlgorithmStopped()
{
    m_isRunning = false;
    m_isPaused = false;
    updateButtonStates();
    logMessage("排序算法已停止");
    emit algorithmStopped();
}

void SortAlgorithmController::onStartButtonClicked()
{
    startAlgorithm();
}

void SortAlgorithmController::onPauseButtonClicked()
{
    pauseAlgorithm();
}

void SortAlgorithmController::onResumeButtonClicked()
{
    resumeAlgorithm();
}

void SortAlgorithmController::onStopButtonClicked()
{
    stopAlgorithm();
}

void SortAlgorithmController::onStepButtonClicked()
{
    stepAlgorithm();
}

void SortAlgorithmController::onResetButtonClicked()
{
    resetAlgorithm();
}

void SortAlgorithmController::onAlgorithmComboBoxChanged(int index)
{
    Q_UNUSED(index)
    logMessage(QString("选择算法: %1").arg(getCurrentAlgorithmName()));
}

void SortAlgorithmController::onDataSizeChanged(int size)
{
    Q_UNUSED(size)
    // 可以在这里添加数据大小变化处理逻辑
}

void SortAlgorithmController::onMinValueChanged(int value)
{
    Q_UNUSED(value)
    // 可以在这里添加最小值变化处理逻辑
}

void SortAlgorithmController::onMaxValueChanged(int value)
{
    Q_UNUSED(value)
    // 可以在这里添加最大值变化处理逻辑
}

void SortAlgorithmController::onGenerateDataButtonClicked()
{
    int size = m_dataSizeSpinBox->value();
    int minValue = m_minValueSpinBox->value();
    int maxValue = m_maxValueSpinBox->value();
    
    setRandomData(size, minValue, maxValue);
}

void SortAlgorithmController::onClearDataButtonClicked()
{
    // 如果算法正在运行，先停止算法
    if (m_isRunning && m_sortModel) {
        m_sortModel->stopAlgorithm();
        logMessage("算法已停止，正在清空数据");
    }
    
    clearData();
}

