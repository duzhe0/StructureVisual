#include "SortModel.h"
#include "VisualItem.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
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

void SortModel::setData(const MyVectorInt &data)
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
    for (size_t i = 0; i < m_barItems.size(); ++i) {
        BarItem *bar = m_barItems[i];
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
    MyVectorInt tempData = m_data;
    
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
    for (size_t i = 0; i < m_barItems.size(); ++i) {
        BarItem *bar = m_barItems[i];
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
    for (size_t i = 0; i < m_barItems.size(); ++i) {
        BarItem *bar = m_barItems[i];
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
    for (size_t i = 0; i < m_barItems.size(); ++i) {
        BarItem *bar = m_barItems[i];
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
    for (size_t i = 0; i < step.indices.size(); ++i) {
        int index = step.indices[i];
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
    addAlgorithmStep("开始冒泡排序", MyVectorInt(), MyVectorInt(), VisualState::Current);
    
    // 使用副本进行模拟，不修改原始 m_data
    MyVectorInt data = m_data;
    int n = static_cast<int>(data.size());
    
    for (int i = 0; i < n - 1; ++i) {
        addAlgorithmStep(QString("第 %1 轮排序").arg(i + 1), MyVectorInt(), MyVectorInt(), VisualState::Normal);
        
        for (int j = 0; j < n - i - 1; ++j) {
            MyVectorInt indices;
            indices.push_back(j);
            indices.push_back(j + 1);
            MyVectorInt values;
            values.push_back(data[j]);
            values.push_back(data[j + 1]);
            addAlgorithmStep(QString("比较元素 %1 和 %2").arg(data[j]).arg(data[j + 1]), 
                           indices, values, VisualState::Current);
            
            if (data[j] > data[j + 1]) {
                MyVectorInt swapIndices;
                swapIndices.push_back(j);
                swapIndices.push_back(j + 1);
                MyVectorInt swapValues;
                swapValues.push_back(data[j]);
                swapValues.push_back(data[j + 1]);
                MyVectorInt newPos;
                newPos.push_back(j);
                newPos.push_back(j + 1);
                addAlgorithmStep(QString("交换元素 %1 和 %2").arg(data[j]).arg(data[j + 1]), 
                               swapIndices, swapValues, VisualState::Selected, 1000, true, newPos);
                
                int temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
    
    addAlgorithmStep("冒泡排序完成", MyVectorInt(), MyVectorInt(), VisualState::Normal);
}

void SortModel::generateSelectionSortSteps()
{
    addAlgorithmStep("开始选择排序", MyVectorInt(), MyVectorInt(), VisualState::Current);
    
    MyVectorInt data = m_data;
    int n = static_cast<int>(data.size());
    
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        
        MyVectorInt indices1;
        indices1.push_back(i);
        MyVectorInt values1;
        values1.push_back(data[i]);
        addAlgorithmStep(QString("第 %1 轮，寻找最小元素").arg(i + 1), indices1, values1, VisualState::Current);
        
        for (int j = i + 1; j < n; ++j) {
            MyVectorInt indices2;
            indices2.push_back(j);
            indices2.push_back(minIndex);
            MyVectorInt values2;
            values2.push_back(data[j]);
            values2.push_back(data[minIndex]);
            addAlgorithmStep(QString("比较 %1 和当前最小值 %2").arg(data[j]).arg(data[minIndex]), 
                           indices2, values2, VisualState::Current);
            
            if (data[j] < data[minIndex]) {
                minIndex = j;
                MyVectorInt indices3;
                indices3.push_back(minIndex);
                MyVectorInt values3;
                values3.push_back(data[minIndex]);
                addAlgorithmStep(QString("找到新的最小值 %1").arg(data[minIndex]), 
                               indices3, values3, VisualState::Selected);
            }
        }
        
        if (minIndex != i) {
            MyVectorInt indices4;
            indices4.push_back(i);
            indices4.push_back(minIndex);
            MyVectorInt values4;
            values4.push_back(data[i]);
            values4.push_back(data[minIndex]);
            MyVectorInt newPos4;
            newPos4.push_back(i);
            newPos4.push_back(minIndex);
            addAlgorithmStep(QString("交换 %1 和 %2").arg(data[i]).arg(data[minIndex]), 
                           indices4, values4, VisualState::Selected, 1000, true, newPos4);
            
            int temp = data[i];
            data[i] = data[minIndex];
            data[minIndex] = temp;
        }
    }
    
    addAlgorithmStep("选择排序完成", MyVectorInt(), MyVectorInt(), VisualState::Normal);
}

void SortModel::generateInsertionSortSteps()
{
    addAlgorithmStep("开始插入排序", MyVectorInt(), MyVectorInt(), VisualState::Current);
    
    MyVectorInt data = m_data;
    int n = static_cast<int>(data.size());
    
    for (int i = 1; i < n; ++i) {
        int key = data[i];
        int j = i - 1;
        
        MyVectorInt indices5;
        indices5.push_back(i);
        MyVectorInt values5;
        values5.push_back(key);
        addAlgorithmStep(QString("处理元素 %1").arg(key), indices5, values5, VisualState::Current);
        
        while (j >= 0 && data[j] > key) {
            // 此时 key 所在的元素已经在 j+1 的位置（因为它是一路交换过来的）
            MyVectorInt indices6;
            indices6.push_back(j);
            indices6.push_back(j + 1);
            MyVectorInt values6;
            values6.push_back(data[j]);
            values6.push_back(key);
            addAlgorithmStep(QString("比较元素 %1 和 %2").arg(data[j]).arg(key), 
                           indices6, values6, VisualState::Current);
            
            // 移动操作：将data[j]向右移动一位（实际上是交换）
            MyVectorInt indices7;
            indices7.push_back(j);
            indices7.push_back(j + 1);
            MyVectorInt values7;
            values7.push_back(data[j]);
            values7.push_back(data[j + 1]);
            MyVectorInt newPos7;
            newPos7.push_back(j);
            newPos7.push_back(j + 1);
            addAlgorithmStep(QString("移动元素 %1 到位置 %2").arg(data[j]).arg(j + 1), 
                           indices7, values7, VisualState::Selected, 1000, true, newPos7);
                           
            int temp2 = data[j];
            data[j] = data[j + 1];
            data[j + 1] = temp2;
            j--;
        }
        // 此时 key 已经在 j+1 的位置了，无需再赋值或交换
    }
    
    addAlgorithmStep("插入排序完成", MyVectorInt(), MyVectorInt(), VisualState::Normal);
}

void SortModel::generateQuickSortSteps(MyVectorInt& data, int low, int high)
{
    if (low < high) {
        MyVectorInt indices8;
        indices8.push_back(low);
        indices8.push_back(high);
        MyVectorInt values8;
        values8.push_back(data[low]);
        values8.push_back(data[high]);
        addAlgorithmStep(QString("快速排序子数组 [%1, %2]").arg(low).arg(high), 
                       indices8, values8, VisualState::Current);
        
        int pivotIndex = partition(data, low, high);
        
        MyVectorInt indices9;
        indices9.push_back(pivotIndex);
        MyVectorInt values9;
        values9.push_back(data[pivotIndex]);
        addAlgorithmStep(QString("基准元素 %1 已归位").arg(data[pivotIndex]), 
                       indices9, values9, VisualState::Visited);
        
        generateQuickSortSteps(data, low, pivotIndex - 1);
        generateQuickSortSteps(data, pivotIndex + 1, high);
    }
}

void SortModel::generateMergeSortSteps(MyVectorInt& data, int left, int right)
{
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        MyVectorInt indices10;
        indices10.push_back(left);
        indices10.push_back(right);
        MyVectorInt values10;
        values10.push_back(data[left]);
        values10.push_back(data[right]);
        addAlgorithmStep(QString("归并排序子数组 [%1, %2]").arg(left).arg(right), 
                       indices10, values10, VisualState::Current);
        
        generateMergeSortSteps(data, left, mid);
        generateMergeSortSteps(data, mid + 1, right);
        merge(data, left, mid, right);
    }
}

void SortModel::generateHeapSortSteps()
{
    addAlgorithmStep("开始堆排序", MyVectorInt(), MyVectorInt(), VisualState::Current);
    
    // 使用副本进行模拟，不修改原始 m_data
    MyVectorInt data = m_data;
    int n = static_cast<int>(data.size());
    
    // 构建最大堆
    addAlgorithmStep("构建最大堆", MyVectorInt(), MyVectorInt(), VisualState::Current);
    buildHeap(data);
    
    // 逐个提取元素
    for (int i = n - 1; i > 0; --i) {
        MyVectorInt indices11;
        indices11.push_back(0);
        indices11.push_back(i);
        MyVectorInt values11;
        values11.push_back(data[0]);
        values11.push_back(data[i]);
        MyVectorInt newPos11;
        newPos11.push_back(0);
        newPos11.push_back(i);
        addAlgorithmStep(QString("交换根节点 %1 和最后一个元素 %2").arg(data[0]).arg(data[i]), 
                       indices11, values11, VisualState::Selected, 1000, true, newPos11);
        
        int temp3 = data[0];
        data[0] = data[i];
        data[i] = temp3;
        
        MyVectorInt indices12;
        indices12.push_back(0);
        MyVectorInt values12;
        values12.push_back(data[0]);
        addAlgorithmStep(QString("重新堆化，堆大小: %1").arg(i), 
                       indices12, values12, VisualState::Current);
        
        heapify(data, i, 0);
    }
    
    addAlgorithmStep("堆排序完成", MyVectorInt(), MyVectorInt(), VisualState::Normal);
}

void SortModel::generateRadixSortSteps()
{
    addAlgorithmStep("开始基数排序", MyVectorInt(), MyVectorInt(), VisualState::Current);
    
    // 使用副本进行模拟，不修改原始 m_data
    MyVectorInt data = m_data;
    
    // 找到最大元素（不使用std::max_element）
    int maxElement = data.empty() ? 0 : data[0];
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] > maxElement) {
            maxElement = data[i];
        }
    }
    
    for (int exp = 1; maxElement / exp > 0; exp *= 10) {
        addAlgorithmStep(QString("按第 %1 位排序").arg(exp), MyVectorInt(), MyVectorInt(), VisualState::Current);
        
        MyVectorInt output;
        output.resize(data.size(), 0);
        MyVectorInt count;
        count.resize(10, 0);
        
        // 计数阶段
        addAlgorithmStep("计数阶段：统计每个数字的出现次数", MyVectorInt(), MyVectorInt(), VisualState::Normal);
        for (size_t idx = 0; idx < data.size(); ++idx) {
            int value = data[idx];
            int digit = (value / exp) % 10;
            count[digit]++;
        }
        
        // 累加计数
        addAlgorithmStep("累加计数：计算每个数字的最终位置", MyVectorInt(), MyVectorInt(), VisualState::Normal);
        for (int i = 1; i < 10; ++i) {
            count[i] += count[i - 1];
        }
        
        // 构建输出数组（从后往前，保持稳定性）
        addAlgorithmStep("重新排列：根据计数数组放置元素", MyVectorInt(), MyVectorInt(), VisualState::Current);
        for (int i = static_cast<int>(data.size()) - 1; i >= 0; --i) {
            int digit = (data[i] / exp) % 10;
            int newPos = count[digit] - 1;
            
            if (i != newPos) {
                // 注意：这里是移动/覆盖操作，所以 isSwap = false
                MyVectorInt indices13;
                indices13.push_back(i);
                indices13.push_back(newPos);
                MyVectorInt values13;
                values13.push_back(data[i]);
                MyVectorInt newPos13;
                newPos13.push_back(i);
                newPos13.push_back(newPos);
                addAlgorithmStep(QString("移动元素 %1 到位置 %2 (第%3位是%4)").arg(data[i]).arg(newPos).arg(exp).arg(digit),
                                indices13, values13, VisualState::Selected, 1000, false, newPos13);
            }
            
            output[newPos] = data[i];
            count[digit]--;
        }
        
        data = output;
    }
    
    addAlgorithmStep("基数排序完成", MyVectorInt(), MyVectorInt(), VisualState::Normal);
}

void SortModel::resetAlgorithmState()
{
    m_visited.assign(m_data.size(), false);
    m_auxiliaryArray.clear();
    resetVisualization();
}

void SortModel::addAlgorithmStep(const QString &description, 
                               const MyVectorInt &indices,
                               const MyVectorInt &values,
                               VisualState state,
                               int delay,
                               bool isSwap,
                               const MyVectorInt &newPositions,
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

int SortModel::partition(MyVectorInt& data, int low, int high)
{
    int pivot = data[high];
    int i = low - 1;
    int currentPivotIndex = high;  // 跟踪基准元素的当前位置
    
    MyVectorInt indices14;
    indices14.push_back(high);
    MyVectorInt values14;
    values14.push_back(pivot);
    addAlgorithmStep(QString("选择基准元素: %1 (位置 %2)").arg(pivot).arg(high),
                    indices14, values14, VisualState::Selected, 1000, false, MyVectorInt(), high);
    
    for (int j = low; j < high; ++j) {
        MyVectorInt indices15;
        indices15.push_back(j);
        indices15.push_back(currentPivotIndex);
        MyVectorInt values15;
        values15.push_back(data[j]);
        values15.push_back(pivot);
        addAlgorithmStep(QString("比较元素 %1 和基准 %2").arg(data[j]).arg(pivot),
                        indices15, values15, VisualState::Current, 1000, false, MyVectorInt(), currentPivotIndex);
        
        if (data[j] <= pivot) {
            i++;
            if (i != j) {
                MyVectorInt indices16;
                indices16.push_back(i);
                indices16.push_back(j);
                MyVectorInt values16;
                values16.push_back(data[i]);
                values16.push_back(data[j]);
                MyVectorInt newPos16;
                newPos16.push_back(i);
                newPos16.push_back(j);
                addAlgorithmStep(QString("交换元素 %1 和 %2").arg(data[i]).arg(data[j]),
                                indices16, values16, VisualState::Selected, 1000, true, newPos16, currentPivotIndex);
                int temp4 = data[i];
                data[i] = data[j];
                data[j] = temp4;
            }
        }
    }
    
    // 基准元素归位
    if (i + 1 != high) {
        MyVectorInt indices17;
        indices17.push_back(i + 1);
        indices17.push_back(currentPivotIndex);
        MyVectorInt values17;
        values17.push_back(data[i + 1]);
        values17.push_back(pivot);
        MyVectorInt newPos17;
        newPos17.push_back(i + 1);
        newPos17.push_back(currentPivotIndex);
        addAlgorithmStep(QString("将基准元素 %1 放到正确位置 %2").arg(pivot).arg(i + 1),
                        indices17, values17, VisualState::Selected, 1000, true, newPos17, i + 1);
        int temp5 = data[i + 1];
        data[i + 1] = data[high];
        data[high] = temp5;
        currentPivotIndex = i + 1;  // 更新基准元素位置
    }
    
    return i + 1;
}

void SortModel::merge(MyVectorInt& data, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    MyVectorInt leftArray;
    leftArray.resize(n1, 0);
    MyVectorInt rightArray;
    rightArray.resize(n2, 0);
    
    for (int i = 0; i < n1; ++i) {
        leftArray[i] = data[left + i];
    }
    for (int j = 0; j < n2; ++j) {
        rightArray[j] = data[mid + 1 + j];
    }
    
    MyVectorInt indices18;
    indices18.push_back(left);
    indices18.push_back(mid);
    indices18.push_back(mid + 1);
    indices18.push_back(right);
    addAlgorithmStep(QString("归并两个有序子数组 [%1,%2] 和 [%3,%4]").arg(left).arg(mid).arg(mid+1).arg(right),
                    indices18, MyVectorInt(), VisualState::Current);
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        MyVectorInt indices19;
        indices19.push_back(left + i);
        indices19.push_back(mid + 1 + j);
        MyVectorInt values19;
        values19.push_back(leftArray[i]);
        values19.push_back(rightArray[j]);
        addAlgorithmStep(QString("比较 %1 和 %2").arg(leftArray[i]).arg(rightArray[j]),
                        indices19, values19, VisualState::Current);
        
        if (leftArray[i] <= rightArray[j]) {
            if (k != left + i) {
                MyVectorInt indices20;
                indices20.push_back(left + i);
                indices20.push_back(k);
                MyVectorInt values20;
                values20.push_back(leftArray[i]);
                MyVectorInt newPos20;
                newPos20.push_back(left + i);
                newPos20.push_back(k);
                addAlgorithmStep(QString("移动元素 %1 到位置 %2").arg(leftArray[i]).arg(k),
                                indices20, values20, VisualState::Selected, 1000, false, newPos20);
            }
            data[k] = leftArray[i];
            i++;
        } else {
            if (k != mid + 1 + j) {
                MyVectorInt indices21;
                indices21.push_back(mid + 1 + j);
                indices21.push_back(k);
                MyVectorInt values21;
                values21.push_back(rightArray[j]);
                MyVectorInt newPos21;
                newPos21.push_back(mid + 1 + j);
                newPos21.push_back(k);
                addAlgorithmStep(QString("移动元素 %1 到位置 %2").arg(rightArray[j]).arg(k),
                                indices21, values21, VisualState::Selected, 1000, false, newPos21);
            }
            data[k] = rightArray[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        if (k != left + i) {
            MyVectorInt indices22;
            indices22.push_back(left + i);
            indices22.push_back(k);
            MyVectorInt values22;
            values22.push_back(leftArray[i]);
            MyVectorInt newPos22;
            newPos22.push_back(left + i);
            newPos22.push_back(k);
            addAlgorithmStep(QString("移动剩余元素 %1 到位置 %2").arg(leftArray[i]).arg(k),
                            indices22, values22, VisualState::Selected, 1000, false, newPos22);
        }
        data[k] = leftArray[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        if (k != mid + 1 + j) {
            MyVectorInt indices23;
            indices23.push_back(mid + 1 + j);
            indices23.push_back(k);
            MyVectorInt values23;
            values23.push_back(rightArray[j]);
            MyVectorInt newPos23;
            newPos23.push_back(mid + 1 + j);
            newPos23.push_back(k);
            addAlgorithmStep(QString("移动剩余元素 %1 到位置 %2").arg(rightArray[j]).arg(k),
                            indices23, values23, VisualState::Selected, 1000, false, newPos23);
        }
        data[k] = rightArray[j];
        j++;
        k++;
    }
    
    addAlgorithmStep(QString("归并完成，子数组 [%1,%2] 已有序").arg(left).arg(right),
                    MyVectorInt(), MyVectorInt(), VisualState::Normal);
}

void SortModel::heapify(MyVectorInt& data, int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n) {
        MyVectorInt indices24;
        indices24.push_back(i);
        indices24.push_back(left);
        MyVectorInt values24;
        values24.push_back(data[i]);
        values24.push_back(data[left]);
        addAlgorithmStep(QString("比较节点 %1 和左子节点 %2").arg(data[i]).arg(data[left]),
                        indices24, values24, VisualState::Current);
        if (data[left] > data[largest]) {
            largest = left;
        }
    }
    
    if (right < n) {
        MyVectorInt indices25;
        indices25.push_back(largest);
        indices25.push_back(right);
        MyVectorInt values25;
        values25.push_back(data[largest]);
        values25.push_back(data[right]);
        addAlgorithmStep(QString("比较节点 %1 和右子节点 %2").arg(data[largest]).arg(data[right]),
                        indices25, values25, VisualState::Current);
        if (data[right] > data[largest]) {
            largest = right;
        }
    }
    
    if (largest != i) {
        MyVectorInt indices26;
        indices26.push_back(i);
        indices26.push_back(largest);
        MyVectorInt values26;
        values26.push_back(data[i]);
        values26.push_back(data[largest]);
        MyVectorInt newPos26;
        newPos26.push_back(i);
        newPos26.push_back(largest);
        addAlgorithmStep(QString("交换节点 %1 和 %2").arg(data[i]).arg(data[largest]),
                        indices26, values26, VisualState::Selected, 1000, true, newPos26);
        int temp6 = data[i];
        data[i] = data[largest];
        data[largest] = temp6;
        heapify(data, n, largest);
    }
}

void SortModel::buildHeap(MyVectorInt& data)
{
    int n = static_cast<int>(data.size());
    for (int i = n / 2 - 1; i >= 0; --i) {
        MyVectorInt indices27;
        indices27.push_back(i);
        MyVectorInt values27;
        values27.push_back(data[i]);
        addAlgorithmStep(QString("堆化节点 %1 (值: %2)").arg(i).arg(data[i]),
                        indices27, values27, VisualState::Current);
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
        int temp7 = m_data[index1];
        m_data[index1] = m_data[index2];
        m_data[index2] = temp7;
        
        // 2. 交换柱子对象 (视觉层 - 关键步骤！)
        // 这样 m_barItems[index1] 指向的就是刚才从右边飞过来的那个柱子了
        BarItem* tempBar = m_barItems[index1];
        m_barItems[index1] = m_barItems[index2];
        m_barItems[index2] = tempBar;
        
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
