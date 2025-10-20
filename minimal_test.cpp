#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

// 最简单的测试程序
class MinimalTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    MinimalTestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("最小测试程序");
        setMinimumSize(400, 300);
        
        QWidget *centralWidget = new QWidget();
        setCentralWidget(centralWidget);
        
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        
        QLabel *label = new QLabel("Qt环境测试成功！");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 18px; color: green; font-weight: bold;");
        
        QPushButton *button = new QPushButton("点击测试");
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #3498db;"
            "    color: white;"
            "    border: none;"
            "    padding: 15px;"
            "    border-radius: 8px;"
            "    font-size: 14px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: #2980b9;"
            "}"
        );
        
        connect(button, &QPushButton::clicked, this, [this]() {
            QMessageBox::information(this, "成功", 
                "恭喜！\n\n"
                "Qt 6.10.0 MinGW 64-bit 环境配置成功！\n"
                "基本UI组件工作正常。\n\n"
                "现在可以编译更复杂的项目了。");
        });
        
        layout->addWidget(label);
        layout->addWidget(button);
        layout->addStretch();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("最小测试");
    app.setApplicationVersion("1.0");
    
    MinimalTestWindow window;
    window.show();
    
    return app.exec();
}

#include "minimal_test.moc"
