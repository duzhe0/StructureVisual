#ifndef LISTGRAPH_H
#define LISTGRAPH_H

#include "Graph.h"
#include "Iterator.h"

class ListGraphIterator:public Iterator{

};

struct ListUnit{	//边结点的结构体定义
    int m_vertex;
    int m_weight;
};

template< typename T >
class Link{ //链表结点类的定义
private:
    T m_element;
    Link< T >* m_next;
public:
    Link(const T& element, Link< T >* next = 0);
    Link(Link< T >* next = 0);
    ~Link();
    void setElement(const T& element);
    T& getElement();
    void setNext(Link< T >* next);
    Link< T >* getNext();
};

template< typename T >
class LinkedList{	//链表类的定义
private:
    Link< T >* m_head;
public:
    LinkedList();
    ~LinkedList();
    Link< T >* getHead();
    void setHead(Link< T >* head);
};

class ListGraph: public Graph{
private:
    LinkedList<ListUnit>* m_graph_list;

public:
    ListGraph(int numvertex);
    ~ListGraph();
    Edge firstEdge(int onevertex);
    Edge nextEdge(Edge pre);
    void setEdge(int from, int to, int weight);
    void delEdge(int from, int to);
    bool isEdge(Edge edge);
};

Edge ListGraph::firstEdge(int onevertex){
    Edge edge;
    edge.setFrom(onevertex);
    Link<ListUnit>* temp=this->m_graph_list[onevertex].getHead();
    if(temp->getNext()!= 0){
        edge.setTo(temp->getNext()->getElement().m_vertex);
        edge.setWeight(temp->getNext()->getElement().m_weight);
    }
    return edge;
}

Edge ListGraph::nextEdge(Edge pre){
    Edge edge;
    edge.setFrom(pre.getFrom());
    Link<ListUnit>* temp = 	this->m_graph_list[pre.getFrom()].getHead();
    while(temp->getNext() != 0 && 	    temp->getNext()->getElement().m_vertex <= pre.getTo()){
        temp= temp->getNext();
    }
    if(temp->getNext() != 0){
        edge.setTo(temp->getNext()->getElement().m_vertex);
        edge.setWeight(temp->getNext()->getElement().m_weight);
    }
    return edge;
}

void ListGraph::setEdge(int from, int to, int weight){
    Link<ListUnit>* temp = this->m_graph_list[from].getHead();
    while(temp->getNext()!=0&&temp->getNext()->getElement().m_vertex<to){
        temp = temp->getNext();
    }
    if(temp->getNext() == 0){	//边不存在且最后一条边
        temp->setNext(new Link<ListUnit>());
        temp->getNext()->getElement().setVertex(to);
        temp->getNext()->getElement().setWeight(weight);
        this->incEdgeNumber();
        this->incIndegree(to);
        return;
    }
    if(temp->getNext()->getElement().m_vertex == to){//边存在
        temp->getNext()->getElement().setWeight(weight);
        return;
    }
    if(temp->getNext()->getElement().m_vertex > to){//边不存在，但后面还有边
        Link< ListUnit>* other = temp->getNext();
        temp->setNext(new Link<ListUnit>());
        temp->getNext()->getElement().setVertex(to);
        temp->getNext()->getElement().setWeight(to);
        temp->getNext()->setNext(other);
        this->incEdgeNumber();
        this->incIndegree(to);
        return;
    }
}

void ListGraph::delEdge(int from, int to){
    Link<ListUnit>* temp = this->m_graph_list[from].getHead();
    while(temp->getNext()!= 0 &&         temp->getNext()->getElement().m_vertex < to){
        temp = temp->getNext();
    }
    if(temp->getNext()== 0){
        return;
    }
    if(temp->getNext()->getElement().m_vertex > to){
        return;
    }
    if(temp->getNext()->getElement().m_vertex == to){
        Link<ListUnit>* other = temp->getNext()->getNext();
        delete temp->getNext();
        temp->setNext(other);
        this->decEdgeNumber();
        this->decIndegree(to);
        return;
    }
}

Iterator ListGraph::createIterator(){

}

#endif // LISTGRAPH_H
