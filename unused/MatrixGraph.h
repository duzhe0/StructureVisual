#ifndef MATRIXGRAPH_H
#define MATRIXGRAPH_H

#include "Graph.h"
#include "Iterator.h"
#include <cstring>

template<typename T>
class MatrixGraphIterator:public Iterator<T>{
public:
    MatrixGraphIterator(const MatrixGraph* graph){
        graph_=graph;
        iter_visited=graph->m_visited;
        begin_vertex=graph->iter_begin_vertex;
        current_vertex=graph->vecticesNumber();
    }
    void First(){
        current_vertex=begin_vertex;
    }
    bool isDone() const{
        for(int i=0;i<vertex_num;i++){
            if(iter_visited[i]==false)
                return false;
        }
        return true;
    }
    T CurrentItem() const{}
private:
    MatrixGraph* graph_;
    int vertex_num;
    bool* iter_visited;
    int begin_vertex;
    int current_vertex;
};

class MatrixGraph:public Graph{
private:
    int** m_matrix;
    int iter_begin_vertex;
public:
    MatrixGraph(int numvertex);
    virtual ~MatrixGraph();

    bool isEdge(Edge edge);
    Edge firstEdge(int onevertex);
    Edge nextEdge(Edge pre);
    void setEdge(int from, int to, int weight);
    void delEdge(int from, int to);
    void dumpGraph();
    void set_begin_vertex(int vertex);
    Iterator createIterator();
};

MatrixGraph::set_begin_vertex(int vertex){
    iter_begin_vertex=vertex;
}

MatrixGraph::MatrixGraph(int numvertex):Graph(numvertex){
    this->m_matrix = (int**)new int*[this->vecticesNumber()];
    for( int i = 0; i < this->vecticesNumber(); i++){
        this->m_matrix[i] = new int [this->vecticesNumber()];
    }
    for( int i = 0; i < this->vecticesNumber(); i++){
        for( int j = 0 ; j < this->vecticesNumber(); j++){
            this->m_matrix[i][j] = 0;
        }
    }
    iter_begin_vertex=0;
}

MatrixGraph::~MatrixGraph(){
    if(this->m_matrix != 0){
        for(int i = 0; i < this->vecticesNumber(); i++){
            if(this->m_matrix[i] != 0 ){
                delete[] this->m_matrix[i];
                this->m_matrix[i] = 0;
            }
        }
        delete[] this->m_matrix;
        this->m_matrix = 0;
    }
}

Edge MatrixGraph::firstEdge(int onevertex){
    Edge myedge;
    myedge.setFrom(onevertex);
    for(int i=0;i<this->vecticesNumber();i++){
        if(this->m_matrix[onevertex][i]!=0){
            myedge.setTo(i);
            myedge.setWeight(this->m_matrix[onevertex][i]);
            break;
        }
    }
    return myedge;
}

Edge MatrixGraph::nextEdge(Edge pre){
    Edge myedge;
    myedge.setFrom(pre.getFrom());
    if(pre.getTo() < this->vecticesNumber()){
        for(int i = pre.getTo()+1;i<this->vecticesNumber();i++){
            if(this->m_matrix[pre.getFrom()][i]!=0){
                myedge.setTo(i);
                myedge.setWeight(this->m_matrix[pre.getFrom()][i]);
                break;
            }
        }
    }
    return myedge;
}

void MatrixGraph::setEdge(int from, int to, int weight){
    if(this->m_matrix[from][to] <= 0){
        this->incEdgeNumber();		//边的条数++
        this->incIndegree(to);	//顶点to的度++
    }
    this->m_matrix[from][to] = weight;
}

void MatrixGraph::delEdge(int from, int to){
    if(this->m_matrix[from][to] > 0){
        this->decEdgeNumber(); 	//边的条数--
        this->decIndegree(to); 	//顶点to的度--
    }
    this->m_matrix[from][to] = 0;
}

Iterator MatrixGraph::createIterator(){
    return new MatrixGraphIterator(this);
}
#endif // MATRIXGRAPH_H
