#ifndef IGRAPH_H
#define IGRAPH_H

#include"Iterable.h"

class Edge{
private:
    int m_from;
    int m_to;
    int m_weight;
public:
    Edge();
    Edge(int from, int to, int weight);

    int getFrom();
    void setFrom(int from);
    int getTo();
    void setTo(int to);
    int getWeight();
    void setWeight(int weight);
};

class IGraph:public Iterable{				//图的ADT
public:
    int vecticesNumber();			//返回图的顶点个数
    int edgesNumber();			//返回图的边数
    Edge firstEdge(int onevertex);	//返回与顶点关联的第一条边
    Edge nextEdge(Edge pre); 		//返回区相关关联顶点的下一条边
    void setEdge(int from, int to, int weight);	//添加一条边
    void delEdge(int from, int to);	//删除一条边
    bool isEdge(Edge edge);		//判断是否是该图的一条边
    int fromVertex(Edge edge);		//返回边的始点
    int toVertex(Edge edge);		//返回边的终点
    int weight(Edge edge);		//返回边的权值
};


#endif // IGRAPH_H
