#include "GraphModel.h"
#include "VisualItem.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
#include <QtMath>
#include <algorithm>
#include <random>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QStringConverter>
#include <set>
#include <climits>
#include <map>

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
    if (m_vertices.find(label) != m_vertices.end()) {
        qDebug() << "Vertex" << label << "already exists";
        return false;
    }
    
    VertexItem *vertex = new VertexItem(label, position);
    m_vertices[label] = vertex;
    m_adjacencyList[label] = std::set<QString>();
    
    if (m_scene) {
        m_scene->addItem(vertex);
        vertex->startInsertAnimation();
    }
    
    emit vertexAdded(label);
    return true;
}

bool GraphModel::removeVertex(const QString &label)
{
    auto it = m_vertices.find(label);
    if (it == m_vertices.end()) {
        qDebug() << "Vertex" << label << "does not exist";
        return false;
    }
    
    VertexItem *vertex = it->second;
    
    // 删除所有相关的边
    std::vector<std::pair<QString, QString>> edgesToRemove;
    for (const auto &edgePair : m_edges) {
        const QString &from = edgePair.first.first;
        const QString &to = edgePair.first.second;
        if (from == label || to == label) {
            edgesToRemove.push_back({from, to});
        }
    }
    
    for (const auto &edgePair : edgesToRemove) {
        removeEdge(edgePair.first, edgePair.second);
    }
    
    // 删除顶点
    m_vertices.erase(it);
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
    if (m_vertices.find(from) == m_vertices.end() || 
        m_vertices.find(to) == m_vertices.end()) {
        qDebug() << "One or both vertices do not exist";
        return false;
    }
    
    auto edgeKey = std::make_pair(from, to);
    if (m_edges.find(edgeKey) != m_edges.end()) {
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
        auto reverseKey = std::make_pair(to, from);
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
    auto edgeKey = std::make_pair(from, to);
    auto it = m_edges.find(edgeKey);
    if (it == m_edges.end()) {
        qDebug() << "Edge does not exist";
        return false;
    }
    
    EdgeItem *edge = it->second;
    m_edges.erase(it);
    m_adjacencyList[from].erase(to);
    
    if (!m_isDirected) {
        auto reverseKey = std::make_pair(to, from);
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
    auto it = m_vertices.find(label);
    return (it != m_vertices.end()) ? it->second : nullptr;
}

EdgeItem* GraphModel::getEdge(const QString &from, const QString &to) const
{
    auto edgeKey = std::make_pair(from, to);
    auto it = m_edges.find(edgeKey);
    return (it != m_edges.end()) ? it->second : nullptr;
}

std::vector<VertexItem*> GraphModel::getAllVertices() const
{
    std::vector<VertexItem*> vertices;
    for (const auto &pair : m_vertices) {
        vertices.push_back(pair.second);
    }
    return vertices;
}

std::vector<EdgeItem*> GraphModel::getAllEdges() const
{
    std::vector<EdgeItem*> edges;
    for (const auto &pair : m_edges) {
        edges.push_back(pair.second);
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
    for (auto &pair : m_vertices) {
        scene->addItem(pair.second);
    }
    for (auto &pair : m_edges) {
        scene->addItem(pair.second);
    }
}

void GraphModel::applyCircularLayout()
{
    if (m_vertices.empty()) return;
    
    int index = 0;
    int total = static_cast<int>(m_vertices.size());
    qreal radius = qMin(200.0, total * 20.0);
    
    for (auto &pair : m_vertices) {
        QPointF position = calculateCircularPosition(index, total, radius);
        pair.second->setPosition(position);
        index++;
    }
    
    // 更新边的位置
    for (auto &pair : m_edges) {
        pair.second->updatePosition();
    }
}

void GraphModel::applyForceDirectedLayout()
{
    // 简化的力导向布局算法
    const int iterations = 100;
    const qreal k = 50.0; // 理想距离
    const qreal c = 0.1;  // 冷却因子
    
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
    
    for (auto &pair : m_vertices) {
        QPointF position(col * spacing, row * spacing);
        pair.second->setPosition(position);
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
    
    // 更新边的位置
    for (auto &pair : m_edges) {
        pair.second->updatePosition();
    }
}

void GraphModel::resetVisualization()
{
    // 重置所有顶点的可视化状态
    for (auto &pair : m_vertices) {
        pair.second->setVisualState(VisualState::Normal);
        pair.second->stopAnimations();
    }
    
    // 重置所有边的可视化状态
    for (auto &pair : m_edges) {
        pair.second->setVisualState(VisualState::Normal);
        pair.second->stopAnimations();
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
    for (auto &pair : m_edges) {
        if (m_scene) {
            m_scene->removeItem(pair.second);
        }
        delete pair.second;
    }
    m_edges.clear();
    
    // 删除所有顶点
    for (auto &pair : m_vertices) {
        if (m_scene) {
            m_scene->removeItem(pair.second);
        }
        delete pair.second;
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
    for (const QString &vertexLabel : step.vertices) {
        VertexItem *vertex = getVertex(vertexLabel);
        if (vertex) {
            vertex->setVisualState(step.state);
            if (step.state == VisualState::Current) {
                vertex->startPulseAnimation();
            }
        }
    }
    
    for (const auto &edgePair : step.edges) {
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
    if (m_vertices.find(startVertex) == m_vertices.end()) {
        return;
    }
    addAlgorithmStep(QString("开始深度优先搜索，从顶点 %1 开始").arg(startVertex), 
                    {startVertex}, {}, VisualState::Current);
    std::stack<QString> stack;
    std::set<QString> visited;
    std::set<QString> inStack;  // 记录已在栈中的顶点，避免重复入栈
    
    stack.push(startVertex);
    inStack.insert(startVertex);
    
    while(!stack.empty()){
        QString current = stack.top();
        stack.pop();
        inStack.erase(current);  // 从栈中移除时，清除标记
        
        // 如果该元素已访问，则跳过
        if(visited.find(current) != visited.end()){
            continue;
        }
        
        // 访问该元素
        addAlgorithmStep(QString("访问顶点 %1").arg(current), 
                        {current}, {}, VisualState::Visited);
        visited.insert(current);
        
        // 获取所有邻居
        std::vector<QString> neighbors = getNeighbors(current);
        for (const QString &neighbor : neighbors){
            // 如果邻居未访问且不在栈中，则加入栈
            if(visited.find(neighbor) == visited.end() && 
               inStack.find(neighbor) == inStack.end()){
                stack.push(neighbor);
                inStack.insert(neighbor);  // 标记已入栈
                // 只把新发现的邻居设为Current状态，current保持Visited状态
                addAlgorithmStep(QString("发现未访问的邻居 %1，加入栈中").arg(neighbor), 
                                {neighbor}, {{current, neighbor}}, VisualState::Current);
            }
        }
    }
    addAlgorithmStep("深度优先搜索完成", {}, {}, VisualState::Normal);
    
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
    if (m_vertices.find(startVertex) == m_vertices.end()) {
        return;
    }
    
    addAlgorithmStep(QString("开始广度优先搜索，从顶点 %1 开始").arg(startVertex), 
                    {startVertex}, {}, VisualState::Current);
    
    std::queue<QString> queue;
    std::set<QString> visited;
    
    queue.push(startVertex);
    visited.insert(startVertex);
    
    while (!queue.empty()) {
        QString current = queue.front();
        queue.pop();
        
        addAlgorithmStep(QString("处理顶点 %1").arg(current), 
                        {current}, {}, VisualState::Visited);
        
        std::vector<QString> neighbors = getNeighbors(current);
        for (const QString &neighbor : neighbors) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                // 只把新发现的邻居设为Current状态，current保持Visited状态
                addAlgorithmStep(QString("发现未访问的邻居 %1，加入队列").arg(neighbor), 
                                {neighbor}, {{current, neighbor}}, VisualState::Current);
                queue.push(neighbor);
            }
        }
    }
    
    addAlgorithmStep("广度优先搜索完成", {}, {}, VisualState::Normal);
}

void GraphModel::generateDijkstraSteps(const QString &startVertex)
{
    if (m_vertices.find(startVertex) == m_vertices.end()) {
        return;
    }
    
    addAlgorithmStep(QString("开始Dijkstra最短路径算法，从顶点 %1 开始").arg(startVertex), 
                    {startVertex}, {}, VisualState::Visited);
    
    // 距离映射：顶点 -> 最短距离
    std::map<QString, int> distances;
    // 前驱映射：顶点 -> 前驱顶点
    std::map<QString, QString> predecessors;
    // 未访问顶点集合
    std::set<QString> unvisited;
    
    // 初始化：所有顶点距离为无穷大，起始顶点距离为0
    for (const auto &pair : m_vertices) {
        distances[pair.first] = INT_MAX;
        unvisited.insert(pair.first);
    }
    distances[startVertex] = 0;
    
    while (!unvisited.empty()) {
        // 找到未访问顶点中距离最小的
        QString current;
        int minDist = INT_MAX;
        for (const QString &v : unvisited) {
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
            addAlgorithmStep(QString("选择距离最小的未访问顶点 %1 (距离: %2)").arg(current).arg(minDist),
                            {current}, {}, VisualState::Visited);
        }
        
        // 更新邻居的距离
        std::vector<QString> neighbors = getNeighbors(current);
        for (const QString &neighbor : neighbors) {
            if (unvisited.find(neighbor) == unvisited.end()) {
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
                    QString prevPred = predecessors.find(neighbor) != predecessors.end() ? predecessors[neighbor] : "无";
                    predecessors[neighbor] = current;  // 更新前驱顶点
                    addAlgorithmStep(QString("发现更短路径到 %1：%2 -> %3 (新距离: %4，前驱: %2)").arg(neighbor)
                                    .arg(current).arg(neighbor).arg(newDist),
                                    {neighbor}, {{current, neighbor}}, VisualState::Current);
                    distances[neighbor] = newDist;
                } else {
                    addAlgorithmStep(QString("检查到 %1 的路径：%2 -> %3 (距离: %4，不更新)").arg(neighbor)
                                    .arg(current).arg(neighbor).arg(distances[neighbor]),
                                    {neighbor}, {{current, neighbor}}, VisualState::Normal);
                }
            }
        }
        
        if (current != startVertex) {
            addAlgorithmStep(QString("顶点 %1 已处理完成，距离: %2").arg(current).arg(distances[current]),
                            {current}, {}, VisualState::Visited);
        }
    }
    
    // 显示最短路径结果
    addAlgorithmStep("Dijkstra算法完成，开始显示最短路径结果", {}, {}, VisualState::Normal);
    
    // 为每个可达顶点显示最短路径
    for (const auto &pair : m_vertices) {
        const QString &target = pair.first;
        if (target == startVertex) {
            continue;  // 跳过起始顶点
        }
        
        if (distances[target] == INT_MAX) {
            addAlgorithmStep(QString("顶点 %1 不可达").arg(target), {target}, {}, VisualState::Normal);
            continue;
        }
        
        // 回溯路径
        std::vector<QString> path;
        QString current = target;
        while (current != startVertex && predecessors.find(current) != predecessors.end()) {
            path.push_back(current);
            current = predecessors[current];
        }
        path.push_back(startVertex);
        std::reverse(path.begin(), path.end());
        
        // 构建路径字符串
        QString pathStr;
        for (size_t i = 0; i < path.size(); i++) {
            if (i > 0) pathStr += " -> ";
            pathStr += path[i];
        }
        
        // 构建边列表用于高亮显示
        std::vector<std::pair<QString, QString>> pathEdges;
        for (size_t i = 0; i < path.size() - 1; i++) {
            pathEdges.push_back(std::make_pair(path[i], path[i + 1]));
        }
        
        addAlgorithmStep(QString("到顶点 %1 的最短路径：%2 (距离: %3)").arg(target).arg(pathStr).arg(distances[target]),
                        path, pathEdges, VisualState::Selected);
    }
    
    addAlgorithmStep("所有最短路径显示完成", {}, {}, VisualState::Normal);
}

void GraphModel::generatePrimSteps(const QString &startVertex)
{
    if (m_vertices.empty()) {
        return;
    }
    
    // 确定起始顶点：如果指定了且存在则使用，否则使用第一个顶点
    QString actualStartVertex = startVertex;
    if (actualStartVertex.isEmpty() || m_vertices.find(actualStartVertex) == m_vertices.end()) {
        actualStartVertex = m_vertices.begin()->first;
        if (!startVertex.isEmpty()) {
            addAlgorithmStep(QString("指定的起始顶点 %1 不存在，使用顶点 %2 作为起始点").arg(startVertex).arg(actualStartVertex),
                            {actualStartVertex}, {}, VisualState::Current);
        }
    }
    
    addAlgorithmStep(QString("开始Prim最小生成树算法，起始顶点: %1").arg(actualStartVertex), 
                    {actualStartVertex}, {}, VisualState::Current);
    
    // 已加入最小生成树的顶点集合
    std::set<QString> mstVertices;
    // 已加入最小生成树的边集合
    std::set<std::pair<QString, QString>> mstEdges;
    
    // 使用指定的起始顶点
    mstVertices.insert(actualStartVertex);
    
    addAlgorithmStep(QString("选择起始顶点 %1 加入最小生成树").arg(startVertex),
                    {startVertex}, {}, VisualState::Current);
    
    while (mstVertices.size() < m_vertices.size()) {
        // 找到连接已访问顶点和未访问顶点的最小权重边
        QString minFrom, minTo;
        int minWeight = INT_MAX;
        
        for (const QString &v : mstVertices) {
            std::vector<QString> neighbors = getNeighbors(v);
            for (const QString &neighbor : neighbors) {
                if (mstVertices.find(neighbor) != mstVertices.end()) {
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
            addAlgorithmStep("图不连通，无法生成完整的最小生成树", {}, {}, VisualState::Normal);
            break;
        }
        
        // 将找到的最小边加入MST
        mstVertices.insert(minTo);
        mstEdges.insert(std::make_pair(minFrom, minTo));
        
        addAlgorithmStep(QString("找到最小权重边：%1 -> %2 (权重: %3)，加入最小生成树").arg(minFrom).arg(minTo).arg(minWeight),
                        {minFrom, minTo}, {{minFrom, minTo}}, VisualState::Selected);
        
        addAlgorithmStep(QString("顶点 %1 已加入最小生成树").arg(minTo),
                        {minTo}, {}, VisualState::Selected);
    }
    
    addAlgorithmStep(QString("Prim算法完成，最小生成树包含 %1 个顶点和 %2 条边").arg(mstVertices.size()).arg(mstEdges.size()),
                    {}, {}, VisualState::Normal);
}

void GraphModel::generateKruskalSteps()
{
    if (m_vertices.empty()) {
        return;
    }
    
    addAlgorithmStep("开始Kruskal最小生成树算法", {}, {}, VisualState::Current);
    
    // 收集所有边并按权重排序
    struct EdgeInfo {
        QString from;
        QString to;
        int weight;
    };
    
    std::vector<EdgeInfo> allEdges;
    for (const auto &pair : m_edges) {
        EdgeItem *edge = pair.second;
        if (edge) {
            allEdges.push_back({pair.first.first, pair.first.second, edge->getWeight()});
        }
    }
    
    // 按权重排序
    std::sort(allEdges.begin(), allEdges.end(), 
              [](const EdgeInfo &a, const EdgeInfo &b) { return a.weight < b.weight; });
    
    addAlgorithmStep(QString("收集所有边并按权重排序，共 %1 条边").arg(allEdges.size()),
                    {}, {}, VisualState::Normal);
    
    // 并查集：每个顶点的父节点
    std::map<QString, QString> parent;
    for (const auto &pair : m_vertices) {
        parent[pair.first] = pair.first;  // 初始时每个顶点是自己的父节点
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
    std::set<std::pair<QString, QString>> mstEdges;
    int edgesAdded = 0;
    
    for (const auto &edgeInfo : allEdges) {
        QString rootFrom = findRoot(edgeInfo.from);
        QString rootTo = findRoot(edgeInfo.to);
        
        if (rootFrom != rootTo) {
            // 不在同一连通分量中，可以加入MST
            parent[rootFrom] = rootTo;  // 合并两个连通分量
            mstEdges.insert(std::make_pair(edgeInfo.from, edgeInfo.to));
            edgesAdded++;
            
            addAlgorithmStep(QString("选择边：%1 -> %2 (权重: %3)，加入最小生成树").arg(edgeInfo.from)
                            .arg(edgeInfo.to).arg(edgeInfo.weight),
                            {edgeInfo.from, edgeInfo.to}, {{edgeInfo.from, edgeInfo.to}}, VisualState::Selected);
        }
        
        // 如果已经添加了n-1条边，MST完成
        if (edgesAdded >= static_cast<int>(m_vertices.size()) - 1) {
            break;
        }
    }
    
    addAlgorithmStep(QString("Kruskal算法完成，最小生成树包含 %1 条边").arg(edgesAdded),
                    {}, {}, VisualState::Normal);
}

void GraphModel::generateTopologicalSortSteps()
{
    if (m_vertices.empty()) {
        return;
    }
    
    if (!m_isDirected) {
        addAlgorithmStep("拓扑排序只能用于有向图", {}, {}, VisualState::Normal);
        return;
    }
    
    addAlgorithmStep("开始拓扑排序", {}, {}, VisualState::Current);
    
    // 计算每个顶点的入度
    std::map<QString, int> inDegree;
    for (const auto &pair : m_vertices) {
        inDegree[pair.first] = 0;
    }
    
    for (const auto &pair : m_edges) {
        inDegree[pair.first.second]++;  // 目标顶点入度+1
    }
    
    addAlgorithmStep("计算每个顶点的入度", {}, {}, VisualState::Normal);
    
    // 队列：存储入度为0的顶点
    std::queue<QString> zeroInDegreeQueue;
    for (const auto &pair : inDegree) {
        if (pair.second == 0) {
            zeroInDegreeQueue.push(pair.first);
            addAlgorithmStep(QString("顶点 %1 入度为0，加入队列").arg(pair.first),
                            {pair.first}, {}, VisualState::Current);
        }
    }
    
    std::vector<QString> topologicalOrder;
    
    while (!zeroInDegreeQueue.empty()) {
        QString current = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        
        topologicalOrder.push_back(current);
        
        addAlgorithmStep(QString("处理顶点 %1 (拓扑序第 %2 个)").arg(current).arg(topologicalOrder.size()),
                        {current}, {}, VisualState::Visited);
        
        // 减少所有邻居的入度
        std::vector<QString> neighbors = getNeighbors(current);
        for (const QString &neighbor : neighbors) {
            inDegree[neighbor]--;
            
            addAlgorithmStep(QString("减少顶点 %1 的入度，当前入度: %2").arg(neighbor).arg(inDegree[neighbor]),
                            {neighbor}, {{current, neighbor}}, VisualState::Current);
            
            if (inDegree[neighbor] == 0) {
                zeroInDegreeQueue.push(neighbor);
                addAlgorithmStep(QString("顶点 %1 入度变为0，加入队列").arg(neighbor),
                                {neighbor}, {}, VisualState::Current);
            }
        }
    }
    
    if (topologicalOrder.size() < m_vertices.size()) {
        addAlgorithmStep("图中存在环，无法完成拓扑排序", {}, {}, VisualState::Normal);
    } else {
        QString orderStr = "拓扑排序结果: ";
        for (size_t i = 0; i < topologicalOrder.size(); i++) {
            if (i > 0) orderStr += " -> ";
            orderStr += topologicalOrder[i];
        }
        addAlgorithmStep(orderStr, {}, {}, VisualState::Normal);
        addAlgorithmStep("拓扑排序完成", {}, {}, VisualState::Normal);
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
                                 const std::vector<QString> &vertices,
                                 const std::vector<std::pair<QString, QString>> &edges,
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
    std::map<QString, QPointF> forces;
    
    // 初始化力
    for (const auto &pair : m_vertices) {
        forces[pair.first] = QPointF(0, 0);
    }
    
    // 计算排斥力（所有顶点对之间）
    for (auto it1 = m_vertices.begin(); it1 != m_vertices.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != m_vertices.end(); ++it2) {
            QPointF pos1 = it1->second->pos();
            QPointF pos2 = it2->second->pos();
            QPointF diff = pos1 - pos2;
            qreal distance = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
            
            if (distance > 0) {
                qreal force = 100.0 / (distance * distance);
                QPointF forceVector = (diff / distance) * force;
                forces[it1->first] += forceVector;
                forces[it2->first] -= forceVector;
            }
        }
    }
    
    // 计算吸引力（相邻顶点之间）
    for (const auto &edgePair : m_edges) {
        const QString &from = edgePair.first.first;
        const QString &to = edgePair.first.second;
        
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
    for (auto &pair : m_vertices) {
        QPointF force = forces[pair.first] * damping;
        QPointF newPos = pair.second->pos() + force;
        pair.second->setPosition(newPos);
    }
    
    // 更新边的位置
    for (auto &pair : m_edges) {
        pair.second->updatePosition();
    }
}

std::vector<QString> GraphModel::getNeighbors(const QString &vertex) const
{
    std::vector<QString> neighbors;
    auto it = m_adjacencyList.find(vertex);
    if (it != m_adjacencyList.end()) {
        for (const QString &neighbor : it->second) {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

bool GraphModel::hasPath(const QString &from, const QString &to) const
{
    if (from == to) return true;
    
    std::set<QString> visited;
    std::queue<QString> queue;
    
    queue.push(from);
    visited.insert(from);
    
    while (!queue.empty()) {
        QString current = queue.front();
        queue.pop();
        
        std::vector<QString> neighbors = getNeighbors(current);
        for (const QString &neighbor : neighbors) {
            if (neighbor == to) {
                return true;
            }
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                queue.push(neighbor);
            }
        }
    }
    
    return false;
}

std::vector<std::pair<QString, QString>> GraphModel::getAllEdgesSorted() const
{
    std::vector<std::pair<QString, QString>> edges;
    for (const auto &pair : m_edges) {
        edges.push_back(pair.first);
    }
    
    // 按权重排序（这里简化处理）
    std::sort(edges.begin(), edges.end());
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
    for (const auto &vertexPair : m_vertices) {
        const QString &label = vertexPair.first;
        VertexItem *vertex = vertexPair.second;
        
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
    std::set<std::pair<QString, QString>> processedEdges; // 避免重复保存无向图的边
    
    for (const auto &edgePair : m_edges) {
        const QString &from = edgePair.first.first;
        const QString &to = edgePair.first.second;
        EdgeItem *edge = edgePair.second;
        
        // 对于无向图，只保存一次边（避免重复）
        if (!m_isDirected) {
            std::pair<QString, QString> normalizedEdge = (from < to) ? 
                std::make_pair(from, to) : std::make_pair(to, from);
            if (processedEdges.find(normalizedEdge) != processedEdges.end()) {
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
