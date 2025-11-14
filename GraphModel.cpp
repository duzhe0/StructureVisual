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
            generatePrimSteps();
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
    
    stack.push(startVertex);
    
    while (!stack.empty()) {
        QString current = stack.top();
        stack.pop();
        
        if (visited.find(current) != visited.end()) {
            continue;
        }
        
        visited.insert(current);
        addAlgorithmStep(QString("访问顶点 %1").arg(current), 
                        {current}, {}, VisualState::Visited);
        
        std::vector<QString> neighbors = getNeighbors(current);
        for (const QString &neighbor : neighbors) {
            if (visited.find(neighbor) == visited.end()) {
                addAlgorithmStep(QString("发现未访问的邻居 %1，加入栈中").arg(neighbor), 
                                {current, neighbor}, {{current, neighbor}}, VisualState::Current);
                stack.push(neighbor);
            }
        }
    }
    
    addAlgorithmStep("深度优先搜索完成", {}, {}, VisualState::Normal);
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
                addAlgorithmStep(QString("发现未访问的邻居 %1，加入队列").arg(neighbor), 
                                {current, neighbor}, {{current, neighbor}}, VisualState::Current);
                queue.push(neighbor);
            }
        }
    }
    
    addAlgorithmStep("广度优先搜索完成", {}, {}, VisualState::Normal);
}

void GraphModel::generateDijkstraSteps(const QString &startVertex)
{
    // 简化的Dijkstra算法步骤生成
    addAlgorithmStep(QString("开始Dijkstra最短路径算法，从顶点 %1 开始").arg(startVertex), 
                    {startVertex}, {}, VisualState::Current);
    
    // 这里可以实现完整的Dijkstra算法步骤
    addAlgorithmStep("Dijkstra算法完成", {}, {}, VisualState::Normal);
}

void GraphModel::generatePrimSteps()
{
    addAlgorithmStep("开始Prim最小生成树算法", {}, {}, VisualState::Current);
    // 实现Prim算法步骤
    addAlgorithmStep("Prim算法完成", {}, {}, VisualState::Normal);
}

void GraphModel::generateKruskalSteps()
{
    addAlgorithmStep("开始Kruskal最小生成树算法", {}, {}, VisualState::Current);
    // 实现Kruskal算法步骤
    addAlgorithmStep("Kruskal算法完成", {}, {}, VisualState::Normal);
}

void GraphModel::generateTopologicalSortSteps()
{
    addAlgorithmStep("开始拓扑排序", {}, {}, VisualState::Current);
    // 实现拓扑排序步骤
    addAlgorithmStep("拓扑排序完成", {}, {}, VisualState::Normal);
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
