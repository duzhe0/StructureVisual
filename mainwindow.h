#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// 前向声明
class GraphModel;
class SortModel;
class GraphAlgorithmController;
class SortAlgorithmController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void doNow();

private slots:
    void handleGraphPB();
    void handleSortPB();
    void switchToGraphMode();
    void switchToSortMode();
    void onAlgorithmStarted();
    void onAlgorithmCompleted();
    void onAlgorithmStepCompleted(const QString &description);
    void updateStatusBar();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupCentralWidget();
    void setupGraphVisualization();
    void setupSortVisualization();
    void connectSignals();
    void initializeModels();

private:
    Ui::MainWindow *ui;
    
    // 核心组件
    QSplitter *m_mainSplitter;
    QTabWidget *m_modeTabWidget;
    QStackedWidget *m_visualizationStack;
    
    // 图可视化组件
    QGraphicsView *m_graphView;
    QGraphicsScene *m_graphScene;
    GraphModel *m_graphModel;
    GraphAlgorithmController *m_graphController;
    
    // 排序可视化组件
    QGraphicsView *m_sortView;
    QGraphicsScene *m_sortScene;
    SortModel *m_sortModel;
    SortAlgorithmController *m_sortController;
    
    // 状态栏组件
    QLabel *m_statusLabel;
    QLabel *m_algorithmLabel;
    QProgressBar *m_progressBar;
    QTimer *m_statusTimer;
    
    // 菜单和工具栏
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_algorithmMenu;
    QMenu *m_helpMenu;
    
    // 当前模式
    enum VisualizationMode {
        GraphMode,
        SortMode
    } m_currentMode;
};

#endif // MAINWINDOW_H