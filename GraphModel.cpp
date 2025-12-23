#include "GraphModel.h"
#include "VisualItem.h"
#include "MyMapQStringToInt.h"
#include "MyMapQStringToQString.h"
#include "MyMapQStringToQPointF.h"
#include "MyMapQStringToVertexItemPtr.h"
#include "MyMapPairToEdgeItemPtr.h"
#include "MyPairQStringQString.h"
#include "MySetQString.h"
#include "MySetPairQStringQString.h"
#include "MyMapQStringToSetQString.h"
#include "MyQueueQString.h"
#include "MyStackQString.h"
#include "MyVectorPairQStringQString.h"
#include "MyVectorEdgeInfo.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
#include <QtMath>
#include <random>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QStringConverter>
#include <climits>

GraphModel::GraphModel(QObject *parent)
    : QObject(parent)
    , m_scene(nullptr)
    , m_isDirected(true)
    , m_currentAlgorithm(GraphAlgorithm::DFS)
    , m_algorithmRunning(false)
    , m_algorithmPaused(false)
    , m_algorithmTimer(new QTimer(this))
{
    connect(m_algorithmTimer, &QTimer::timeout, this, &GraphModel::processNextStep);
    m_algorithmTimer->setSingleShot(true);
}

GraphModel::~GraphModel()
{
    clearGraph();
}

bool GraphModel::addVertex(const QString &label, const QPointF &position)
{
    if (m_vertices.find(label) != nullptr) {
        qDebug() << "Vertex" << label << "already exists";
        return false;
    }
    
    VertexItem *vertex = new VertexItem(label, position);
    m_vertices[label] = vertex;
    m_adjacencyList[label] = MySetQString();
    
    if (m_scene) {
        m_scene->addItem(vertex);
        vertex->startInsertAnimation();
    }
    
    emit vertexAdded(label);
    return true;
}

bool GraphModel::removeVertex(const QString &label)
{
    VertexItem** ptr = m_vertices.find(label);
    if (ptr == nullptr) {
        qDebug() << "Vertex" << label << "does not exist";
        return false;
    }
    
    VertexItem *vertex = *ptr;
    
    // 删除所有相关的边
    MyVectorPairQStringQString edgesToRemove;
    for (size_t i = 0; i < m_edges.size(); ++i) {
        const QString &from = m_edges.keyAt(i).first;
        const QString &to = m_edges.keyAt(i).second;
        if (from == label || to == label) {
            edgesToRemove.push_back(MyPairQStringQString(from, to));
        }
    }
    
    for (size_t i = 0; i < edgesToRemove.size(); ++i) {
        const MyPairQStringQString &edgePair = edgesToRemove.at(i);
        removeEdge(edgePair.first, edgePair.second);
    }
    
    // 删除顶点
    m_vertices.erase(label);
    m_adjacencyList.erase(label);
    
    if (m_scene) {
        vertex->startDeleteAnimation();
    } else {
        delete vertex;
    }
    
    emit vertexRemoved(label);
    return true;
}

bool GraphModel::addEdge(const QString &from, const QString &to, int weight)
{
    if (m_vertices.find(from) == nullptr || 
        m_vertices.find(to) == nullptr) {
        qDebug() << "One or both vertices do not exist";
        return false;
    }
    
    MyPairQStringQString edgeKey(from, to);
    if (m_edges.find(edgeKey) != nullptr) {
        qDebug() << "Edge already exists";
        return false;
    }
    
    VertexItem *fromVertex = m_vertices[from];
    VertexItem *toVertex = m_vertices[to];
    
    EdgeItem *edge = new EdgeItem(fromVertex, toVertex, weight, m_isDirected);
    m_edges[edgeKey] = edge;
    m_adjacencyList[from].insert(to);
    
    if (!m_isDirected) {
        m_adjacencyList[to].insert(from);
        MyPairQStringQString reverseKey(to, from);
        m_edges[reverseKey] = edge; // 无向图共享边对象
    }
    
    if (m_scene) {
        m_scene->addItem(edge);
    }
    
    emit edgeAdded(from, to, weight);
    return true;
}

bool GraphModel::removeEdge(const QString &from, const QString &to)
{
    MyPairQStringQString edgeKey(from, to);
    EdgeItem** ptr = m_edges.find(edgeKey);
    if (ptr == nullptr) {
        qDebug() << "Edge does not exist";
        return false;
    }
    
    EdgeItem *edge = *ptr;
    m_edges.erase(edgeKey);
    m_adjacencyList[from].erase(to);
    
    if (!m_isDirected) {
        MyPairQStringQString reverseKey(to, from);
        m_edges.erase(reverseKey);
        m_adjacencyList[to].erase(from);
    }
    
    if (m_scene) {
        m_scene->removeItem(edge);
        delete edge;
    }
    
    emit edgeRemoved(from, to);
    return true;
}

VertexItem* GraphModel::getVertex(const QString &label) const
{
    VertexItem* const* ptr = m_vertices.find(label);
    return (ptr != nullptr) ? *ptr : nullptr;
}

EdgeItem* GraphModel::getEdge(const QString &from, const QString &to) const
{
    MyPairQStringQString edgeKey(from, to);
    EdgeItem* const* ptr = m_edges.find(edgeKey);
    return (ptr != nullptr) ? *ptr : nullptr;
}

MyVectorVertexItemPtr GraphModel::getAllVertices() const
{
    MyVectorVertexItemPtr vertices;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        vertices.push_back(m_vertices.valueAt(i));
    }
    return vertices;
}

MyVectorEdgeItemPtr GraphModel::getAllEdges() const
{
    MyVectorEdgeItemPtr edges;
    for (size_t i = 0; i < m_edges.size(); ++i) {
        edges.push_back(m_edges.valueAt(i));
    }
    return edges;
}

int GraphModel::getVertexCount() const
{
    return static_cast<int>(m_vertices.size());
}

int GraphModel::getEdgeCount() const
{
    return static_cast<int>(m_edges.size());
}

bool GraphModel::isEmpty() const
{
    return m_vertices.empty();
}

void GraphModel::setDirected(bool directed)
{
    if (m_isDirected != directed) {
        m_isDirected = directed;
        // 重新构建边结构以适应有向/无向图
        // 这里可以添加更复杂的转换逻辑
    }
}

void GraphModel::executeAlgorithm(GraphAlgorithm algorithm, const QString &startVertex)
{
    if (m_algorithmRunning) {
        stopAlgorithm();
    }
    
    m_currentAlgorithm = algorithm;
    m_startVertex = startVertex;
    m_algorithmRunning = true;
    m_algorithmPaused = false;
    
    resetAlgorithmState();
    
    // 生成算法步骤
    switch (algorithm) {
        case GraphAlgorithm::DFS:
            generateDFSSteps(startVertex);
            break;
        case GraphAlgorithm::BFS:
            generateBFSSteps(startVertex);
            break;
        case GraphAlgorithm::Dijkstra:
            generateDijkstraSteps(startVertex);
            break;
        case GraphAlgorithm::Prim:
            generatePrimSteps(startVertex);
            break;
        case GraphAlgorithm::Kruskal:
            generateKruskalSteps();
            break;
        case GraphAlgorithm::TopologicalSort:
            generateTopologicalSortSteps();
            break;
    }
    
    emit algorithmStarted(algorithm);
    
    if (!m_algorithmSteps.empty()) {
        m_algorithmTimer->start(1000); // 1秒后开始第一步
    }
}

void GraphModel::pauseAlgorithm()
{
    if (m_algorithmRunning && !m_algorithmPaused) {
        m_algorithmPaused = true;
        m_algorithmTimer->stop();
        emit algorithmPaused();
    }
}

void GraphModel::resumeAlgorithm()
{
    if (m_algorithmRunning && m_algorithmPaused) {
        m_algorithmPaused = false;
        m_algorithmTimer->start(500); // 500ms后继续
        emit algorithmResumed();
    }
}

void GraphModel::stopAlgorithm()
{
    if (m_algorithmRunning) {
        m_algorithmRunning = false;
        m_algorithmPaused = false;
        m_algorithmTimer->stop();
        
        resetVisualization();
        emit algorithmStopped();
    }
}

void GraphModel::stepAlgorithm()
{
    if (m_algorithmRunning && m_algorithmPaused && !m_algorithmSteps.empty()) {
        processNextStep();
    }
}

void GraphModel::setScene(QGraphicsScene *scene)
{
    m_scene = scene;
    
    // 将现有项目添加到场景
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        scene->addItem(m_vertices.valueAt(i));
    }
    for (size_t i = 0; i < m_edges.size(); ++i) {
        scene->addItem(m_edges.valueAt(i));
    }
}

void GraphModel::applyCircularLayout()
{
    if (m_vertices.empty()) return;
    
    int index = 0;
    int total = static_cast<int>(m_vertices.size());
    qreal radius = qMin(200.0, total * 20.0);
    
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF position = calculateCircularPosition(index, total, radius);
        m_vertices.valueAt(i)->setPosition(position);
        index++;
    }
    
    // 更新边的位置
    for (size_t i = 0; i < m_edges.size(); ++i) {
        m_edges.valueAt(i)->updatePosition();
    }
}

void GraphModel::applyForceDirectedLayout()
{
    // 简化的力导向布局算法
    const int iterations = 100;
    
    for (int iter = 0; iter < iterations; ++iter) {
        applyForceDirectedStep();
    }
}

void GraphModel::applyGridLayout(int columns)
{
    if (m_vertices.empty()) return;
    
    if (columns <= 0) {
        columns = static_cast<int>(qSqrt(m_vertices.size()));
    }
    
    int row = 0, col = 0;
    const qreal spacing = 100.0;
    
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF position(col * spacing, row * spacing);
        m_vertices.valueAt(i)->setPosition(position);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    // 更新边的位置
    for (size_t i = 0; i < m_edges.size(); ++i) {
        m_edges.valueAt(i)->updatePosition();
    }
}

void GraphModel::resetVisualization()
{
    // 重置所有顶点的可视化状态
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        m_vertices.valueAt(i)->setVisualState(VisualState::Normal);
        m_vertices.valueAt(i)->stopAnimations();
    }
    
    // 重置所有边的可视化状态
    for (size_t i = 0; i < m_edges.size(); ++i) {
        m_edges.valueAt(i)->setVisualState(VisualState::Normal);
        m_edges.valueAt(i)->stopAnimations();
    }
}

void GraphModel::clearGraph()
{
    stopAlgorithm();
    
    // 清空算法步骤
    while (!m_algorithmSteps.empty()) {
        m_algorithmSteps.pop();
    }
    
    // 删除所有边
    for (size_t i = 0; i < m_edges.size(); ++i) {
        if (m_scene) {
            m_scene->removeItem(m_edges.valueAt(i));
        }
        delete m_edges.valueAt(i);
    }
    m_edges.clear();
    
    // 删除所有顶点
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        if (m_scene) {
            m_scene->removeItem(m_vertices.valueAt(i));
        }
        delete m_vertices.valueAt(i);
    }
    m_vertices.clear();
    m_adjacencyList.clear();
}

void GraphModel::processNextStep()
{
    if (m_algorithmSteps.empty()) {
        m_algorithmRunning = false;
        emit algorithmCompleted(m_currentAlgorithm);
        return;
    }
    
    AlgorithmStep step = m_algorithmSteps.front();
    m_algorithmSteps.pop();
    
    // 应用可视化状态
    for (size_t i = 0; i < step.vertices.size(); ++i) {
        const QString &vertexLabel = step.vertices.at(i);
        VertexItem *vertex = getVertex(vertexLabel);
        if (vertex) {
            vertex->setVisualState(step.state);
            if (step.state == VisualState::Current) {
                vertex->startPulseAnimation();
            }
        }
    }
    
    for (size_t i = 0; i < step.edges.size(); ++i) {
        const MyPairQStringQString &edgePair = step.edges.at(i);
        EdgeItem *edge = getEdge(edgePair.first, edgePair.second);
        if (edge) {
            edge->setVisualState(step.state);
            if (step.state == VisualState::Current) {
                edge->startTraverseAnimation();
            }
        }
    }
    
    emit algorithmStepCompleted(step);
    
    // 继续下一步
    if (!m_algorithmSteps.empty() && m_algorithmRunning && !m_algorithmPaused) {
        m_algorithmTimer->start(step.delay);
    }
}

void GraphModel::generateDFSSteps(const QString &startVertex)
{
    if (m_vertices.find(startVertex) == nullptr) {
        return;
    }
    MyVectorQString startVertices1;
    startVertices1.push_back(startVertex);
    addAlgorithmStep(QString("开始深度优先搜索，从顶点 %1 开始").arg(startVertex), 
                    startVertices1, MyVectorPairQStringQString(), VisualState::Current);
    MyStackQString stack;
    MySetQString visited;
    MySetQString inStack;  // 记录已在栈中的顶点，避免重复入栈
    
    stack.push(startVertex);
    inStack.insert(startVertex);
    
    while(!stack.empty()){
        QString current = stack.top();
        stack.pop();
        inStack.erase(current);  // 从栈中移除时，清除标记
        
        // 如果该元素已访问，则跳过
        if(visited.find(current)){
            continue;
        }
        
        // 访问该元素
        MyVectorQString currentVec1;
        currentVec1.push_back(current);
        addAlgorithmStep(QString("访问顶点 %1").arg(current), 
                        currentVec1, MyVectorPairQStringQString(), VisualState::Visited);
        visited.insert(current);
        
        // 获取所有邻居
        MyVectorQString neighbors = getNeighbors(current);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            const QString &neighbor = neighbors.at(i);
            // 如果邻居未访问且不在栈中，则加入栈
            if(!visited.find(neighbor) &&
               !inStack.find(neighbor)){
                stack.push(neighbor);
                inStack.insert(neighbor);  // 标记已入栈
                // 只把新发现的邻居设为Current状态，current保持Visited状态
                MyVectorQString neighborVec1;
                neighborVec1.push_back(neighbor);
                MyVectorPairQStringQString edgeVec1;
                edgeVec1.push_back(MyPairQStringQString(current, neighbor));
                addAlgorithmStep(QString("发现未访问的邻居 %1，加入栈中").arg(neighbor), 
                                neighborVec1, edgeVec1, VisualState::Current);
            }
        }
    }
    addAlgorithmStep("深度优先搜索完成", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
    
    /*
    如果起始点不存在退出
    建立空栈
    建立空集合
    起始点入栈
    while(栈非空){
        栈顶元素出栈
        如果该元素已访问，则继续
        访问该元素
        标记该元素已访问
        找一个邻居，如果找到且未访问，则加入栈
    }
    */
}

void GraphModel::generateBFSSteps(const QString &startVertex)
{
    if (m_vertices.find(startVertex) == nullptr) {
        return;
    }
    
    MyVectorQString startVertices2;
    startVertices2.push_back(startVertex);
    addAlgorithmStep(QString("开始广度优先搜索，从顶点 %1 开始").arg(startVertex), 
                    startVertices2, MyVectorPairQStringQString(), VisualState::Current);
    
    MyQueueQString queue;
    MySetQString visited;
    
    queue.push(startVertex);
    visited.insert(startVertex);
    
    while (!queue.empty()) {
        QString current = queue.front();
        queue.pop();
        
        MyVectorQString currentVec2;
        currentVec2.push_back(current);
        addAlgorithmStep(QString("处理顶点 %1").arg(current), 
                        currentVec2, MyVectorPairQStringQString(), VisualState::Visited);
        
        MyVectorQString neighbors = getNeighbors(current);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            const QString &neighbor = neighbors.at(i);
            if (!visited.find(neighbor)) {
                visited.insert(neighbor);
                // 只把新发现的邻居设为Current状态，current保持Visited状态
                MyVectorQString neighborVec2;
                neighborVec2.push_back(neighbor);
                MyVectorPairQStringQString edgeVec2;
                edgeVec2.push_back(MyPairQStringQString(current, neighbor));
                addAlgorithmStep(QString("发现未访问的邻居 %1，加入队列").arg(neighbor), 
                                neighborVec2, edgeVec2, VisualState::Current);
                queue.push(neighbor);
            }
        }
    }
    
    addAlgorithmStep("广度优先搜索完成", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
}

void GraphModel::generateDijkstraSteps(const QString &startVertex)
{
    if (m_vertices.find(startVertex) == nullptr) {
        return;
    }
    
    MyVectorQString startVertices3;
    startVertices3.push_back(startVertex);
    addAlgorithmStep(QString("开始Dijkstra最短路径算法，从顶点 %1 开始").arg(startVertex), 
                    startVertices3, MyVectorPairQStringQString(), VisualState::Visited);
    
    // 距离映射：顶点 -> 最短距离
    MyMapQStringToInt distances;
    // 前驱映射：顶点 -> 前驱顶点
    MyMapQStringToQString predecessors;
    // 未访问顶点集合
    MySetQString unvisited;
    
    // 初始化：所有顶点距离为无穷大，起始顶点距离为0
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        distances[m_vertices.keyAt(i)] = INT_MAX;
        unvisited.insert(m_vertices.keyAt(i));
    }
    distances[startVertex] = 0;
    
    while (!unvisited.empty()) {
        // 找到未访问顶点中距离最小的
        QString current;
        int minDist = INT_MAX;
        for (size_t i = 0; i < unvisited.size(); ++i) {
            const QString &v = unvisited.at(i);
            if (distances[v] < minDist) {
                minDist = distances[v];
                current = v;
            }
        }
        
        // 如果所有未访问顶点距离都是无穷大，说明图不连通
        if (minDist == INT_MAX) {
            break;
        }
        
        unvisited.erase(current);
        
        if (current != startVertex) {
            MyVectorQString currentVec3;
            currentVec3.push_back(current);
            addAlgorithmStep(QString("选择距离最小的未访问顶点 %1 (距离: %2)").arg(current).arg(minDist),
                            currentVec3, MyVectorPairQStringQString(), VisualState::Visited);
        }
        
        // 更新邻居的距离
        MyVectorQString neighbors = getNeighbors(current);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            const QString &neighbor = neighbors.at(i);
            if (!unvisited.find(neighbor)) {
                continue;  // 已访问，跳过
            }
            
            EdgeItem *edge = getEdge(current, neighbor);
            if (!edge) {
                // 无向图，尝试反向边
                edge = getEdge(neighbor, current);
            }
            
            if (edge) {
                int weight = edge->getWeight();
                int newDist = distances[current] + weight;
                
                if (newDist < distances[neighbor]) {
                    QString prevPred = predecessors.find(neighbor) != nullptr ? *predecessors.find(neighbor) : "无";
                    predecessors[neighbor] = current;  // 更新前驱顶点
                    addAlgorithmStep(QString("发现更短路径到 %1：%2 -> %3 (新距离: %4，前驱: %2)").arg(neighbor)
                                    .arg(current).arg(neighbor).arg(newDist),
                                    MyVectorQString(), MyVectorPairQStringQString(), VisualState::Current);
                    distances[neighbor] = newDist;
                } else {
                    addAlgorithmStep(QString("检查到 %1 的路径：%2 -> %3 (距离: %4，不更新)").arg(neighbor)
                                    .arg(current).arg(neighbor).arg(distances[neighbor]),
                                    MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
                }
            }
        }
        
        if (current != startVertex) {
            MyVectorQString currentVec4;
            currentVec4.push_back(current);
            addAlgorithmStep(QString("顶点 %1 已处理完成，距离: %2").arg(current).arg(distances[current]),
                            currentVec4, MyVectorPairQStringQString(), VisualState::Visited);
        }
    }
    
    // 显示最短路径结果
    addAlgorithmStep("Dijkstra算法完成，开始显示最短路径结果", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
    
    // 为每个可达顶点显示最短路径
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QString &target = m_vertices.keyAt(i);
        if (target == startVertex) {
            continue;  // 跳过起始顶点
        }
        
        if (distances[target] == INT_MAX) {
            MyVectorQString targetVec;
            targetVec.push_back(target);
            addAlgorithmStep(QString("顶点 %1 不可达").arg(target), targetVec, MyVectorPairQStringQString(), VisualState::Normal);
            continue;
        }
        
        // 回溯路径
        MyVectorQString path;
        QString current = target;
        while (current != startVertex && predecessors.find(current) != nullptr) {
            path.push_back(current);
            current = predecessors[current];
        }
        path.push_back(startVertex);
        // 手动反转路径（因为 MyVectorQString 没有 reverse 方法）
        for (size_t i = 0; i < path.size() / 2; ++i) {
            QString temp = path[i];
            path[i] = path[path.size() - 1 - i];
            path[path.size() - 1 - i] = temp;
        }
        
        // 构建路径字符串
        QString pathStr;
        for (size_t i = 0; i < path.size(); i++) {
            if (i > 0) pathStr += " -> ";
            pathStr += path[i];
        }
        
        // 构建边列表用于高亮显示
        MyVectorPairQStringQString pathEdges;
        for (size_t i = 0; i < path.size() - 1; i++) {
            pathEdges.push_back(MyPairQStringQString(path[i], path[i + 1]));
        }
        
        addAlgorithmStep(QString("到顶点 %1 的最短路径：%2 (距离: %3)").arg(target).arg(pathStr).arg(distances[target]),
                        path, pathEdges, VisualState::Selected);
    }
    
    addAlgorithmStep("所有最短路径显示完成", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
}

void GraphModel::generatePrimSteps(const QString &startVertex)
{
    if (m_vertices.empty()) {
        return;
    }
    
    // 确定起始顶点：如果指定了且存在则使用，否则使用第一个顶点
    QString actualStartVertex = startVertex;
    if (actualStartVertex.isEmpty() || m_vertices.find(actualStartVertex) == nullptr) {
        if (m_vertices.size() > 0) {
            actualStartVertex = m_vertices.keyAt(0);
        }
        if (!startVertex.isEmpty()) {
            MyVectorQString actualStartVec1;
            actualStartVec1.push_back(actualStartVertex);
            addAlgorithmStep(QString("指定的起始顶点 %1 不存在，使用顶点 %2 作为起始点").arg(startVertex).arg(actualStartVertex),
                            actualStartVec1, MyVectorPairQStringQString(), VisualState::Current);
        }
    }
    
    MyVectorQString actualStartVec2;
    actualStartVec2.push_back(actualStartVertex);
    addAlgorithmStep(QString("开始Prim最小生成树算法，起始顶点: %1").arg(actualStartVertex), 
                    actualStartVec2, MyVectorPairQStringQString(), VisualState::Current);
    
    // 已加入最小生成树的顶点集合
    MySetQString mstVertices;
    // 已加入最小生成树的边集合
    MySetPairQStringQString mstEdges;
    
    // 使用指定的起始顶点
    mstVertices.insert(actualStartVertex);
    
    MyVectorQString startVec4;
    startVec4.push_back(startVertex);
    addAlgorithmStep(QString("选择起始顶点 %1 加入最小生成树").arg(startVertex),
                    startVec4, MyVectorPairQStringQString(), VisualState::Current);
    
    while (mstVertices.size() < m_vertices.size()) {
        // 找到连接已访问顶点和未访问顶点的最小权重边
        QString minFrom, minTo;
        int minWeight = INT_MAX;
        
        for (size_t j = 0; j < mstVertices.size(); ++j) {
            const QString &v = mstVertices.at(j);
            MyVectorQString neighbors = getNeighbors(v);
            for (size_t i = 0; i < neighbors.size(); ++i) {
                const QString &neighbor = neighbors.at(i);
                if (mstVertices.find(neighbor)) {
                    continue;  // 邻居已在MST中，跳过
                }
                
                EdgeItem *edge = getEdge(v, neighbor);
                if (!edge && !m_isDirected) {
                    edge = getEdge(neighbor, v);
                }
                
                if (edge) {
                    int weight = edge->getWeight();
                    if (weight < minWeight) {
                        minWeight = weight;
                        minFrom = v;
                        minTo = neighbor;
                    }
                }
            }
        }
        
        if (minWeight == INT_MAX) {
            // 图不连通，无法继续
            addAlgorithmStep("图不连通，无法生成完整的最小生成树", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
            break;
        }
        
        // 将找到的最小边加入MST
        mstVertices.insert(minTo);
        mstEdges.insert(MyPairQStringQString(minFrom, minTo));
        
        MyVectorQString minVec;
        minVec.push_back(minFrom);
        minVec.push_back(minTo);
        MyVectorPairQStringQString minEdgeVec;
        minEdgeVec.push_back(MyPairQStringQString(minFrom, minTo));
        addAlgorithmStep(QString("找到最小权重边：%1 -> %2 (权重: %3)，加入最小生成树").arg(minFrom).arg(minTo).arg(minWeight),
                        minVec, minEdgeVec, VisualState::Selected);
        
        MyVectorQString minToVec;
        minToVec.push_back(minTo);
        addAlgorithmStep(QString("顶点 %1 已加入最小生成树").arg(minTo),
                        minToVec, MyVectorPairQStringQString(), VisualState::Selected);
    }
    
    addAlgorithmStep(QString("Prim算法完成，最小生成树包含 %1 个顶点和 %2 条边").arg(mstVertices.size()).arg(mstEdges.size()),
                    MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
}

void GraphModel::generateKruskalSteps()
{
    if (m_vertices.empty()) {
        return;
    }
    
    addAlgorithmStep("开始Kruskal最小生成树算法", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Current);
    
    // 收集所有边并按权重排序
    MyVectorEdgeInfo allEdges;
    for (size_t i = 0; i < m_edges.size(); ++i) {
        EdgeItem *edge = m_edges.valueAt(i);
        if (edge) {
            EdgeInfo edgeInfo(m_edges.keyAt(i).first, m_edges.keyAt(i).second, edge->getWeight());
            allEdges.push_back(edgeInfo);
        }
    }
    
    // 按权重排序（使用冒泡排序）
    for (size_t i = 0; i < allEdges.size(); ++i) {
        for (size_t j = 0; j < allEdges.size() - 1 - i; ++j) {
            if (allEdges[j + 1].weight < allEdges[j].weight) {
                EdgeInfo temp = allEdges[j];
                allEdges[j] = allEdges[j + 1];
                allEdges[j + 1] = temp;
            }
        }
    }
    
    addAlgorithmStep(QString("收集所有边并按权重排序，共 %1 条边").arg(allEdges.size()),
                    MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
    
    // 并查集：每个顶点的父节点
    MyMapQStringToQString parent;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QString key = m_vertices.keyAt(i);
        parent[key] = key;  // 初始时每个顶点是自己的父节点
    }
    
    // 查找根节点
    auto findRoot = [&](const QString &v) -> QString {
        QString root = v;
        // 找到祖宗-祖宗的父亲是它自己
        while (parent[root] != root) {
            //往上找一级
            root = parent[root];
        }
        // 路径压缩
        //
        QString temp = v;
        //把从v到root之间的parent都设为祖宗，省的待会再找一遍
        while (parent[temp] != root) {
            QString next = parent[temp];
            parent[temp] = root;
            temp = next;
        }
        return root;
    };
    
    // 已加入最小生成树的边集合
    MySetPairQStringQString mstEdges;
    int edgesAdded = 0;
    
    for (size_t idx = 0; idx < allEdges.size(); ++idx) {
        const EdgeInfo &edgeInfo = allEdges[idx];
        QString rootFrom = findRoot(edgeInfo.from);
        QString rootTo = findRoot(edgeInfo.to);
        
        if (rootFrom != rootTo) {
            // 不在同一连通分量中，可以加入MST
            parent[rootFrom] = rootTo;  // 合并两个连通分量
            mstEdges.insert(MyPairQStringQString(edgeInfo.from, edgeInfo.to));
            edgesAdded++;
            
            addAlgorithmStep(QString("选择边：%1 -> %2 (权重: %3)，加入最小生成树").arg(edgeInfo.from)
                            .arg(edgeInfo.to).arg(edgeInfo.weight),
                            MyVectorQString(), MyVectorPairQStringQString(), VisualState::Selected);
        }
        
        // 如果已经添加了n-1条边，MST完成
        if (edgesAdded >= static_cast<int>(m_vertices.size()) - 1) {
            break;
        }
    }
    
    addAlgorithmStep(QString("Kruskal算法完成，最小生成树包含 %1 条边").arg(edgesAdded),
                    MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
}

void GraphModel::generateTopologicalSortSteps()
{
    if (m_vertices.empty()) {
        return;
    }
    
    if (!m_isDirected) {
        addAlgorithmStep("拓扑排序只能用于有向图", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
        return;
    }
    
    addAlgorithmStep("开始拓扑排序", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Current);
    
    // 计算每个顶点的入度
    MyMapQStringToInt inDegree;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        inDegree[m_vertices.keyAt(i)] = 0;
    }
    
    for (size_t i = 0; i < m_edges.size(); ++i) {
        inDegree[m_edges.keyAt(i).second]++;  // 目标顶点入度+1
    }
    
    addAlgorithmStep("计算每个顶点的入度", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
    
    // 队列：存储入度为0的顶点
    MyQueueQString zeroInDegreeQueue;
    for (size_t i = 0; i < inDegree.size(); ++i) {
        if (inDegree.valueAt(i) == 0) {
            QString vertex = inDegree.keyAt(i);
            zeroInDegreeQueue.push(vertex);
            MyVectorQString vertexVec;
            vertexVec.push_back(vertex);
            addAlgorithmStep(QString("顶点 %1 入度为0，加入队列").arg(vertex),
                            vertexVec, MyVectorPairQStringQString(), VisualState::Current);
        }
    }
    
    MyVectorQString topologicalOrder;
    
    while (!zeroInDegreeQueue.empty()) {
        QString current = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        
        topologicalOrder.push_back(current);
        
        MyVectorQString currentVec5;
        currentVec5.push_back(current);
        addAlgorithmStep(QString("处理顶点 %1 (拓扑序第 %2 个)").arg(current).arg(topologicalOrder.size()),
                        currentVec5, MyVectorPairQStringQString(), VisualState::Visited);
        
        // 减少所有邻居的入度
        MyVectorQString neighbors = getNeighbors(current);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            const QString &neighbor = neighbors.at(i);
            inDegree[neighbor]--;
            
            MyVectorQString neighborVec3;
            neighborVec3.push_back(neighbor);
            MyVectorPairQStringQString edgeVec3;
            edgeVec3.push_back(MyPairQStringQString(current, neighbor));
            addAlgorithmStep(QString("减少顶点 %1 的入度，当前入度: %2").arg(neighbor).arg(inDegree[neighbor]),
                            neighborVec3, edgeVec3, VisualState::Current);
            
            if (inDegree[neighbor] == 0) {
                zeroInDegreeQueue.push(neighbor);
                MyVectorQString neighborVec4;
                neighborVec4.push_back(neighbor);
                addAlgorithmStep(QString("顶点 %1 入度变为0，加入队列").arg(neighbor),
                                neighborVec4, MyVectorPairQStringQString(), VisualState::Current);
            }
        }
    }
    
    if (topologicalOrder.size() < m_vertices.size()) {
        addAlgorithmStep("图中存在环，无法完成拓扑排序", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
    } else {
        QString orderStr = "拓扑排序结果: ";
        for (size_t i = 0; i < topologicalOrder.size(); i++) {
            if (i > 0) orderStr += " -> ";
            orderStr += topologicalOrder[i];
        }
        addAlgorithmStep(orderStr, MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
        addAlgorithmStep("拓扑排序完成", MyVectorQString(), MyVectorPairQStringQString(), VisualState::Normal);
    }
}

void GraphModel::resetAlgorithmState()
{
    m_visitedVertices.clear();
    m_visitedEdges.clear();
    
    while (!m_bfsQueue.empty()) {
        m_bfsQueue.pop();
    }
    
    while (!m_dfsStack.empty()) {
        m_dfsStack.pop();
    }
    resetVisualization();
}

void GraphModel::addAlgorithmStep(const QString &description, 
                                 const MyVectorQString &vertices,
                                 const MyVectorPairQStringQString &edges,
                                 VisualState state,
                                 int delay)
{
    AlgorithmStep step;
    step.description = description;
    step.vertices = vertices;
    step.edges = edges;
    step.state = state;
    step.delay = delay;
    
    m_algorithmSteps.push(step);
}

QPointF GraphModel::calculateCircularPosition(int index, int total, qreal radius)
{
    qreal angle = 2.0 * M_PI * index / total;
    qreal x = radius * qCos(angle);
    qreal y = radius * qSin(angle);
    return QPointF(x, y);
}

void GraphModel::applyForceDirectedStep()
{
    // 简化的力导向布局实现
    MyMapQStringToQPointF forces;
    
    // 初始化力
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        forces[m_vertices.keyAt(i)] = QPointF(0, 0);
    }
    
    // 计算排斥力（所有顶点对之间）
    for (size_t i1 = 0; i1 < m_vertices.size(); ++i1) {
        for (size_t i2 = i1 + 1; i2 < m_vertices.size(); ++i2) {
            QPointF pos1 = m_vertices.valueAt(i1)->pos();
            QPointF pos2 = m_vertices.valueAt(i2)->pos();
            QPointF diff = pos1 - pos2;
            qreal distance = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
            
            if (distance > 0) {
                qreal force = 100.0 / (distance * distance);
                QPointF forceVector = (diff / distance) * force;
                forces[m_vertices.keyAt(i1)] += forceVector;
                forces[m_vertices.keyAt(i2)] -= forceVector;
            }
        }
    }
    
    // 计算吸引力（相邻顶点之间）
    for (size_t i = 0; i < m_edges.size(); ++i) {
        const QString &from = m_edges.keyAt(i).first;
        const QString &to = m_edges.keyAt(i).second;
        
        QPointF pos1 = m_vertices[from]->pos();
        QPointF pos2 = m_vertices[to]->pos();
        QPointF diff = pos2 - pos1;
        qreal distance = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
        
        if (distance > 0) {
            qreal force = distance / 50.0; // 理想距离
            QPointF forceVector = (diff / distance) * force;
            forces[from] += forceVector;
            forces[to] -= forceVector;
        }
    }
    
    // 应用力
    const qreal damping = 0.1;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF force = forces[m_vertices.keyAt(i)] * damping;
        QPointF newPos = m_vertices.valueAt(i)->pos() + force;
        m_vertices.valueAt(i)->setPosition(newPos);
    }
    
    // 更新边的位置
    for (size_t i = 0; i < m_edges.size(); ++i) {
        m_edges.valueAt(i)->updatePosition();
    }
}

MyVectorQString GraphModel::getNeighbors(const QString &vertex) const
{
    MyVectorQString neighbors;
    const MySetQString* ptr = m_adjacencyList.find(vertex);
    if (ptr != nullptr) {
        for (size_t i = 0; i < ptr->size(); ++i) {
            neighbors.push_back(ptr->at(i));
        }
    }
    return neighbors;
}

bool GraphModel::hasPath(const QString &from, const QString &to) const
{
    if (from == to) return true;
    
    MySetQString visited;
    MyQueueQString queue;
    
    queue.push(from);
    visited.insert(from);
    
    while (!queue.empty()) {
        QString current = queue.front();
        queue.pop();
        
        MyVectorQString neighbors = getNeighbors(current);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            const QString &neighbor = neighbors.at(i);
            if (neighbor == to) {
                return true;
            }
            if (!visited.find(neighbor)) {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    
    return false;
}

MyVectorPairQStringQString GraphModel::getAllEdgesSorted() const
{
    MyVectorPairQStringQString edges;
    for (size_t i = 0; i < m_edges.size(); ++i) {
        edges.push_back(MyPairQStringQString(m_edges.keyAt(i).first, m_edges.keyAt(i).second));
    }
    
    // 按权重排序（这里简化处理，使用冒泡排序）
    // MyPairQStringQString 已经实现了 operator<，所以可以直接比较
    for (size_t i = 0; i < edges.size(); ++i) {
        for (size_t j = 0; j < edges.size() - 1 - i; ++j) {
            if (edges[j + 1] < edges[j]) {
                MyPairQStringQString temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }
    return edges;
}

// ==================== 文件操作实现 ====================

bool GraphModel::saveToFile(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开文件进行写入:" << fileName;
        return false;
    }
    
    QJsonObject graphObject;
    
    // 保存图的基本属性
    graphObject["type"] = "graph";
    graphObject["directed"] = m_isDirected;
    graphObject["vertexCount"] = static_cast<int>(m_vertices.size());
    graphObject["edgeCount"] = static_cast<int>(m_edges.size());
    
    // 保存顶点信息
    QJsonArray verticesArray;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QString &label = m_vertices.keyAt(i);
        VertexItem *vertex = m_vertices.valueAt(i);
        
        QJsonObject vertexObject;
        vertexObject["label"] = label;
        vertexObject["x"] = vertex->pos().x();
        vertexObject["y"] = vertex->pos().y();
        vertexObject["radius"] = vertex->getRadius();
        
        verticesArray.append(vertexObject);
    }
    graphObject["vertices"] = verticesArray;
    
    // 保存边信息
    QJsonArray edgesArray;
    MySetPairQStringQString processedEdges; // 避免重复保存无向图的边
    
    for (size_t i = 0; i < m_edges.size(); ++i) {
        const QString &from = m_edges.keyAt(i).first;
        const QString &to = m_edges.keyAt(i).second;
        EdgeItem *edge = m_edges.valueAt(i);
        
        // 对于无向图，只保存一次边（避免重复）
        if (!m_isDirected) {
            MyPairQStringQString normalizedEdge = (from < to) ? 
                MyPairQStringQString(from, to) : MyPairQStringQString(to, from);
            if (processedEdges.find(normalizedEdge)) {
                continue;
            }
            processedEdges.insert(normalizedEdge);
        }
        
        QJsonObject edgeObject;
        edgeObject["from"] = from;
        edgeObject["to"] = to;
        edgeObject["weight"] = edge->getWeight();
        edgeObject["directed"] = m_isDirected;
        
        edgesArray.append(edgeObject);
    }
    graphObject["edges"] = edgesArray;
    
    // 保存到文件
    QJsonDocument document(graphObject);
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << document.toJson();
    
    file.close();
    qDebug() << "图已保存到文件:" << fileName;
    return true;
}

bool GraphModel::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件进行读取:" << fileName;
        return false;
    }
    
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    //用utf-8读取文本文件
    //存储在QString中 以utf-16编码存储
    QString jsonString = stream.readAll();
    file.close();
    
    QJsonParseError error;
    //toUtf8 将QString转换为utf-8编码
    QJsonDocument document = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        return false;
    }
    
    QJsonObject graphObject = document.object();
    
    // 验证文件类型
    if (graphObject["type"].toString() != "graph") {
        qDebug() << "文件类型错误，不是图文件";
        return false;
    }
    
    // 清空当前图
    clearGraph();
    
    // 加载图的基本属性
    m_isDirected = graphObject["directed"].toBool();
    
    // 加载顶点
    QJsonArray verticesArray = graphObject["vertices"].toArray();
    for (const QJsonValue &value : verticesArray) {
        QJsonObject vertexObject = value.toObject();//从JSON value到JSON Object
        QString label = vertexObject["label"].toString();//从JSON Object到QString
        qreal x = vertexObject["x"].toDouble();//从JSON Object到qreal
        qreal y = vertexObject["y"].toDouble();//从JSON Object到qreal
        qreal radius = vertexObject["radius"].toDouble(25.0); // 默认半径25
        
        addVertex(label, QPointF(x, y));
        
        // 设置顶点半径
        VertexItem *vertex = getVertex(label);
        if (vertex) {
            vertex->setRadius(radius);
        }
    }
    
    // 加载边
    QJsonArray edgesArray = graphObject["edges"].toArray();
    for (const QJsonValue &value : edgesArray) {
        QJsonObject edgeObject = value.toObject();//从JSON value到JSON Object
        QString from = edgeObject["from"].toString();
        QString to = edgeObject["to"].toString();
        int weight = edgeObject["weight"].toInt(1);
        
        addEdge(from, to, weight);
    }
    
    qDebug() << "图已从文件加载:" << fileName;
    qDebug() << "顶点数:" << m_vertices.size() << "边数:" << m_edges.size();
    return true;
}

// ==================== 矩阵和邻接表表示实现 ====================

QMap<QString, QMap<QString, int>> GraphModel::getMatrixRepresentation() const
{
    QMap<QString, QMap<QString, int>> matrix;
    
    // 初始化所有顶点对为 -1（不相连）
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QString from = m_vertices.keyAt(i);
        QMap<QString, int> row;
        for (size_t j = 0; j < m_vertices.size(); ++j) {
            QString to = m_vertices.keyAt(j);
            row[to] = -1;
        }
        matrix[from] = row;
    }
    
    // 填充边的权重
    for (size_t i = 0; i < m_edges.size(); ++i) {
        QString from = m_edges.keyAt(i).first;
        QString to = m_edges.keyAt(i).second;
        EdgeItem *edge = m_edges.valueAt(i);
        int weight = edge->getWeight();
        
        // 如果权重为0或1，在无权图中显示1
        if (weight <= 1) {
            weight = 1;
        }
        
        matrix[from][to] = weight;
        
        // 如果是无向图，也设置反向
        if (!m_isDirected) {
            matrix[to][from] = weight;
        }
    }
    
    return matrix;
}

void GraphModel::updateFromMatrix(const QMap<QString, QMap<QString, int>> &matrix)
{
    // 先删除所有现有边
    MyVectorPairQStringQString edgesToRemove;
    for (size_t i = 0; i < m_edges.size(); ++i) {
        edgesToRemove.push_back(MyPairQStringQString(m_edges.keyAt(i).first, m_edges.keyAt(i).second));
    }
    for (size_t i = 0; i < edgesToRemove.size(); ++i) {
        const MyPairQStringQString &edgePair = edgesToRemove.at(i);
        removeEdge(edgePair.first, edgePair.second);
    }
    
    // 根据矩阵添加新边
    for (auto it = matrix.begin(); it != matrix.end(); ++it) {
        QString from = it.key();
        const QMap<QString, int> &row = it.value();
        
        for (auto jt = row.begin(); jt != row.end(); ++jt) {
            QString to = jt.key();
            int weight = jt.value();
            
            // 如果值为 -1，表示不相连，跳过
            if (weight == -1) {
                continue;
            }
            
            // 如果是无向图且 from > to，跳过（避免重复）
            if (!m_isDirected && from > to) {
                continue;
            }
            
            // 添加边
            addEdge(from, to, weight);
        }
    }
}

QMap<QString, QList<QPair<QString, int>>> GraphModel::getAdjacencyListRepresentation() const
{
    QMap<QString, QList<QPair<QString, int>>> adjList;
    
    // 初始化所有顶点的邻接表为空
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        adjList[m_vertices.keyAt(i)] = QList<QPair<QString, int>>();
    }
    
    // 填充邻接表
    for (size_t i = 0; i < m_edges.size(); ++i) {
        QString from = m_edges.keyAt(i).first;
        QString to = m_edges.keyAt(i).second;
        EdgeItem *edge = m_edges.valueAt(i);
        int weight = edge->getWeight();
        
        // 如果权重为0或1，在无权图中显示1
        if (weight <= 1) {
            weight = 1;
        }
        
        adjList[from].append(qMakePair(to, weight));
        
        // 如果是无向图，也添加到反向
        if (!m_isDirected) {
            adjList[to].append(qMakePair(from, weight));
        }
    }
    
    return adjList;
}

void GraphModel::updateFromAdjacencyList(const QMap<QString, QList<QPair<QString, int>>> &adjList)
{
    // 先删除所有现有边
    MyVectorPairQStringQString edgesToRemove;
    for (size_t i = 0; i < m_edges.size(); ++i) {
        edgesToRemove.push_back(MyPairQStringQString(m_edges.keyAt(i).first, m_edges.keyAt(i).second));
    }
    for (size_t i = 0; i < edgesToRemove.size(); ++i) {
        const MyPairQStringQString &edgePair = edgesToRemove.at(i);
        removeEdge(edgePair.first, edgePair.second);
    }
    
    // 根据邻接表添加新边
    for (auto it = adjList.begin(); it != adjList.end(); ++it) {
        QString from = it.key();
        const QList<QPair<QString, int>> &neighbors = it.value();
        
        for (const auto &neighborPair : neighbors) {
            QString to = neighborPair.first;
            int weight = neighborPair.second;
            
            // 如果是无向图且 from > to，跳过（避免重复）
            if (!m_isDirected && from > to) {
                continue;
            }
            
            // 添加边
            addEdge(from, to, weight);
        }
    }
}
