#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QTimer>
#include <QMap>
#include <QList>
#include <memory>
#include "VisualItem.h"
#include "MyVectorQString.h"
#include "MyVectorVertexItemPtr.h"
#include "MyVectorEdgeItemPtr.h"
#include "MyMapQStringToVertexItemPtr.h"
#include "MyMapPairToEdgeItemPtr.h"
#include "MySetQString.h"
#include "MySetPairQStringQString.h"
#include "MyMapQStringToSetQString.h"
#include "MyQueueQString.h"
#include "MyQueueAlgorithmStep.h"
#include "MyStackQString.h"
#include "MyVectorPairQStringQString.h"

// 前向声明
class VertexItem;
class EdgeItem;
class QGraphicsScene;

// 图算法类型枚举
enum class GraphAlgorithm {
    DFS,           // 深度优先搜索
    BFS,           // 广度优先搜索
    Dijkstra,      // 最短路径
    Prim,          // 最小生成树
    Kruskal,       // 最小生成树
    TopologicalSort // 拓扑排序
};

// 图算法步骤结构
struct AlgorithmStep {
    QString description;           // 步骤描述
    MyVectorQString vertices;    // 涉及的顶点
    MyVectorPairQStringQString edges; // 涉及的边
    VisualState state;            // 可视化状态
    int delay;                    // 延迟时间(ms)
};

// 图数据模型类
class GraphModel : public QObject
{
    Q_OBJECT

public:
    explicit GraphModel(QObject *parent = nullptr);
    ~GraphModel();

    // 图的基本操作
    bool addVertex(const QString &label, const QPointF &position = QPointF(0, 0));
    bool removeVertex(const QString &label);
    bool addEdge(const QString &from, const QString &to, int weight = 1);
    bool removeEdge(const QString &from, const QString &to);
    
    // 顶点和边的查询
    VertexItem* getVertex(const QString &label) const;
    EdgeItem* getEdge(const QString &from, const QString &to) const;
    MyVectorVertexItemPtr getAllVertices() const;
    MyVectorEdgeItemPtr getAllEdges() const;
    
    // 图的基本属性
    int getVertexCount() const;
    int getEdgeCount() const;
    bool isEmpty() const;
    bool isDirected() const { return m_isDirected; }
    void setDirected(bool directed);
    
    // 算法执行
    void executeAlgorithm(GraphAlgorithm algorithm, const QString &startVertex = "");
    void pauseAlgorithm();
    void resumeAlgorithm();
    void stopAlgorithm();
    void stepAlgorithm();
    
    // 算法状态
    bool isAlgorithmRunning() const { return m_algorithmRunning; }
    bool isAlgorithmPaused() const { return m_algorithmPaused; }
    GraphAlgorithm getCurrentAlgorithm() const { return m_currentAlgorithm; }
    
    // 场景管理
    void setScene(QGraphicsScene *scene);
    QGraphicsScene* getScene() const { return m_scene; }
    
    // 布局算法
    void applyCircularLayout();
    void applyForceDirectedLayout();
    void applyGridLayout(int columns = 0);
    
    // 重置和清理
    void resetVisualization();
    void clearGraph();
    
    // 文件操作
    bool saveToFile(const QString &fileName) const;
    bool loadFromFile(const QString &fileName);
    
    // 矩阵和邻接表表示
    QMap<QString, QMap<QString, int>> getMatrixRepresentation() const;
    void updateFromMatrix(const QMap<QString, QMap<QString, int>> &matrix);
    QMap<QString, QList<QPair<QString, int>>> getAdjacencyListRepresentation() const;
    void updateFromAdjacencyList(const QMap<QString, QList<QPair<QString, int>>> &adjList);

signals:
    void algorithmStarted(GraphAlgorithm algorithm);
    void algorithmStepCompleted(const AlgorithmStep &step);
    void algorithmCompleted(GraphAlgorithm algorithm);
    void algorithmPaused();
    void algorithmResumed();
    void algorithmStopped();
    void vertexAdded(const QString &label);
    void vertexRemoved(const QString &label);
    void edgeAdded(const QString &from, const QString &to, int weight);
    void edgeRemoved(const QString &from, const QString &to);

private slots:
    void processNextStep();

private:
    // 图数据结构
    MyMapQStringToVertexItemPtr m_vertices;
    MyMapPairToEdgeItemPtr m_edges;
    MyMapQStringToSetQString m_adjacencyList;
    
    // 场景和可视化
    QGraphicsScene *m_scene;
    bool m_isDirected;
    
    // 算法执行相关
    GraphAlgorithm m_currentAlgorithm;
    bool m_algorithmRunning;
    bool m_algorithmPaused;
    MyQueueAlgorithmStep m_algorithmSteps;
    QTimer *m_algorithmTimer;
    QString m_startVertex;
    
    // 算法状态跟踪
    MySetQString m_visitedVertices;
    MySetPairQStringQString m_visitedEdges;
    MyQueueQString m_bfsQueue;
    MyStackQString m_dfsStack;
    
    // 算法实现
    void generateDFSSteps(const QString &startVertex);
    void generateBFSSteps(const QString &startVertex);
    void generateDijkstraSteps(const QString &startVertex);
    void generatePrimSteps(const QString &startVertex);
    void generateKruskalSteps();
    void generateTopologicalSortSteps();
    
    // 辅助方法
    void resetAlgorithmState();
    void addAlgorithmStep(const QString &description, 
                         const MyVectorQString &vertices = MyVectorQString(),
                         const MyVectorPairQStringQString &edges = MyVectorPairQStringQString(),
                         VisualState state = VisualState::Current,
                         int delay = 1000);
    
    // 布局算法辅助
    QPointF calculateCircularPosition(int index, int total, qreal radius = 200.0);
    void applyForceDirectedStep();
    
    // 图算法辅助
    MyVectorQString getNeighbors(const QString &vertex) const;
    bool hasPath(const QString &from, const QString &to) const;
    MyVectorPairQStringQString getAllEdgesSorted() const;
};

#endif // GRAPHMODEL_H
