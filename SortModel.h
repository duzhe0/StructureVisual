#ifndef SORTMODEL_H
#define SORTMODEL_H

#include <QObject>
#include <QTimer>
#include <QColor>
#include <memory>
#include <vector>
#include <queue>
#include <random>
#include "VisualItem.h"

// 前向声明
class BarItem;
class QGraphicsScene;

// 排序算法类型枚举
enum class SortAlgorithm {
    BubbleSort,     // 冒泡排序
    SelectionSort,  // 选择排序
    InsertionSort,  // 插入排序
    QuickSort,      // 快速排序
    MergeSort,      // 归并排序
    HeapSort,       // 堆排序
    RadixSort       // 基数排序
};

// 排序步骤结构
struct SortStep {
    QString description;                    // 步骤描述
    std::vector<int> indices;              // 涉及的索引
    std::vector<int> values;               // 涉及的值
    VisualState state;                     // 可视化状态
    int delay;                             // 延迟时间(ms)
    bool isSwap;                           // 是否为交换操作
    std::vector<int> newPositions;         // 新位置（用于交换动画）
    int pivotIndex = -1;                   // 基准元素索引（用于快速排序等高亮）
};

// 排序数据模型类
class SortModel : public QObject
{
    Q_OBJECT

public:
    explicit SortModel(QObject *parent = nullptr);
    ~SortModel();

    // 数据管理
    void setData(const std::vector<int> &data);
    void setRandomData(int size, int minValue = 1, int maxValue = 100);
    void clearData();
    std::vector<int> getData() const { return m_data; }
    
    // 可视化项管理
    void createBarItems();
    void clearBarItems();
    std::vector<BarItem*> getBarItems() const { return m_barItems; }
    
    // 算法执行
    void executeAlgorithm(SortAlgorithm algorithm);
    void pauseAlgorithm();
    void resumeAlgorithm();
    void stopAlgorithm();
    void stepAlgorithm();
    
    // 算法状态
    bool isAlgorithmRunning() const { return m_algorithmRunning; }
    bool isAlgorithmPaused() const { return m_algorithmPaused; }
    SortAlgorithm getCurrentAlgorithm() const { return m_currentAlgorithm; }
    
    // 场景管理
    void setScene(QGraphicsScene *scene);
    QGraphicsScene* getScene() const { return m_scene; }
    
    // 布局管理
    void updateLayout();
    void setBarSpacing(qreal spacing);
    qreal getBarSpacing() const { return m_barSpacing; }
    
    // 重置和清理
    void resetVisualization();
    void resetData();

signals:
    void algorithmStarted(SortAlgorithm algorithm);
    void algorithmStepCompleted(const SortStep &step);
    void algorithmCompleted(SortAlgorithm algorithm);
    void algorithmPaused();
    void algorithmResumed();
    void algorithmStopped();
    void dataChanged();
    void comparisonMade(int index1, int index2, bool isGreater);
    void swapPerformed(int index1, int index2);

private slots:
    void processNextStep();

private:
    // 数据存储
    std::vector<int> m_data;
    std::vector<int> m_originalData;
    
    // 可视化项
    std::vector<BarItem*> m_barItems;
    QGraphicsScene *m_scene;
    
    // 布局参数
    qreal m_barSpacing;
    qreal m_barWidth;
    qreal m_maxHeight;
    
    // 算法执行相关
    SortAlgorithm m_currentAlgorithm;
    bool m_algorithmRunning;
    bool m_algorithmPaused;
    std::queue<SortStep> m_algorithmSteps;
    QTimer *m_algorithmTimer;
    
    // 算法状态跟踪
    std::vector<bool> m_visited;
    std::vector<int> m_auxiliaryArray; // 用于归并排序等需要辅助数组的算法
    
    // 算法实现
    void generateBubbleSortSteps();
    void generateSelectionSortSteps();
    void generateInsertionSortSteps();
    void generateQuickSortSteps(std::vector<int>& data, int low, int high);
    void generateMergeSortSteps(std::vector<int>& data, int left, int right);
    void generateHeapSortSteps();
    void generateRadixSortSteps();
    
    // 辅助方法
    void resetAlgorithmState();
    void addAlgorithmStep(const QString &description, 
                         const std::vector<int> &indices = {},
                         const std::vector<int> &values = {},
                         VisualState state = VisualState::Current,
                         int delay = 1000,
                         bool isSwap = false,
                         const std::vector<int> &newPositions = {},
                         int pivotIndex = -1);
    
    // 算法辅助函数
    int partition(std::vector<int>& data, int low, int high);
    void merge(std::vector<int>& data, int left, int mid, int right);
    void heapify(std::vector<int>& data, int n, int i);
    void buildHeap(std::vector<int>& data);
    
    // 可视化辅助
    void updateBarItems();
    QPointF calculateBarPosition(int index) const;
    void animateSwap(int index1, int index2);
    void animateMove(int srcIndex, int destIndex, int value);
    
    // 随机数生成
    std::random_device m_rd;
    std::mt19937 m_gen;
};

#endif // SORTMODEL_H
