#include "SortModel.h"
#include "VisualItem.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <random>

SortModel::SortModel(QObject *parent)
    : QObject(parent)
    , m_scene(nullptr)
    , m_barSpacing(40.0)
    , m_barWidth(30.0)
    , m_maxHeight(200.0)
    , m_currentAlgorithm(SortAlgorithm::BubbleSort)
    , m_algorithmRunning(false)
    , m_algorithmPaused(false)
    , m_algorithmTimer(new QTimer(this))
    , m_gen(m_rd())
{
    connect(m_algorithmTimer, &QTimer::timeout, this, &SortModel::processNextStep);
    m_algorithmTimer->setSingleShot(true);
}

SortModel::~SortModel()
{
    clearBarItems();
}

void SortModel::setData(const std::vector<int> &data)
{
    m_data = data;
    m_originalData = data;
    updateBarItems();
    emit dataChanged();
}

void SortModel::setRandomData(int size, int minValue, int maxValue)
{
    std::uniform_int_distribution<> dis(minValue, maxValue);
    m_data.clear();
    m_data.reserve(size);
    
    for (int i = 0; i < size; ++i) {
        m_data.push_back(dis(m_gen));
    }
    
    m_originalData = m_data;
    updateBarItems();
    emit dataChanged();
}

void SortModel::clearData()
{
    m_data.clear();
    m_originalData.clear();
    clearBarItems();
    emit dataChanged();
}

void SortModel::createBarItems()
{
    clearBarItems();
    
    for (size_t i = 0; i < m_data.size(); ++i) {
        BarItem *bar = new BarItem(m_data[i], static_cast<int>(i));
        bar->setBarWidth(m_barWidth);
        bar->setPosition(calculateBarPosition(static_cast<int>(i)));
        
        m_barItems.push_back(bar);
        
        if (m_scene) {
            m_scene->addItem(bar);
        }
    }
}

void SortModel::clearBarItems()
{
    for (BarItem *bar : m_barItems) {
        if (m_scene) {
            m_scene->removeItem(bar);
        }
        delete bar;
    }
    m_barItems.clear();
}

void SortModel::executeAlgorithm(SortAlgorithm algorithm)
{
    if (m_algorithmRunning) {
        stopAlgorithm();
    }
    
    if (m_data.empty()) {
        qDebug() << "No data to sort";
        return;
    }
    
    m_currentAlgorithm = algorithm;
    m_algorithmRunning = true;
    m_algorithmPaused = false;
    
    resetAlgorithmState();
    
    // 生成算法步骤
    switch (algorithm) {
        case SortAlgorithm::BubbleSort:
            generateBubbleSortSteps();
            break;
        case SortAlgorithm::SelectionSort:
            generateSelectionSortSteps();
            break;
        case SortAlgorithm::InsertionSort:
            generateInsertionSortSteps();
            break;
        case SortAlgorithm::QuickSort:
            generateQuickSortSteps(0, static_cast<int>(m_data.size()) - 1);
            break;
        case SortAlgorithm::MergeSort:
            generateMergeSortSteps(0, static_cast<int>(m_data.size()) - 1);
            break;
        case SortAlgorithm::HeapSort:
            generateHeapSortSteps();
            break;
        case SortAlgorithm::RadixSort:
            generateRadixSortSteps();
            break;
    }
    
    emit algorithmStarted(algorithm);
    
    if (!m_algorithmSteps.empty()) {
        m_algorithmTimer->start(1000); // 1秒后开始第一步
    }
}

void SortModel::pauseAlgorithm()
{
    if (m_algorithmRunning && !m_algorithmPaused) {
        m_algorithmPaused = true;
        m_algorithmTimer->stop();
        emit algorithmPaused();
    }
}

void SortModel::resumeAlgorithm()
{
    if (m_algorithmRunning && m_algorithmPaused) {
        m_algorithmPaused = false;
        m_algorithmTimer->start(500); // 500ms后继续
        emit algorithmResumed();
    }
}

void SortModel::stopAlgorithm()
{
    if (m_algorithmRunning) {
        m_algorithmRunning = false;
        m_algorithmPaused = false;
        m_algorithmTimer->stop();
        
        resetVisualization();
        emit algorithmStopped();
    }
}

void SortModel::stepAlgorithm()
{
    if (m_algorithmRunning && m_algorithmPaused && !m_algorithmSteps.empty()) {
        processNextStep();
    }
}

void SortModel::setScene(QGraphicsScene *scene)
{
    m_scene = scene;
    
    // 将现有柱状图项添加到场景
    for (BarItem *bar : m_barItems) {
        scene->addItem(bar);
    }
}

void SortModel::updateLayout()
{
    for (size_t i = 0; i < m_barItems.size(); ++i) {
        m_barItems[i]->setPosition(calculateBarPosition(static_cast<int>(i)));
    }
}

void SortModel::setBarSpacing(qreal spacing)
{
    if (m_barSpacing != spacing) {
        m_barSpacing = spacing;
        updateLayout();
    }
}

void SortModel::resetVisualization()
{
    // 重置所有柱状图项的可视化状态
    for (BarItem *bar : m_barItems) {
        bar->setVisualState(VisualState::Normal);
        bar->stopAnimations();
    }
}

void SortModel::resetData()
{
    m_data = m_originalData;
    updateBarItems();
    resetVisualization();
}

void SortModel::processNextStep()
{
    if (m_algorithmSteps.empty()) {
        m_algorithmRunning = false;
        emit algorithmCompleted(m_currentAlgorithm);
        return;
    }
    
    SortStep step = m_algorithmSteps.front();
    m_algorithmSteps.pop();
    
    // 应用可视化状态
    for (int index : step.indices) {
        if (index >= 0 && index < static_cast<int>(m_barItems.size())) {
            BarItem *bar = m_barItems[index];
            bar->setVisualState(step.state);
            
            if (step.state == VisualState::Current) {
                bar->startCompareAnimation();
            } else if (step.state == VisualState::Selected) {
                bar->startSelectAnimation();
            }
        }
    }
    
    // 处理交换操作
    if (step.isSwap && step.newPositions.size() >= 2) {
        int index1 = step.newPositions[0];
        int index2 = step.newPositions[1];
        animateSwap(index1, index2);
        emit swapPerformed(index1, index2);
    }
    
    emit algorithmStepCompleted(step);
    
    // 继续下一步
    if (!m_algorithmSteps.empty() && m_algorithmRunning && !m_algorithmPaused) {
        m_algorithmTimer->start(step.delay);
    }
}

void SortModel::generateBubbleSortSteps()
{
    addAlgorithmStep("开始冒泡排序", {}, {}, VisualState::Current);
    
    std::vector<int> data = m_data;
    int n = static_cast<int>(data.size());
    
    for (int i = 0; i < n - 1; ++i) {
        addAlgorithmStep(QString("第 %1 轮排序").arg(i + 1), {}, {}, VisualState::Normal);
        
        for (int j = 0; j < n - i - 1; ++j) {
            addAlgorithmStep(QString("比较元素 %1 和 %2").arg(data[j]).arg(data[j + 1]), 
                           {j, j + 1}, {data[j], data[j + 1]}, VisualState::Current);
            
            if (data[j] > data[j + 1]) {
                addAlgorithmStep(QString("交换元素 %1 和 %2").arg(data[j]).arg(data[j + 1]), 
                               {j, j + 1}, {data[j], data[j + 1]}, VisualState::Selected, 1000, true, {j, j + 1});
                
                std::swap(data[j], data[j + 1]);
                m_data[j] = data[j];
                m_data[j + 1] = data[j + 1];
            }
        }
    }
    
    addAlgorithmStep("冒泡排序完成", {}, {}, VisualState::Normal);
}

void SortModel::generateSelectionSortSteps()
{
    addAlgorithmStep("开始选择排序", {}, {}, VisualState::Current);
    
    std::vector<int> data = m_data;
    int n = static_cast<int>(data.size());
    
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        
        addAlgorithmStep(QString("第 %1 轮，寻找最小元素").arg(i + 1), {i}, {data[i]}, VisualState::Current);
        
        for (int j = i + 1; j < n; ++j) {
            addAlgorithmStep(QString("比较 %1 和当前最小值 %2").arg(data[j]).arg(data[minIndex]), 
                           {j, minIndex}, {data[j], data[minIndex]}, VisualState::Current);
            
            if (data[j] < data[minIndex]) {
                minIndex = j;
                addAlgorithmStep(QString("找到新的最小值 %1").arg(data[minIndex]), 
                               {minIndex}, {data[minIndex]}, VisualState::Selected);
            }
        }
        
        if (minIndex != i) {
            addAlgorithmStep(QString("交换 %1 和 %2").arg(data[i]).arg(data[minIndex]), 
                           {i, minIndex}, {data[i], data[minIndex]}, VisualState::Selected, 1000, true, {i, minIndex});
            
            std::swap(data[i], data[minIndex]);
            m_data[i] = data[i];
            m_data[minIndex] = data[minIndex];
        }
    }
    
    addAlgorithmStep("选择排序完成", {}, {}, VisualState::Normal);
}

void SortModel::generateInsertionSortSteps()
{
    addAlgorithmStep("开始插入排序", {}, {}, VisualState::Current);
    
    std::vector<int> data = m_data;
    int n = static_cast<int>(data.size());
    
    for (int i = 1; i < n; ++i) {
        int key = data[i];
        int j = i - 1;
        
        addAlgorithmStep(QString("处理元素 %1").arg(key), {i}, {key}, VisualState::Current);
        
        while (j >= 0 && data[j] > key) {
            addAlgorithmStep(QString("移动元素 %1 到右侧").arg(data[j]), 
                           {j, j + 1}, {data[j], data[j + 1]}, VisualState::Current);
            
            data[j + 1] = data[j];
            m_data[j + 1] = data[j + 1];
            j--;
        }
        
        data[j + 1] = key;
        m_data[j + 1] = key;
        
        addAlgorithmStep(QString("插入元素 %1 到位置 %2").arg(key).arg(j + 1), 
                       {j + 1}, {key}, VisualState::Selected);
    }
    
    addAlgorithmStep("插入排序完成", {}, {}, VisualState::Normal);
}

void SortModel::generateQuickSortSteps(int low, int high)
{
    if (low < high) {
        addAlgorithmStep(QString("快速排序子数组 [%1, %2]").arg(low).arg(high), 
                       {low, high}, {m_data[low], m_data[high]}, VisualState::Current);
        
        int pivotIndex = partition(low, high);
        
        addAlgorithmStep(QString("选择基准元素 %1，分区完成").arg(m_data[pivotIndex]), 
                       {pivotIndex}, {m_data[pivotIndex]}, VisualState::Selected);
        
        generateQuickSortSteps(low, pivotIndex - 1);
        generateQuickSortSteps(pivotIndex + 1, high);
    }
}

void SortModel::generateMergeSortSteps(int left, int right)
{
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        addAlgorithmStep(QString("归并排序子数组 [%1, %2]").arg(left).arg(right), 
                       {left, right}, {m_data[left], m_data[right]}, VisualState::Current);
        
        generateMergeSortSteps(left, mid);
        generateMergeSortSteps(mid + 1, right);
        merge(left, mid, right);
    }
}

void SortModel::generateHeapSortSteps()
{
    addAlgorithmStep("开始堆排序", {}, {}, VisualState::Current);
    
    int n = static_cast<int>(m_data.size());
    
    // 构建最大堆
    addAlgorithmStep("构建最大堆", {}, {}, VisualState::Current);
    buildHeap();
    
    // 逐个提取元素
    for (int i = n - 1; i > 0; --i) {
        addAlgorithmStep(QString("交换根节点 %1 和最后一个元素 %2").arg(m_data[0]).arg(m_data[i]), 
                       {0, i}, {m_data[0], m_data[i]}, VisualState::Selected, 1000, true, {0, i});
        
        std::swap(m_data[0], m_data[i]);
        
        addAlgorithmStep(QString("重新堆化，堆大小: %1").arg(i), 
                       {0}, {m_data[0]}, VisualState::Current);
        
        heapify(i, 0);
    }
    
    addAlgorithmStep("堆排序完成", {}, {}, VisualState::Normal);
}

void SortModel::generateRadixSortSteps()
{
    addAlgorithmStep("开始基数排序", {}, {}, VisualState::Current);
    
    // 简化的基数排序实现
    int maxElement = *std::max_element(m_data.begin(), m_data.end());
    
    for (int exp = 1; maxElement / exp > 0; exp *= 10) {
        addAlgorithmStep(QString("按第 %1 位排序").arg(exp), {}, {}, VisualState::Current);
        
        // 这里可以实现完整的基数排序步骤
        std::vector<int> output(m_data.size());
        std::vector<int> count(10, 0);
        
        // 计数阶段
        for (int value : m_data) {
            count[(value / exp) % 10]++;
        }
        
        // 累加计数
        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }
        
        // 构建输出数组
        for (int i = static_cast<int>(m_data.size()) - 1; i >= 0; --i) {
            int digit = (m_data[i] / exp) % 10;
            output[count[digit] - 1] = m_data[i];
            count[digit]--;
        }
        
        m_data = output;
    }
    
    addAlgorithmStep("基数排序完成", {}, {}, VisualState::Normal);
}

void SortModel::resetAlgorithmState()
{
    m_visited.assign(m_data.size(), false);
    m_auxiliaryArray.clear();
    resetVisualization();
}

void SortModel::addAlgorithmStep(const QString &description, 
                               const std::vector<int> &indices,
                               const std::vector<int> &values,
                               VisualState state,
                               int delay,
                               bool isSwap,
                               const std::vector<int> &newPositions)
{
    SortStep step;
    step.description = description;
    step.indices = indices;
    step.values = values;
    step.state = state;
    step.delay = delay;
    step.isSwap = isSwap;
    step.newPositions = newPositions;
    
    m_algorithmSteps.push(step);
}

int SortModel::partition(int low, int high)
{
    int pivot = m_data[high];
    int i = low - 1;
    
    for (int j = low; j < high; ++j) {
        if (m_data[j] <= pivot) {
            i++;
            std::swap(m_data[i], m_data[j]);
        }
    }
    
    std::swap(m_data[i + 1], m_data[high]);
    return i + 1;
}

void SortModel::merge(int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    std::vector<int> leftArray(n1);
    std::vector<int> rightArray(n2);
    
    for (int i = 0; i < n1; ++i) {
        leftArray[i] = m_data[left + i];
    }
    for (int j = 0; j < n2; ++j) {
        rightArray[j] = m_data[mid + 1 + j];
    }
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (leftArray[i] <= rightArray[j]) {
            m_data[k] = leftArray[i];
            i++;
        } else {
            m_data[k] = rightArray[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        m_data[k] = leftArray[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        m_data[k] = rightArray[j];
        j++;
        k++;
    }
}

void SortModel::heapify(int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && m_data[left] > m_data[largest]) {
        largest = left;
    }
    
    if (right < n && m_data[right] > m_data[largest]) {
        largest = right;
    }
    
    if (largest != i) {
        std::swap(m_data[i], m_data[largest]);
        heapify(n, largest);
    }
}

void SortModel::buildHeap()
{
    int n = static_cast<int>(m_data.size());
    for (int i = n / 2 - 1; i >= 0; --i) {
        heapify(n, i);
    }
}

void SortModel::updateBarItems()
{
    clearBarItems();
    createBarItems();
}

QPointF SortModel::calculateBarPosition(int index) const
{
    qreal x = index * m_barSpacing;
    qreal y = 0; // 柱状图底部对齐
    return QPointF(x, y);
}

void SortModel::animateSwap(int index1, int index2)
{
    if (index1 >= 0 && index1 < static_cast<int>(m_barItems.size()) &&
        index2 >= 0 && index2 < static_cast<int>(m_barItems.size())) {
        
        BarItem *bar1 = m_barItems[index1];
        BarItem *bar2 = m_barItems[index2];
        
        QPointF pos1 = bar1->pos();
        QPointF pos2 = bar2->pos();
        
        bar1->startSwapAnimation(pos2);
        bar2->startSwapAnimation(pos1);
        
        // 更新数据
        std::swap(m_data[index1], m_data[index2]);
        bar1->setValue(m_data[index1]);
        bar2->setValue(m_data[index2]);
    }
}
