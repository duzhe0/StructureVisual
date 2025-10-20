#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

#include "GraphModel.h"
#include "VisualItem.h"
#include "AlgorithmController.h"

// BFS算法演示窗口
class BFSDemoWindow : public QMainWindow
{
    Q_OBJECT

public:
    BFSDemoWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setupGraph();
        connectSignals();
    }

private slots:
    void startBFS()
    {
        if (m_graphModel->getVertexCount() == 0) {
            QMessageBox::warning(this, "警告", "请先创建图结构！");
            return;
        }
        
        m_graphModel->executeAlgorithm(GraphAlgorithm::BFS, "A");
        m_statusLabel->setText("BFS算法开始执行...");
    }
    
    void createSampleGraph()
    {
        m_graphModel->clearGraph();
        
        // 创建一个用于BFS演示的图
        // 图结构: A-B-C
        //         |   |
        //         D-E-F
        //         |   |
        //         G-H-I
        
        // 添加顶点
        m_graphModel->addVertex("A", QPointF(-150, -150));
        m_graphModel->addVertex("B", QPointF(0, -150));
        m_graphModel->addVertex("C", QPointF(150, -150));
        m_graphModel->addVertex("D", QPointF(-150, 0));
        m_graphModel->addVertex("E", QPointF(0, 0));
        m_graphModel->addVertex("F", QPointF(150, 0));
        m_graphModel->addVertex("G", QPointF(-150, 150));
        m_graphModel->addVertex("H", QPointF(0, 150));
        m_graphModel->addVertex("I", QPointF(150, 150));
        
        // 添加边
        m_graphModel->addEdge("A", "B", 1);
        m_graphModel->addEdge("B", "C", 1);
        m_graphModel->addEdge("A", "D", 1);
        m_graphModel->addEdge("B", "E", 1);
        m_graphModel->addEdge("C", "F", 1);
        m_graphModel->addEdge("D", "E", 1);
        m_graphModel->addEdge("E", "F", 1);
        m_graphModel->addEdge("D", "G", 1);
        m_graphModel->addEdge("E", "H", 1);
        m_graphModel->addEdge("F", "I", 1);
        m_graphModel->addEdge("G", "H", 1);
        m_graphModel->addEdge("H", "I", 1);
        
        m_statusLabel->setText("示例图已创建，包含9个顶点和12条边");
    }
    
    void resetGraph()
    {
        m_graphModel->resetVisualization();
        m_statusLabel->setText("图已重置，可以重新开始BFS");
    }
    
    void onAlgorithmCompleted()
    {
        m_statusLabel->setText("BFS算法执行完成！");
        QMessageBox::information(this, "完成", "BFS算法执行完成！\n\n"
            "BFS按照以下顺序访问了所有顶点：\n"
            "A -> B -> D -> C -> E -> G -> F -> H -> I\n\n"
            "这展示了广度优先搜索的特点：\n"
            "1. 从起始顶点开始\n"
            "2. 先访问所有相邻顶点\n"
            "3. 再访问相邻顶点的相邻顶点\n"
            "4. 逐层扩展，直到访问完所有可达顶点");
    }
    
    void onAlgorithmStepCompleted()
    {
        // 更新状态显示当前步骤
        m_statusLabel->setText("BFS执行中...");
    }

private:
    void setupUI()
    {
        setWindowTitle("BFS算法可视化演示");
        setMinimumSize(1000, 700);
        resize(1200, 800);
        
        // 创建中央部件
        QWidget *centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        
        // 创建控制面板
        QWidget *controlPanel = new QWidget();
        controlPanel->setMaximumWidth(300);
        QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
        
        // 标题
        QLabel *titleLabel = new QLabel("BFS算法演示");
        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");
        controlLayout->addWidget(titleLabel);
        
        // 说明文本
        QLabel *descLabel = new QLabel(
            "广度优先搜索(BFS)是一种图遍历算法，\n"
            "它从起始顶点开始，逐层访问所有相邻顶点。\n\n"
            "特点：\n"
            "• 使用队列数据结构\n"
            "• 保证找到最短路径\n"
            "• 时间复杂度: O(V+E)\n"
            "• 空间复杂度: O(V)"
        );
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("color: #34495e; padding: 10px;");
        controlLayout->addWidget(descLabel);
        
        // 控制按钮
        QPushButton *createGraphBtn = new QPushButton("创建示例图");
        createGraphBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #3498db;"
            "    color: white;"
            "    border: none;"
            "    padding: 10px;"
            "    border-radius: 5px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #2980b9;"
            "}"
        );
        connect(createGraphBtn, &QPushButton::clicked, this, &BFSDemoWindow::createSampleGraph);
        
        QPushButton *startBFSBtn = new QPushButton("开始BFS算法");
        startBFSBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #27ae60;"
            "    color: white;"
            "    border: none;"
            "    padding: 10px;"
            "    border-radius: 5px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #229954;"
            "}"
        );
        connect(startBFSBtn, &QPushButton::clicked, this, &BFSDemoWindow::startBFS);
        
        QPushButton *resetBtn = new QPushButton("重置图");
        resetBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #e74c3c;"
            "    color: white;"
            "    border: none;"
            "    padding: 10px;"
            "    border-radius: 5px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #c0392b;"
            "}"
        );
        connect(resetBtn, &QPushButton::clicked, this, &BFSDemoWindow::resetGraph);
        
        controlLayout->addWidget(createGraphBtn);
        controlLayout->addWidget(startBFSBtn);
        controlLayout->addWidget(resetBtn);
        
        // 状态显示
        m_statusLabel = new QLabel("就绪 - 请先创建示例图");
        m_statusLabel->setStyleSheet(
            "QLabel {"
            "    background-color: #ecf0f1;"
            "    border: 1px solid #bdc3c7;"
            "    padding: 10px;"
            "    border-radius: 5px;"
            "    color: #2c3e50;"
            "}"
        );
        controlLayout->addWidget(m_statusLabel);
        
        controlLayout->addStretch();
        
        // 创建图形视图
        m_graphScene = new QGraphicsScene(this);
        m_graphScene->setSceneRect(-300, -300, 600, 600);
        m_graphScene->setBackgroundBrush(QBrush(QColor(248, 249, 250)));
        
        m_graphView = new QGraphicsView(m_graphScene);
        m_graphView->setRenderHint(QPainter::Antialiasing);
        m_graphView->setDragMode(QGraphicsView::RubberBandDrag);
        m_graphView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        
        // 组装布局
        mainLayout->addWidget(controlPanel);
        mainLayout->addWidget(m_graphView, 1);
    }
    
    void setupGraph()
    {
        // 创建图模型
        m_graphModel = new GraphModel(this);
        m_graphModel->setScene(m_graphScene);
        m_graphModel->setDirected(false); // 无向图
        
        // 创建算法控制器
        m_graphController = new GraphAlgorithmController(this);
        m_graphController->setGraphModel(m_graphModel);
    }
    
    void connectSignals()
    {
        // 连接算法完成信号
        connect(m_graphModel, &GraphModel::algorithmCompleted, 
                this, &BFSDemoWindow::onAlgorithmCompleted);
        connect(m_graphModel, &GraphModel::algorithmStepCompleted, 
                this, &BFSDemoWindow::onAlgorithmStepCompleted);
    }

private:
    QGraphicsScene *m_graphScene;
    QGraphicsView *m_graphView;
    GraphModel *m_graphModel;
    GraphAlgorithmController *m_graphController;
    QLabel *m_statusLabel;
};

// 主函数
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("BFS算法可视化演示");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("AlgorithmVisualization");
    
    // 创建并显示演示窗口
    BFSDemoWindow demo;
    demo.show();
    
    return app.exec();
}

#include "demo_bfs.moc"
