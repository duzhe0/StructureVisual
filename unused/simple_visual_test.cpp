#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QMessageBox>
#include <QDebug>

// 简化的可视化测试程序
class SimpleVisualTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimpleVisualTestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setupScene();
    }

private slots:
    void testGraphCreation()
    {
        // 创建简单的图结构
        m_scene->clear();
        setupScene();
        
        // 添加顶点
        QGraphicsEllipseItem *vertexA = new QGraphicsEllipseItem(-25, -25, 50, 50);
        vertexA->setPos(-100, 0);
        vertexA->setBrush(QColor(100, 150, 255));
        vertexA->setPen(QPen(QColor(0, 0, 0), 2));
        m_scene->addItem(vertexA);
        
        QGraphicsEllipseItem *vertexB = new QGraphicsEllipseItem(-25, -25, 50, 50);
        vertexB->setPos(0, 0);
        vertexB->setBrush(QColor(100, 150, 255));
        vertexB->setPen(QPen(QColor(0, 0, 0), 2));
        m_scene->addItem(vertexB);
        
        QGraphicsEllipseItem *vertexC = new QGraphicsEllipseItem(-25, -25, 50, 50);
        vertexC->setPos(100, 0);
        vertexC->setBrush(QColor(100, 150, 255));
        vertexC->setPen(QPen(QColor(0, 0, 0), 2));
        m_scene->addItem(vertexC);
        
        // 添加标签
        QGraphicsSimpleTextItem *labelA = new QGraphicsSimpleTextItem("A");
        labelA->setPos(-100 - 10, -10);
        m_scene->addItem(labelA);
        
        QGraphicsSimpleTextItem *labelB = new QGraphicsSimpleTextItem("B");
        labelB->setPos(-10, -10);
        m_scene->addItem(labelB);
        
        QGraphicsSimpleTextItem *labelC = new QGraphicsSimpleTextItem("C");
        labelC->setPos(100 - 10, -10);
        m_scene->addItem(labelC);
        
        // 添加边
        QGraphicsLineItem *edgeAB = new QGraphicsLineItem(-75, 0, -25, 0);
        edgeAB->setPen(QPen(QColor(0, 0, 0), 2));
        m_scene->addItem(edgeAB);
        
        QGraphicsLineItem *edgeBC = new QGraphicsLineItem(25, 0, 75, 0);
        edgeBC->setPen(QPen(QColor(0, 0, 0), 2));
        m_scene->addItem(edgeBC);
        
        m_statusLabel->setText("简单图结构已创建：A-B-C");
    }
    
    void testSortVisualization()
    {
        m_scene->clear();
        setupScene();
        
        // 创建简单的排序可视化
        std::vector<int> values = {30, 60, 20, 80, 40};
        std::vector<QColor> colors = {
            QColor(255, 100, 100),
            QColor(100, 255, 100),
            QColor(100, 100, 255),
            QColor(255, 255, 100),
            QColor(255, 100, 255)
        };
        
        for (size_t i = 0; i < values.size(); ++i) {
            int height = values[i] * 2; // 放大显示
            QGraphicsRectItem *bar = new QGraphicsRectItem(-20, -height, 40, height);
            bar->setPos(i * 80 - 160, 0);
            bar->setBrush(colors[i]);
            bar->setPen(QPen(QColor(0, 0, 0), 1));
            m_scene->addItem(bar);
            
            // 添加数值标签
            QGraphicsSimpleTextItem *label = new QGraphicsSimpleTextItem(QString::number(values[i]));
            label->setPos(i * 80 - 160 - 10, -height - 20);
            m_scene->addItem(label);
        }
        
        m_statusLabel->setText("排序可视化已创建：柱状图显示");
    }
    
    void clearScene()
    {
        m_scene->clear();
        setupScene();
        m_statusLabel->setText("场景已清空");
    }

private:
    void setupUI()
    {
        setWindowTitle("简化可视化测试");
        setMinimumSize(800, 600);
        
        QWidget *centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        
        // 左侧控制面板
        QWidget *controlPanel = new QWidget();
        controlPanel->setMaximumWidth(200);
        QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
        
        QLabel *titleLabel = new QLabel("测试控制");
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
        controlLayout->addWidget(titleLabel);
        
        QPushButton *graphBtn = new QPushButton("测试图可视化");
        graphBtn->setStyleSheet(
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
        
        QPushButton *sortBtn = new QPushButton("测试排序可视化");
        sortBtn->setStyleSheet(
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
        
        QPushButton *clearBtn = new QPushButton("清空场景");
        clearBtn->setStyleSheet(
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
        
        controlLayout->addWidget(graphBtn);
        controlLayout->addWidget(sortBtn);
        controlLayout->addWidget(clearBtn);
        controlLayout->addStretch();
        
        // 右侧图形视图
        m_scene = new QGraphicsScene(this);
        m_scene->setSceneRect(-300, -200, 600, 400);
        m_scene->setBackgroundBrush(QBrush(QColor(248, 249, 250)));
        
        m_view = new QGraphicsView(m_scene);
        m_view->setRenderHint(QPainter::Antialiasing);
        m_view->setDragMode(QGraphicsView::RubberBandDrag);
        
        // 状态标签
        m_statusLabel = new QLabel("就绪");
        m_statusLabel->setStyleSheet(
            "QLabel {"
            "    background-color: #ecf0f1;"
            "    border: 1px solid #bdc3c7;"
            "    padding: 10px;"
            "    border-radius: 5px;"
            "    color: #2c3e50;"
            "}"
        );
        
        QVBoxLayout *rightLayout = new QVBoxLayout();
        rightLayout->addWidget(m_view);
        rightLayout->addWidget(m_statusLabel);
        
        mainLayout->addWidget(controlPanel);
        mainLayout->addLayout(rightLayout, 1);
        
        // 连接信号
        connect(graphBtn, &QPushButton::clicked, this, &SimpleVisualTestWindow::testGraphCreation);
        connect(sortBtn, &QPushButton::clicked, this, &SimpleVisualTestWindow::testSortVisualization);
        connect(clearBtn, &QPushButton::clicked, this, &SimpleVisualTestWindow::clearScene);
    }
    
    void setupScene()
    {
        // 添加坐标轴
        QGraphicsLineItem *xAxis = new QGraphicsLineItem(-250, 0, 250, 0);
        xAxis->setPen(QPen(QColor(200, 200, 200), 1));
        m_scene->addItem(xAxis);
        
        QGraphicsLineItem *yAxis = new QGraphicsLineItem(0, -150, 0, 150);
        yAxis->setPen(QPen(QColor(200, 200, 200), 1));
        m_scene->addItem(yAxis);
        
        // 添加标题
        QGraphicsSimpleTextItem *title = new QGraphicsSimpleTextItem("可视化测试区域");
        title->setPos(-80, -180);
        title->setFont(QFont("Arial", 12, QFont::Bold));
        m_scene->addItem(title);
    }

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QLabel *m_statusLabel;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("简化可视化测试");
    app.setApplicationVersion("1.0");
    
    SimpleVisualTestWindow window;
    window.show();
    
    qDebug() << "简化可视化测试程序启动成功";
    
    return app.exec();
}

#include "simple_visual_test.moc"
