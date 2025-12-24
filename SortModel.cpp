#include "SortModel.h"
#include "VisualItem.h"
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QTimer>
#include <QDebug>
#include <random>

SortModel::SortModel(QObject *parent)
    : QObject(parent)
    , m_scene(nullptr)
    , m_mergeUnderline(nullptr)
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
    // 如果算法正在运行，先停止算法和定时器
    if (m_algorithmRunning) {
        m_algorithmRunning = false;
        m_algorithmPaused = false;
        m_algorithmTimer->stop();  // 立即停止定时器
        // 清空算法步骤队列，避免访问已删除的项
        while (!m_algorithmSteps.empty()) {
            m_algorithmSteps.pop();
        }
        // 重置可视化状态（此时 m_barItems 还存在）
        resetVisualization();
        emit algorithmStopped();
    } else {
        // 即使算法未运行，也清空步骤队列
        while (!m_algorithmSteps.empty()) {
            m_algorithmSteps.pop();
        }
    }
    
    m_data = data;
    m_originalData = data;
    updateBarItems();  // 这会清空旧的 m_barItems 并创建新的
    emit dataChanged();
}

void SortModel::setRandomData(int size, int minValue, int maxValue)
{
    // 如果算法正在运行，先停止算法和定时器
    if (m_algorithmRunning) {
        m_algorithmRunning = false;
        m_algorithmPaused = false;
        m_algorithmTimer->stop();  // 立即停止定时器
        // 清空算法步骤队列，避免访问已删除的项
        while (!m_algorithmSteps.empty()) {
            m_algorithmSteps.pop();
        }
        // 重置可视化状态（此时 m_barItems 还存在）
        resetVisualization();
        emit algorithmStopped();
    } else {
        // 即使算法未运行，也清空步骤队列
        while (!m_algorithmSteps.empty()) {
            m_algorithmSteps.pop();
        }
    }
    
    std::uniform_int_distribution<> dis(minValue, maxValue);
    m_data.clear();
    m_data.reserve(size);
    
    for (int i = 0; i < size; ++i) {
        m_data.push_back(dis(m_gen));
    }
    
    m_originalData = m_data;
    updateBarItems();  // 这会清空旧的 m_barItems 并创建新的
    emit dataChanged();
}

void SortModel::clearData()
{
    // 如果算法正在运行，先停止算法
    if (m_algorithmRunning) {
        stopAlgorithm();
    }
    
    // 清空算法步骤队列，避免访问已删除的项
    while (!m_algorithmSteps.empty()) {
        m_algorithmSteps.pop();
    }
    
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
        // 数据为空，不执行算法（提示信息已在控制器层显示）
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
        m_algorithmTimer->start(1000); // 1秒后开始第一步
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
    // 添加安全检查，防止访问已删除的项
    for (size_t i = 0; i < m_barItems.size(); ++i) {
        BarItem *bar = m_barItems[i];
        if (bar) {
            bar->setVisualState(VisualState::Normal);
            bar->stopAnimations();
        }
    }
    hideMergeUnderline();
}

void SortModel::resetData()
{
    m_data = m_originalData;
    updateBarItems();
    resetVisualization();
}

void SortModel::processNextStep()
{
    // 检查算法是否仍在运行，如果数据已被清除，停止算法
    // 必须在函数开始就检查，防止后续访问已删除的 m_barItems
    if (!m_algorithmRunning || m_barItems.empty() || m_data.empty()) {
        if (m_algorithmRunning) {
            m_algorithmRunning = false;
            m_algorithmPaused = false;
            m_algorithmTimer->stop();
            // 清空剩余的步骤
            while (!m_algorithmSteps.empty()) {
                m_algorithmSteps.pop();
            }
            emit algorithmStopped();
        }
        return;
    }
    
    if (m_algorithmSteps.empty()) {
        m_algorithmRunning = false;
        m_algorithmPaused = false;
        emit algorithmCompleted(m_currentAlgorithm);
        return;
    }
    
    SortStep step = m_algorithmSteps.front();
    m_algorithmSteps.pop();
    
    // 再次检查 m_barItems 是否为空（防止在步骤处理过程中被清除）
    if (m_barItems.empty() || m_data.empty()) {
        m_algorithmRunning = false;
        m_algorithmPaused = false;
        m_algorithmTimer->stop();
        while (!m_algorithmSteps.empty()) {
            m_algorithmSteps.pop();
        }
        emit algorithmStopped();
        return;
    }
    
    // 重置所有非 Visited 的柱子状态为 Normal，避免颜色残留
    // 但保留当前步骤的基准元素（如果有）的 Selected 状态
    for (size_t i = 0; i < m_barItems.size(); ++i) {
        BarItem *bar = m_barItems[i];
        if (bar && bar->getVisualState() != VisualState::Visited) {
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
            if (bar) {
                bar->setVisualState(step.state);
                
                if (step.state == VisualState::Current) {
                    bar->startCompareAnimation();
                } else if (step.state == VisualState::Selected) {
                    bar->startSelectAnimation();
                }
            }
        }
    }
    
    // 特殊处理：如果有基准元素，将其始终保持为 Selected (红色) 状态
    if (step.pivotIndex >= 0 && step.pivotIndex < static_cast<int>(m_barItems.size())) {
        BarItem *pivotBar = m_barItems[step.pivotIndex];
        if (pivotBar) {
            pivotBar->setVisualState(VisualState::Selected);
        }
    }
    
    // 处理归并区域下划线（仅归并排序）
    if (m_currentAlgorithm == SortAlgorithm::MergeSort && step.mergeLeft >= 0 && step.mergeRight >= 0) {
        updateMergeUnderline(step.mergeLeft, step.mergeRight);
    } else {
        // 如果不是归并排序或没有归并区域信息，隐藏下划线
        hideMergeUnderline();
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
        // 使用速度设置，而不是步骤中的固定延迟
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
                               int pivotIndex,
                               int mergeLeft,
                               int mergeRight)
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
    step.mergeLeft = mergeLeft;
    step.mergeRight = mergeRight;
    
    m_algorithmSteps.push(step);
}

int SortModel::partition(MyVectorInt& data, int low, int high)
{
    // 选择中间位置的元素作为基准
    int mid = low + (high - low) / 2;
    int pivot = data[mid];
    int currentPivotIndex = mid;  // 跟踪基准元素的当前位置
    
    // 显示选择中间元素作为基准
    MyVectorInt indices14;
    indices14.push_back(mid);
    MyVectorInt values14;
    values14.push_back(pivot);
    addAlgorithmStep(QString("选择中间位置元素作为基准: %1 (位置 %2)").arg(pivot).arg(mid),
                    indices14, values14, VisualState::Selected, 1000, false, MyVectorInt(), mid);
    
    // 将中间元素与最后一个元素交换，以便使用标准的分区逻辑
    if (mid != high) {
        MyVectorInt indicesSwap;
        indicesSwap.push_back(mid);
        indicesSwap.push_back(high);
        MyVectorInt valuesSwap;
        valuesSwap.push_back(data[mid]);
        valuesSwap.push_back(data[high]);
        MyVectorInt newPosSwap;
        newPosSwap.push_back(high);
        newPosSwap.push_back(mid);
        addAlgorithmStep(QString("将基准元素 %1 移到末尾位置 %2").arg(pivot).arg(high),
                        indicesSwap, valuesSwap, VisualState::Selected, 1000, true, newPosSwap, high);
        int temp = data[mid];
        data[mid] = data[high];
        data[high] = temp;
        currentPivotIndex = high;  // 更新基准元素位置
    }
    
    int i = low - 1;
    
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
                    indices18, MyVectorInt(), VisualState::Current, 1000, false, MyVectorInt(), -1, left, right);
    
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


void SortModel::updateBarItems()
{
    clearBarItems();
    
    // 根据数据数量动态调整柱状图间距和场景大小
    if (m_data.size() > 0 && m_scene) {
        // 计算所需的总宽度（留一些边距）
        qreal totalWidth = m_data.size() * m_barSpacing;
        qreal minSpacing = 20.0;  // 最小间距
        qreal maxSpacing = 40.0;  // 最大间距
        
        // 如果数据太多，减小间距以适应场景
        // 假设场景可用宽度约为1000（从场景矩形800 + 边距）
        qreal availableWidth = 1000.0;
        if (totalWidth > availableWidth) {
            m_barSpacing = qMax(minSpacing, availableWidth / m_data.size());
        } else {
            m_barSpacing = maxSpacing;  // 使用默认间距
        }
        
        // 更新场景矩形以适应所有柱状图
        qreal sceneWidth = qMax(800.0, totalWidth + 200);  // 至少800，加上边距
        qreal sceneHeight = 400.0;  // 保持高度不变
        m_scene->setSceneRect(-100, -100, sceneWidth, sceneHeight);
    }
    
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

void SortModel::updateMergeUnderline(int left, int right)
{
    if (!m_scene || left < 0 || right < 0 || left > right || 
        left >= static_cast<int>(m_barItems.size()) || right >= static_cast<int>(m_barItems.size())) {
        return;
    }
    
    // 删除旧的下划线
    if (m_mergeUnderline) {
        m_scene->removeItem(m_mergeUnderline);
        delete m_mergeUnderline;
        m_mergeUnderline = nullptr;
    }
    
    // 计算下划线的位置
    QPointF leftPos = calculateBarPosition(left);
    QPointF rightPos = calculateBarPosition(right);
    
    // 下划线位于柱子底部下方10像素
    qreal y = 10.0;  // 柱子底部是y=0，下划线在下方10像素
    qreal x1 = leftPos.x() - m_barWidth / 2;
    qreal x2 = rightPos.x() + m_barWidth / 2;
    
    // 创建下划线
    m_mergeUnderline = new QGraphicsLineItem(x1, y, x2, y);
    m_mergeUnderline->setPen(QPen(QColor(255, 0, 0), 2));  // 红色，2像素宽
    m_scene->addItem(m_mergeUnderline);
    m_mergeUnderline->setZValue(100);  // 确保下划线在最上层
}

void SortModel::hideMergeUnderline()
{
    if (m_mergeUnderline && m_scene) {
        m_scene->removeItem(m_mergeUnderline);
        delete m_mergeUnderline;
        m_mergeUnderline = nullptr;
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
