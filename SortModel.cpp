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
    
    // 1. 重置数据到原始状态，确保每次排序从头开始
    resetData();
    
    // 2. 复制一份"草稿数据"用于算法生成步骤
    // 这样可以避免在生成步骤时就修改了真实数据，导致动画执行时状态不一致
    std::vector<int> tempData = m_data;
    
    m_currentAlgorithm = algorithm;
    m_algorithmRunning = true;
    m_algorithmPaused = false;
    
    resetAlgorithmState();
    
    // 生成算法步骤（操作 tempData）
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
            generateQuickSortSteps(tempData, 0, static_cast<int>(tempData.size()) - 1);
            break;
        case SortAlgorithm::MergeSort:
            generateMergeSortSteps(tempData, 0, static_cast<int>(tempData.size()) - 1);
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
    
    // 重置所有非 Visited 的柱子状态为 Normal，避免颜色残留
    // 但保留当前步骤的基准元素（如果有）的 Selected 状态
    for (BarItem *bar : m_barItems) {
        if (bar->getVisualState() != VisualState::Visited) {
            // 如果这个柱子是当前步骤的基准元素，保留其 Selected 状态
            int barIndex = bar->getIndex();
            if (step.pivotIndex >= 0 && barIndex == step.pivotIndex) {
                continue;  // 跳过，保持 Selected 状态
            }
            bar->setVisualState(VisualState::Normal);
        }
    }
    
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
    
    // 特殊处理：如果有基准元素，将其始终保持为 Selected (红色) 状态
    if (step.pivotIndex >= 0 && step.pivotIndex < static_cast<int>(m_barItems.size())) {
        m_barItems[step.pivotIndex]->setVisualState(VisualState::Selected);
    }
    
    // 处理交换操作
    if (step.isSwap && step.newPositions.size() >= 2) {
        int index1 = step.newPositions[0];
        int index2 = step.newPositions[1];
        animateSwap(index1, index2);
        emit swapPerformed(index1, index2);
    } 
    // 处理移动/覆盖操作 (isSwap=false 但有 newPositions 和 values)
    else if (!step.isSwap && step.newPositions.size() >= 2 && !step.values.empty()) {
        int srcIndex = step.newPositions[0];
        int destIndex = step.newPositions[1];
        int value = step.values[0];
        animateMove(srcIndex, destIndex, value);
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
    
    // 使用副本进行模拟，不修改原始 m_data
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
            // 此时 key 所在的元素已经在 j+1 的位置（因为它是一路交换过来的）
            addAlgorithmStep(QString("比较元素 %1 和 %2").arg(data[j]).arg(key), 
                           {j, j + 1}, {data[j], key}, VisualState::Current);
            
            // 移动操作：将data[j]向右移动一位（实际上是交换）
            addAlgorithmStep(QString("移动元素 %1 到位置 %2").arg(data[j]).arg(j + 1), 
                           {j, j + 1}, {data[j], data[j + 1]}, VisualState::Selected, 1000, true, {j, j + 1});
                           
            std::swap(data[j], data[j + 1]);
            j--;
        }
        // 此时 key 已经在 j+1 的位置了，无需再赋值或交换
    }
    
    addAlgorithmStep("插入排序完成", {}, {}, VisualState::Normal);
}

void SortModel::generateQuickSortSteps(std::vector<int>& data, int low, int high)
{
    if (low < high) {
        addAlgorithmStep(QString("快速排序子数组 [%1, %2]").arg(low).arg(high), 
                       {low, high}, {data[low], data[high]}, VisualState::Current);
        
        int pivotIndex = partition(data, low, high);
        
        addAlgorithmStep(QString("基准元素 %1 已归位").arg(data[pivotIndex]), 
                       {pivotIndex}, {data[pivotIndex]}, VisualState::Visited);
        
        generateQuickSortSteps(data, low, pivotIndex - 1);
        generateQuickSortSteps(data, pivotIndex + 1, high);
    }
}

void SortModel::generateMergeSortSteps(std::vector<int>& data, int left, int right)
{
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        addAlgorithmStep(QString("归并排序子数组 [%1, %2]").arg(left).arg(right), 
                       {left, right}, {data[left], data[right]}, VisualState::Current);
        
        generateMergeSortSteps(data, left, mid);
        generateMergeSortSteps(data, mid + 1, right);
        merge(data, left, mid, right);
    }
}

void SortModel::generateHeapSortSteps()
{
    addAlgorithmStep("开始堆排序", {}, {}, VisualState::Current);
    
    // 使用副本进行模拟，不修改原始 m_data
    std::vector<int> data = m_data;
    int n = static_cast<int>(data.size());
    
    // 构建最大堆
    addAlgorithmStep("构建最大堆", {}, {}, VisualState::Current);
    buildHeap(data);
    
    // 逐个提取元素
    for (int i = n - 1; i > 0; --i) {
        addAlgorithmStep(QString("交换根节点 %1 和最后一个元素 %2").arg(data[0]).arg(data[i]), 
                       {0, i}, {data[0], data[i]}, VisualState::Selected, 1000, true, {0, i});
        
        std::swap(data[0], data[i]);
        
        addAlgorithmStep(QString("重新堆化，堆大小: %1").arg(i), 
                       {0}, {data[0]}, VisualState::Current);
        
        heapify(data, i, 0);
    }
    
    addAlgorithmStep("堆排序完成", {}, {}, VisualState::Normal);
}

void SortModel::generateRadixSortSteps()
{
    addAlgorithmStep("开始基数排序", {}, {}, VisualState::Current);
    
    // 使用副本进行模拟，不修改原始 m_data
    std::vector<int> data = m_data;
    int maxElement = *std::max_element(data.begin(), data.end());
    
    for (int exp = 1; maxElement / exp > 0; exp *= 10) {
        addAlgorithmStep(QString("按第 %1 位排序").arg(exp), {}, {}, VisualState::Current);
        
        std::vector<int> output(data.size());
        std::vector<int> count(10, 0);
        
        // 计数阶段
        addAlgorithmStep("计数阶段：统计每个数字的出现次数", {}, {}, VisualState::Normal);
        for (int value : data) {
            int digit = (value / exp) % 10;
            count[digit]++;
        }
        
        // 累加计数
        addAlgorithmStep("累加计数：计算每个数字的最终位置", {}, {}, VisualState::Normal);
        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }
        
        // 构建输出数组（从后往前，保持稳定性）
        addAlgorithmStep("重新排列：根据计数数组放置元素", {}, {}, VisualState::Current);
        for (int i = static_cast<int>(data.size()) - 1; i >= 0; --i) {
            int digit = (data[i] / exp) % 10;
            int newPos = count[digit] - 1;
            
            if (i != newPos) {
                // 注意：这里是移动/覆盖操作，所以 isSwap = false
                addAlgorithmStep(QString("移动元素 %1 到位置 %2 (第%3位是%4)").arg(data[i]).arg(newPos).arg(exp).arg(digit),
                                {i, newPos}, {data[i]}, VisualState::Selected, 1000, false, {i, newPos});
            }
            
            output[newPos] = data[i];
            count[digit]--;
        }
        
        data = output;
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
                               const std::vector<int> &newPositions,
                               int pivotIndex)
{
    SortStep step;
    step.description = description;
    step.indices = indices;
    step.values = values;
    step.state = state;
    step.delay = delay;
    step.isSwap = isSwap;
    step.newPositions = newPositions;
    step.pivotIndex = pivotIndex;
    
    m_algorithmSteps.push(step);
}

int SortModel::partition(std::vector<int>& data, int low, int high)
{
    int pivot = data[high];
    int i = low - 1;
    int currentPivotIndex = high;  // 跟踪基准元素的当前位置
    
    addAlgorithmStep(QString("选择基准元素: %1 (位置 %2)").arg(pivot).arg(high),
                    {high}, {pivot}, VisualState::Selected, 1000, false, {}, high);
    
    for (int j = low; j < high; ++j) {
        addAlgorithmStep(QString("比较元素 %1 和基准 %2").arg(data[j]).arg(pivot),
                        {j, currentPivotIndex}, {data[j], pivot}, VisualState::Current, 1000, false, {}, currentPivotIndex);
        
        if (data[j] <= pivot) {
            i++;
            if (i != j) {
                addAlgorithmStep(QString("交换元素 %1 和 %2").arg(data[i]).arg(data[j]),
                                {i, j}, {data[i], data[j]}, VisualState::Selected, 1000, true, {i, j}, currentPivotIndex);
                std::swap(data[i], data[j]);
            }
        }
    }
    
    // 基准元素归位
    if (i + 1 != high) {
        addAlgorithmStep(QString("将基准元素 %1 放到正确位置 %2").arg(pivot).arg(i + 1),
                        {i + 1, currentPivotIndex}, {data[i + 1], pivot}, VisualState::Selected, 1000, true, {i + 1, currentPivotIndex}, i + 1);
        std::swap(data[i + 1], data[high]);
        currentPivotIndex = i + 1;  // 更新基准元素位置
    }
    
    return i + 1;
}

void SortModel::merge(std::vector<int>& data, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    std::vector<int> leftArray(n1);
    std::vector<int> rightArray(n2);
    
    for (int i = 0; i < n1; ++i) {
        leftArray[i] = data[left + i];
    }
    for (int j = 0; j < n2; ++j) {
        rightArray[j] = data[mid + 1 + j];
    }
    
    addAlgorithmStep(QString("归并两个有序子数组 [%1,%2] 和 [%3,%4]").arg(left).arg(mid).arg(mid+1).arg(right),
                    {left, mid, mid+1, right}, {}, VisualState::Current);
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        addAlgorithmStep(QString("比较 %1 和 %2").arg(leftArray[i]).arg(rightArray[j]),
                        {left + i, mid + 1 + j}, {leftArray[i], rightArray[j]}, VisualState::Current);
        
        if (leftArray[i] <= rightArray[j]) {
            if (k != left + i) {
                addAlgorithmStep(QString("移动元素 %1 到位置 %2").arg(leftArray[i]).arg(k),
                                {left + i, k}, {leftArray[i]}, VisualState::Selected, 1000, false, {left + i, k});
            }
            data[k] = leftArray[i];
            i++;
        } else {
            if (k != mid + 1 + j) {
                addAlgorithmStep(QString("移动元素 %1 到位置 %2").arg(rightArray[j]).arg(k),
                                {mid + 1 + j, k}, {rightArray[j]}, VisualState::Selected, 1000, false, {mid + 1 + j, k});
            }
            data[k] = rightArray[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        if (k != left + i) {
            addAlgorithmStep(QString("移动剩余元素 %1 到位置 %2").arg(leftArray[i]).arg(k),
                            {left + i, k}, {leftArray[i]}, VisualState::Selected, 1000, false, {left + i, k});
        }
        data[k] = leftArray[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        if (k != mid + 1 + j) {
            addAlgorithmStep(QString("移动剩余元素 %1 到位置 %2").arg(rightArray[j]).arg(k),
                            {mid + 1 + j, k}, {rightArray[j]}, VisualState::Selected, 1000, false, {mid + 1 + j, k});
        }
        data[k] = rightArray[j];
        j++;
        k++;
    }
    
    addAlgorithmStep(QString("归并完成，子数组 [%1,%2] 已有序").arg(left).arg(right),
                    {}, {}, VisualState::Normal);
}

void SortModel::heapify(std::vector<int>& data, int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n) {
        addAlgorithmStep(QString("比较节点 %1 和左子节点 %2").arg(data[i]).arg(data[left]),
                        {i, left}, {data[i], data[left]}, VisualState::Current);
        if (data[left] > data[largest]) {
            largest = left;
        }
    }
    
    if (right < n) {
        addAlgorithmStep(QString("比较节点 %1 和右子节点 %2").arg(data[largest]).arg(data[right]),
                        {largest, right}, {data[largest], data[right]}, VisualState::Current);
        if (data[right] > data[largest]) {
            largest = right;
        }
    }
    
    if (largest != i) {
        addAlgorithmStep(QString("交换节点 %1 和 %2").arg(data[i]).arg(data[largest]),
                        {i, largest}, {data[i], data[largest]}, VisualState::Selected, 1000, true, {i, largest});
        std::swap(data[i], data[largest]);
        heapify(data, n, largest);
    }
}

void SortModel::buildHeap(std::vector<int>& data)
{
    int n = static_cast<int>(data.size());
    for (int i = n / 2 - 1; i >= 0; --i) {
        addAlgorithmStep(QString("堆化节点 %1 (值: %2)").arg(i).arg(data[i]),
                        {i}, {data[i]}, VisualState::Current);
        heapify(data, n, i);
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
        
        // 1. 交换数据 (逻辑层)
        std::swap(m_data[index1], m_data[index2]);
        
        // 2. 交换柱子对象 (视觉层 - 关键步骤！)
        // 这样 m_barItems[index1] 指向的就是刚才从右边飞过来的那个柱子了
        std::swap(m_barItems[index1], m_barItems[index2]);
        
        // 3. 更新柱子内部的索引记录
        bar1->setIndex(index2);
        bar2->setIndex(index1);
        
        // 注意：不需要 setValue！柱子带着它原来的数值飞过去就行了。
    }
}

void SortModel::animateMove(int srcIndex, int destIndex, int value)
{
    if (destIndex >= 0 && destIndex < static_cast<int>(m_barItems.size())) {
        BarItem *destBar = m_barItems[destIndex];
        QPointF destPos = calculateBarPosition(destIndex);
        
        // 如果源位置和目标位置不同，播放移动动画
        if (srcIndex >= 0 && srcIndex < static_cast<int>(m_barItems.size()) && srcIndex != destIndex) {
            QPointF srcPos = calculateBarPosition(srcIndex);
            
            // 先高亮源位置（表示数据来自这里）
            m_barItems[srcIndex]->startHighlightAnimation();
            
            // 更新数据
            m_data[destIndex] = value;
            destBar->setValue(value);
            
            // 让目标位置的柱子从源位置"飞"过来（视觉上的移动效果）
            // 先临时移动到源位置
            destBar->setPos(srcPos);
            // 然后动画移动到目标位置
            destBar->startSwapAnimation(destPos);
        } else {
            // 如果源位置和目标位置相同，只更新数值
            m_data[destIndex] = value;
            destBar->setValue(value);
            destBar->startSelectAnimation();
        }
    }
}
