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
#include <QMessageBox>
#include <QDebug>

// 简化的测试程序，验证Qt环境
class SimpleTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimpleTestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setupUI();
        setupScene();
        connectSignals();
    }

private slots:
    void testButtonClicked()
    {
        QMessageBox::information(this, "测试", "Qt环境工作正常！\n\n"
            "这是一个简化的测试程序，验证：\n"
            "✓ Qt 6.10.0 MinGW 64-bit 环境\n"
            "✓ 基本UI组件\n"
            "✓ 图形视图框架\n"
            "✓ 信号槽机制");
    }
    
    void addVertex()
    {
        static int count = 0;
        count++;
        
        QGraphicsEllipseItem *vertex = new QGraphicsEllipseItem(-25, -25, 50, 50);
        vertex->setPos(count * 100 - 200, 0);
        vertex->setBrush(QColor(0, 0, 255)); // blue
        vertex->setPen(QPen(QColor(0, 0, 0), 2)); // black
        
        m_scene->addItem(vertex);
        
        QGraphicsSimpleTextItem *label = new QGraphicsSimpleTextItem(QString::number(count));
        label->setPos(count * 100 - 200 - 10, -10);
        m_scene->addItem(label);
        
        m_statusLabel->setText(QString("已添加顶点 %1").arg(count));
    }
    
    void clearScene()
    {
        m_scene->clear();
        m_statusLabel->setText("场景已清空");
    }

private:
    void setupUI()
    {
        setWindowTitle("Qt环境测试程序");
        setMinimumSize(800, 600);
        
        QWidget *centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        
        // 左侧控制面板
        QWidget *controlPanel = new QWidget();
        controlPanel->setMaximumWidth(200);
        QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
        
        QLabel *titleLabel = new QLabel("控制面板");
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2c3e50;");
        controlLayout->addWidget(titleLabel);
        
        QPushButton *testBtn = new QPushButton("测试Qt环境");
        testBtn->setStyleSheet(
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
        
        QPushButton *addVertexBtn = new QPushButton("添加顶点");
        addVertexBtn->setStyleSheet(
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
        
        controlLayout->addWidget(testBtn);
        controlLayout->addWidget(addVertexBtn);
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
        connect(testBtn, &QPushButton::clicked, this, &SimpleTestWindow::testButtonClicked);
        connect(addVertexBtn, &QPushButton::clicked, this, &SimpleTestWindow::addVertex);
        connect(clearBtn, &QPushButton::clicked, this, &SimpleTestWindow::clearScene);
    }
    
    void setupScene()
    {
        // 添加一些示例图形
        QGraphicsEllipseItem *centerCircle = new QGraphicsEllipseItem(-50, -50, 100, 100);
        centerCircle->setPos(0, 0);
        centerCircle->setBrush(QColor(173, 216, 230)); // lightBlue
        centerCircle->setPen(QPen(QColor(0, 0, 139), 3)); // darkBlue
        m_scene->addItem(centerCircle);
        
        QGraphicsSimpleTextItem *centerLabel = new QGraphicsSimpleTextItem("中心");
        centerLabel->setPos(-15, -10);
        m_scene->addItem(centerLabel);
    }
    
    void connectSignals()
    {
        // 这里可以添加更多的信号连接
    }

private:
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QLabel *m_statusLabel;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Qt环境测试");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("TestApp");
    
    SimpleTestWindow window;
    window.show();
    
    qDebug() << "Qt环境测试程序启动成功";
    qDebug() << "Qt版本:" << QT_VERSION_STR;
    qDebug() << "应用程序版本:" << app.applicationVersion();
    
    return app.exec();
}

#include "simple_test.moc"
