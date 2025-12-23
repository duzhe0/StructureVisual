#ifndef GRAPH_H
#define GRAPH_H

#include "IGraph.h"

class Graph:public IGraph{
private:
    int m_num_vertex;	//顶点的个数
    int m_num_edge;	//边的条数
    bool* m_visited;	//访问的标记
    int* m_indegree;	//入度
public:
    Graph(int numvertex);
    virtual ~Graph();
    int vecticesNumber();	//获得顶点个数
    int edgesNumber();	//获得边的条数
    Edge firstEdge(int onevertex);
    Edge nextEdge(Edge pre);
    void setEdge(int from, int to, int weight);
    void delEdge(int from, int to);
    bool isEdge(Edge edge);
    void traverseGraph(Graph& graph);
    void doTraverse(Graph& graph,int start_vertex);
};

Graph::Graph(int numvertex){
    this->m_num_vertex = numvertex;
    this->m_num_edge = 0;
    this->m_indegree = new int[numvertex];
    this->m_visited = new bool[numvertex];
    for(int i = 0; i < numvertex; i++ ){
        this->m_visited[i] = false;
        this->m_indegree[i] = 0;
    }
}

Graph::~Graph(){
    if(this->m_indegree != 0){
        delete[ ] this->m_indegree;
        this->m_indegree = 0;
    }
    if(this->m_visited != 0){
        delete[ ] this->m_visited;
        this->m_visited = 0;
    }
}

void Graph::traverseGraph(Graph& graph){
    //对图所有顶点的标志位进行初始化
    for(int i = 0; i < this->m_num_vertex; i++ ){
        this->m_visited[i] = false;
    }
    /*      检查图的所有顶点是否被标记过，如果未被标记，则从该未被标记的顶点     开始继续周游，doTraverse函数可用深度优先或者广度优先  */
    for(int i= 0; i< graph.vecticesNumber();i++){
        if(graph.m_visited[i] == false){
            this->doTraverse(graph, i);
        }
    }
}

#endif // GRAPH_H
