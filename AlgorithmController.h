#ifndef ALGORITHMCONTROLLER_H
#define ALGORITHMCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointer>
#include "MyVectorInt.h"

// 前向声明
class GraphModel;
class SortModel;

// 算法控制器基类
class AlgorithmController : public QObject
{
    Q_OBJECT

public:
    explicit AlgorithmController(QObject *parent = nullptr);
    virtual ~AlgorithmController() = default;

    // 控制面板创建
    virtual QWidget* createControlPanel() = 0;
    
    // 算法控制
    virtual void startAlgorithm() = 0;
    virtual void pauseAlgorithm() = 0;
    virtual void resumeAlgorithm() = 0;
    virtual void stopAlgorithm() = 0;
    virtual void stepAlgorithm() = 0;
    virtual void resetAlgorithm() = 0;
    
    // 状态查询
    bool isRunning() const { return m_isRunning; }
    bool isPaused() const { return m_isPaused; }

signals:
    void algorithmStarted();
    void algorithmPaused();
    void algorithmResumed();
    void algorithmStopped();
    void algorithmCompleted();
    void stepCompleted(const QString &description);

protected:
    // 控制面板组件
    QWidget *m_controlPanel;
    QPushButton *m_startButton;
    QPushButton *m_pauseButton;
    QPushButton *m_resumeButton;
    QPushButton *m_stopButton;
    QPushButton *m_stepButton;
    QPushButton *m_resetButton;
    QTextEdit *m_logTextEdit;
    
    // 状态管理
    bool m_isRunning;
    bool m_isPaused;
    
    // 辅助方法
    void setupControlPanel();
    void updateButtonStates();
    void logMessage(const QString &message);
};

// 图算法控制器
class GraphAlgorithmController : public AlgorithmController
{
    Q_OBJECT

public:
    explicit GraphAlgorithmController(QObject *parent = nullptr);
    ~GraphAlgorithmController() = default;

    // 设置模型
    void setGraphModel(GraphModel *model);
    GraphModel* getGraphModel() const { return m_graphModel; }
    
    // 控制面板创建
    QWidget* createControlPanel() override;
    
    // 算法控制
    void startAlgorithm() override;
    void pauseAlgorithm() override;
    void resumeAlgorithm() override;
    void stopAlgorithm() override;
    void stepAlgorithm() override;
    void resetAlgorithm() override;
    
    // 图操作
    void addVertex(const QString &label, const QPointF &position = QPointF(0, 0));
    void removeVertex(const QString &label);
    void addEdge(const QString &from, const QString &to, int weight = 1);
    void removeEdge(const QString &from, const QString &to);
    
    // 布局控制
    void applyCircularLayout();
    void applyForceDirectedLayout();
    void applyGridLayout(int columns = 0);
    
    // 算法选择
    void setAlgorithm(int algorithmIndex);
    QString getCurrentAlgorithmName() const;

private slots:
    void onAlgorithmStarted();
    void onAlgorithmStepCompleted();
    void onAlgorithmCompleted();
    void onAlgorithmPaused();
    void onAlgorithmResumed();
    void onAlgorithmStopped();
    void onStartButtonClicked();
    void onPauseButtonClicked();
    void onResumeButtonClicked();
    void onStopButtonClicked();
    void onStepButtonClicked();
    void onResetButtonClicked();
    void onAlgorithmComboBoxChanged(int index);
    void onStartVertexChanged();
    void onDirectedCheckBoxToggled(bool checked);
    void onLayoutButtonClicked();
    void onDijkstraTableUpdate(const QString &vertex, int distance, const QString &predecessor, bool visited);

private:
    GraphModel *m_graphModel;
    QPointer<class DijkstraTableDialog> m_dijkstraTableDialog;  // 使用QPointer安全管理对话框
    
    // 控制面板组件
    QComboBox *m_algorithmComboBox;
    QLineEdit *m_startVertexEdit;
    QCheckBox *m_directedCheckBox;
    QPushButton *m_circularLayoutButton;
    QPushButton *m_forceLayoutButton;
    QPushButton *m_gridLayoutButton;
    QSpinBox *m_gridColumnsSpinBox;
    
    // 图操作组件
    QGroupBox *m_graphOperationsGroup;
    QLineEdit *m_vertexLabelEdit;
    QPushButton *m_addVertexButton;
    QPushButton *m_removeVertexButton;
    QLineEdit *m_edgeFromEdit;
    QLineEdit *m_edgeToEdit;
    QSpinBox *m_edgeWeightSpinBox;
    QPushButton *m_addEdgeButton;
    QPushButton *m_removeEdgeButton;
    QPushButton *m_showMatrixButton;
    QPushButton *m_showAdjacencyListButton;
    
    void setupControlPanel();
    void setupGraphControlPanel();
    void setupGraphOperationsPanel();
    void setupLayoutPanel();
    void updateGraphOperationButtons();
    
private slots:
    void onShowMatrixButtonClicked();
    void onShowAdjacencyListButtonClicked();
};

// 排序算法控制器
class SortAlgorithmController : public AlgorithmController
{
    Q_OBJECT

public:
    explicit SortAlgorithmController(QObject *parent = nullptr);
    ~SortAlgorithmController() = default;

    // 设置模型
    void setSortModel(SortModel *model);
    SortModel* getSortModel() const { return m_sortModel; }
    
    // 控制面板创建
    QWidget* createControlPanel() override;
    
    // 算法控制
    void startAlgorithm() override;
    void pauseAlgorithm() override;
    void resumeAlgorithm() override;
    void stopAlgorithm() override;
    void stepAlgorithm() override;
    void resetAlgorithm() override;
    
    // 数据管理
    void setRandomData(int size, int minValue = 1, int maxValue = 100);
    void setCustomData(const MyVectorInt &data);
    void clearData();
    
    // 算法选择
    void setAlgorithm(int algorithmIndex);
    QString getCurrentAlgorithmName() const;

private slots:
    void onAlgorithmStarted();
    void onAlgorithmStepCompleted();
    void onAlgorithmCompleted();
    void onAlgorithmPaused();
    void onAlgorithmResumed();
    void onAlgorithmStopped();
    void onStartButtonClicked();
    void onPauseButtonClicked();
    void onResumeButtonClicked();
    void onStopButtonClicked();
    void onStepButtonClicked();
    void onResetButtonClicked();
    void onAlgorithmComboBoxChanged(int index);
    void onDataSizeChanged(int size);
    void onMinValueChanged(int value);
    void onMaxValueChanged(int value);
    void onGenerateDataButtonClicked();
    void onClearDataButtonClicked();

private:
    SortModel *m_sortModel;
    
    // 控制面板组件
    QComboBox *m_algorithmComboBox;
    QSpinBox *m_dataSizeSpinBox;
    QSpinBox *m_minValueSpinBox;
    QSpinBox *m_maxValueSpinBox;
    QPushButton *m_generateDataButton;
    QPushButton *m_clearDataButton;
    
    void setupControlPanel();
    void setupSortControlPanel();
    void setupDataGenerationPanel();
};

#endif // ALGORITHMCONTROLLER_H
