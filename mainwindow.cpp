#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GraphModel.h"
#include "SortModel.h"
#include "AlgorithmController.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_mainSplitter(nullptr)
    , m_modeTabWidget(nullptr)
    , m_visualizationStack(nullptr)
    , m_graphView(nullptr)
    , m_graphScene(nullptr)
    , m_graphModel(nullptr)
    , m_graphController(nullptr)
    , m_sortView(nullptr)
    , m_sortScene(nullptr)
    , m_sortModel(nullptr)
    , m_sortController(nullptr)
    , m_statusLabel(nullptr)
    , m_algorithmLabel(nullptr)
    , m_progressBar(nullptr)
    , m_statusTimer(nullptr)
    , m_fileMenu(nullptr)
    , m_editMenu(nullptr)
    , m_viewMenu(nullptr)
    , m_algorithmMenu(nullptr)
    , m_helpMenu(nullptr)
    , m_currentMode(GraphMode)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    setWindowTitle("数据结构与算法可视化模拟器");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // 初始化组件
    initializeModels();
    setupUI();
    setupMenuBar();
    setupStatusBar();
    connectSignals();
    
    // 默认显示图模式
    switchToGraphMode();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 创建主分割器
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(m_mainSplitter);
    
    // 创建模式标签页
    m_modeTabWidget = new QTabWidget();
    m_modeTabWidget->addTab(new QWidget(), "图算法");
    m_modeTabWidget->addTab(new QWidget(), "排序算法");
    
    // 创建可视化堆栈
    m_visualizationStack = new QStackedWidget();
    
    // 设置图可视化
    setupGraphVisualization();
    
    // 设置排序可视化
    setupSortVisualization();
    
    // 组装布局
    m_mainSplitter->addWidget(m_modeTabWidget);
    m_mainSplitter->addWidget(m_visualizationStack);
    m_mainSplitter->setStretchFactor(0, 0); // 控制面板固定宽度
    m_mainSplitter->setStretchFactor(1, 1); // 可视化区域可伸缩
    
    // 连接标签页切换信号
    connect(m_modeTabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        if (index == 0) {
            switchToGraphMode();
        } else if (index == 1) {
            switchToSortMode();
        }
    });
}

void MainWindow::setupMenuBar()
{
    // 文件菜单
    m_fileMenu = menuBar()->addMenu("文件(&F)");
    
    QAction *newAction = m_fileMenu->addAction("新建(&N)");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, [this]() {
        if (m_currentMode == GraphMode) {
            m_graphModel->clearGraph();
        } else {
            m_sortModel->clearData();
        }
    });
    
    QAction *openAction = m_fileMenu->addAction("打开(&O)");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "打开图文件", "", "JSON文件 (*.json);;所有文件 (*.*)");
        if (!fileName.isEmpty()) {
            if (m_currentMode == GraphMode && m_graphModel) {
                if (m_graphModel->loadFromFile(fileName)) {
                    m_statusLabel->setText(QString("已从文件加载图: %1").arg(fileName));
                    QMessageBox::information(this, "成功", QString("图文件加载成功！\n\n文件: %1\n顶点数: %2\n边数: %3")
                                           .arg(fileName)
                                           .arg(m_graphModel->getVertexCount())
                                           .arg(m_graphModel->getEdgeCount()));
                } else {
                    QMessageBox::warning(this, "错误", "文件加载失败，请检查文件格式是否正确。");
                }
            } else {
                QMessageBox::information(this, "提示", "请在图算法模式下使用文件加载功能。");
            }
        }
    });
    
    QAction *saveAction = m_fileMenu->addAction("保存(&S)");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "保存图文件", "", "JSON文件 (*.json);;所有文件 (*.*)");
        if (!fileName.isEmpty()) {
            if (m_currentMode == GraphMode && m_graphModel) {
                if (m_graphModel->saveToFile(fileName)) {
                    m_statusLabel->setText(QString("图已保存到文件: %1").arg(fileName));
                    QMessageBox::information(this, "成功", QString("图文件保存成功！\n\n文件: %1\n顶点数: %2\n边数: %3")
                                           .arg(fileName)
                                           .arg(m_graphModel->getVertexCount())
                                           .arg(m_graphModel->getEdgeCount()));
                } else {
                    QMessageBox::warning(this, "错误", "文件保存失败，请检查文件路径和权限。");
                }
            } else {
                QMessageBox::information(this, "提示", "请在图算法模式下使用文件保存功能。");
            }
        }
    });
    
    m_fileMenu->addSeparator();
    
    QAction *exitAction = m_fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // 编辑菜单
    m_editMenu = menuBar()->addMenu("编辑(&E)");
    
    QAction *undoAction = m_editMenu->addAction("撤销(&U)");
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setEnabled(false); // 暂时禁用
    
    QAction *redoAction = m_editMenu->addAction("重做(&R)");
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setEnabled(false); // 暂时禁用
    
    // 视图菜单
    m_viewMenu = menuBar()->addMenu("视图(&V)");
    
    QAction *zoomInAction = m_viewMenu->addAction("放大(&I)");
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, this, [this]() {
        if (m_currentMode == GraphMode && m_graphView) {
            m_graphView->scale(1.2, 1.2);
        } else if (m_currentMode == SortMode && m_sortView) {
            m_sortView->scale(1.2, 1.2);
        }
    });
    
    QAction *zoomOutAction = m_viewMenu->addAction("缩小(&O)");
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, this, [this]() {
        if (m_currentMode == GraphMode && m_graphView) {
            m_graphView->scale(0.8, 0.8);
        } else if (m_currentMode == SortMode && m_sortView) {
            m_sortView->scale(0.8, 0.8);
        }
    });
    
    QAction *fitInViewAction = m_viewMenu->addAction("适应窗口(&F)");
    fitInViewAction->setShortcut(QKeySequence("Ctrl+0"));
    connect(fitInViewAction, &QAction::triggered, this, [this]() {
        if (m_currentMode == GraphMode && m_graphView) {
            m_graphView->fitInView(m_graphScene->itemsBoundingRect(), Qt::KeepAspectRatio);
        } else if (m_currentMode == SortMode && m_sortView) {
            m_sortView->fitInView(m_sortScene->itemsBoundingRect(), Qt::KeepAspectRatio);
        }
    });
    
    // 算法菜单
    m_algorithmMenu = menuBar()->addMenu("算法(&A)");
    
    QAction *startAction = m_algorithmMenu->addAction("开始算法(&S)");
    startAction->setShortcut(QKeySequence("F5"));
    connect(startAction, &QAction::triggered, this, [this]() {
        if (m_currentMode == GraphMode && m_graphController) {
            m_graphController->startAlgorithm();
        } else if (m_currentMode == SortMode && m_sortController) {
            m_sortController->startAlgorithm();
        }
    });
    
    QAction *pauseAction = m_algorithmMenu->addAction("暂停算法(&P)");
    pauseAction->setShortcut(QKeySequence("F6"));
    connect(pauseAction, &QAction::triggered, this, [this]() {
        if (m_currentMode == GraphMode && m_graphController) {
            m_graphController->pauseAlgorithm();
        } else if (m_currentMode == SortMode && m_sortController) {
            m_sortController->pauseAlgorithm();
        }
    });
    
    QAction *stopAction = m_algorithmMenu->addAction("停止算法(&T)");
    stopAction->setShortcut(QKeySequence("F7"));
    connect(stopAction, &QAction::triggered, this, [this]() {
        if (m_currentMode == GraphMode && m_graphController) {
            m_graphController->stopAlgorithm();
        } else if (m_currentMode == SortMode && m_sortController) {
            m_sortController->stopAlgorithm();
        }
    });
    
    // 帮助菜单
    m_helpMenu = menuBar()->addMenu("帮助(&H)");
    
    QAction *aboutAction = m_helpMenu->addAction("关于(&A)");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "关于", 
            "数据结构与算法可视化模拟器 v1.0\n\n"
            "基于Qt 6框架开发\n"
            "支持图算法和排序算法的可视化演示\n\n"
            "开发团队: AI Assistant");
    });
}

void MainWindow::setupStatusBar()
{
    // 状态标签
    m_statusLabel = new QLabel("就绪");
    statusBar()->addWidget(m_statusLabel);
    
    // 算法标签
    m_algorithmLabel = new QLabel("当前算法: 无");
    statusBar()->addPermanentWidget(m_algorithmLabel);
    
    // 进度条
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    statusBar()->addPermanentWidget(m_progressBar);
    
    // 状态更新定时器
    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    m_statusTimer->start(1000); // 每秒更新一次
}

void MainWindow::setupCentralWidget()
{
    // 这个方法在setupUI中已经实现
}

void MainWindow::setupGraphVisualization()
{
    // 创建图场景和视图
    m_graphScene = new QGraphicsScene(this);
    m_graphScene->setSceneRect(-500, -500, 1000, 1000);
    m_graphScene->setBackgroundBrush(QBrush(QColor(240, 240, 240)));
    
    m_graphView = new QGraphicsView(m_graphScene);
    m_graphView->setRenderHint(QPainter::Antialiasing);
    m_graphView->setDragMode(QGraphicsView::RubberBandDrag);
    m_graphView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    // 设置图模型的场景
    m_graphModel->setScene(m_graphScene);
    
    // 创建图控制器
    m_graphController = new GraphAlgorithmController(this);
    m_graphController->setGraphModel(m_graphModel);
    
    // 创建图模式的控制面板
    QWidget *graphControlWidget = new QWidget();
    QVBoxLayout *graphControlLayout = new QVBoxLayout(graphControlWidget);
    
    // 添加图控制器面板
    QWidget *graphControlPanel = m_graphController->createControlPanel();
    graphControlLayout->addWidget(graphControlPanel);
    
    // 添加一些示例图操作按钮
    QGroupBox *exampleGroup = new QGroupBox("示例图");
    QVBoxLayout *exampleLayout = new QVBoxLayout(exampleGroup);
    
    QPushButton *createSampleGraphBtn = new QPushButton("创建示例图");
    connect(createSampleGraphBtn, &QPushButton::clicked, this, [this]() {
        m_graphModel->clearGraph();
        
        // 创建一个简单的示例图
        m_graphModel->addVertex("A", QPointF(-100, -100));
        m_graphModel->addVertex("B", QPointF(100, -100));
        m_graphModel->addVertex("C", QPointF(100, 100));
        m_graphModel->addVertex("D", QPointF(-100, 100));
        m_graphModel->addVertex("E", QPointF(0, 0));
        
        m_graphModel->addEdge("A", "B", 5);
        m_graphModel->addEdge("B", "C", 3);
        m_graphModel->addEdge("C", "D", 4);
        m_graphModel->addEdge("D", "A", 2);
        m_graphModel->addEdge("A", "E", 1);
        m_graphModel->addEdge("B", "E", 2);
        m_graphModel->addEdge("C", "E", 3);
        m_graphModel->addEdge("D", "E", 1);
        
        m_statusLabel->setText("示例图已创建");
    });
    
    QPushButton *clearGraphBtn = new QPushButton("清空图");
    connect(clearGraphBtn, &QPushButton::clicked, this, [this]() {
        m_graphModel->clearGraph();
        m_statusLabel->setText("图已清空");
    });
    
    exampleLayout->addWidget(createSampleGraphBtn);
    exampleLayout->addWidget(clearGraphBtn);
    
    graphControlLayout->addWidget(exampleGroup);
    graphControlLayout->addStretch();
    
    // 设置图模式标签页
    m_modeTabWidget->widget(0)->setLayout(new QVBoxLayout());
    m_modeTabWidget->widget(0)->layout()->addWidget(graphControlWidget);
    
    // 添加图视图到可视化堆栈
    m_visualizationStack->addWidget(m_graphView);
}

void MainWindow::setupSortVisualization()
{
    // 创建排序场景和视图
    m_sortScene = new QGraphicsScene(this);
    m_sortScene->setSceneRect(-200, -200, 800, 400);
    m_sortScene->setBackgroundBrush(QBrush(QColor(240, 240, 240)));
    
    m_sortView = new QGraphicsView(m_sortScene);
    m_sortView->setRenderHint(QPainter::Antialiasing);
    m_sortView->setDragMode(QGraphicsView::NoDrag);
    m_sortView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    // 设置排序模型的场景
    m_sortModel->setScene(m_sortScene);
    
    // 创建排序控制器
    m_sortController = new SortAlgorithmController(this);
    m_sortController->setSortModel(m_sortModel);
    
    // 创建排序模式的控制面板
    QWidget *sortControlWidget = new QWidget();
    QVBoxLayout *sortControlLayout = new QVBoxLayout(sortControlWidget);
    
    // 添加排序控制器面板
    QWidget *sortControlPanel = m_sortController->createControlPanel();
    sortControlLayout->addWidget(sortControlPanel);
    
    // 添加一些示例数据按钮
    QGroupBox *exampleGroup = new QGroupBox("示例数据");
    QVBoxLayout *exampleLayout = new QVBoxLayout(exampleGroup);
    
    QPushButton *createSampleDataBtn = new QPushButton("生成随机数据");
    connect(createSampleDataBtn, &QPushButton::clicked, this, [this]() {
        m_sortModel->setRandomData(20, 1, 100);
        m_statusLabel->setText("随机数据已生成");
    });
    
    QPushButton *clearDataBtn = new QPushButton("清空数据");
    connect(clearDataBtn, &QPushButton::clicked, this, [this]() {
        m_sortModel->clearData();
        m_statusLabel->setText("数据已清空");
    });
    
    exampleLayout->addWidget(createSampleDataBtn);
    exampleLayout->addWidget(clearDataBtn);
    
    sortControlLayout->addWidget(exampleGroup);
    sortControlLayout->addStretch();
    
    // 设置排序模式标签页
    m_modeTabWidget->widget(1)->setLayout(new QVBoxLayout());
    m_modeTabWidget->widget(1)->layout()->addWidget(sortControlWidget);
    
    // 添加排序视图到可视化堆栈
    m_visualizationStack->addWidget(m_sortView);
}

void MainWindow::connectSignals()
{
    // 连接原有的按钮信号
    connect(ui->GraphPB, &QPushButton::clicked, this, &MainWindow::handleGraphPB);
    connect(ui->SortPB, &QPushButton::clicked, this, &MainWindow::handleSortPB);
    
    // 连接算法控制器信号
    if (m_graphController) {
        connect(m_graphController, &GraphAlgorithmController::algorithmStarted, this, &MainWindow::onAlgorithmStarted);
        connect(m_graphController, &GraphAlgorithmController::algorithmCompleted, this, &MainWindow::onAlgorithmCompleted);
        connect(m_graphController, &GraphAlgorithmController::stepCompleted, this, &MainWindow::onAlgorithmStepCompleted);
    }
    
    if (m_sortController) {
        connect(m_sortController, &SortAlgorithmController::algorithmStarted, this, &MainWindow::onAlgorithmStarted);
        connect(m_sortController, &SortAlgorithmController::algorithmCompleted, this, &MainWindow::onAlgorithmCompleted);
        connect(m_sortController, &SortAlgorithmController::stepCompleted, this, &MainWindow::onAlgorithmStepCompleted);
    }
}

void MainWindow::initializeModels()
{
    // 创建图模型
    m_graphModel = new GraphModel(this);
    
    // 创建排序模型
    m_sortModel = new SortModel(this);
}

void MainWindow::handleGraphPB()
{
    switchToGraphMode();
    emit doNow();
}

void MainWindow::handleSortPB()
{
    switchToSortMode();
    emit doNow();
}

void MainWindow::switchToGraphMode()
{
    m_currentMode = GraphMode;
    m_modeTabWidget->setCurrentIndex(0);
    m_visualizationStack->setCurrentWidget(m_graphView);
    m_algorithmLabel->setText("当前模式: 图算法");
    m_statusLabel->setText("图算法模式");
}

void MainWindow::switchToSortMode()
{
    m_currentMode = SortMode;
    m_modeTabWidget->setCurrentIndex(1);
    m_visualizationStack->setCurrentWidget(m_sortView);
    m_algorithmLabel->setText("当前模式: 排序算法");
    m_statusLabel->setText("排序算法模式");
}

void MainWindow::onAlgorithmStarted()
{
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_statusLabel->setText("算法执行中...");
}

void MainWindow::onAlgorithmCompleted()
{
    m_progressBar->setVisible(false);
    m_statusLabel->setText("算法执行完成");
}

void MainWindow::onAlgorithmStepCompleted(const QString &description)
{
    m_statusLabel->setText(description);
}

void MainWindow::updateStatusBar()
{
    // 更新状态栏信息
    if (m_currentMode == GraphMode) {
        if (m_graphModel) {
            int vertexCount = m_graphModel->getVertexCount();
            int edgeCount = m_graphModel->getEdgeCount();
            QString status = QString("顶点: %1, 边: %2").arg(vertexCount).arg(edgeCount);
            if (m_graphModel->isAlgorithmRunning()) {
                status += " | 算法执行中";
            }
            m_statusLabel->setText(status);
        }
    } else if (m_currentMode == SortMode) {
        if (m_sortModel) {
            auto data = m_sortModel->getData();
            QString status = QString("数据元素: %1").arg(data.size());
            if (m_sortModel->isAlgorithmRunning()) {
                status += " | 算法执行中";
            }
            m_statusLabel->setText(status);
        }
    }
}