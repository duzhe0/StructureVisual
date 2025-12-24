#ifndef ADJACENCYLISTREPRESENTATIONDIALOG_H
#define ADJACENCYLISTREPRESENTATIONDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QList>
#include "GraphModel.h"

class AdjacencyListRepresentationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdjacencyListRepresentationDialog(GraphModel *model, QWidget *parent = nullptr);
    ~AdjacencyListRepresentationDialog() = default;

private slots:
    void onNeighborTextChanged();
    void onAddNeighborClicked();
    void onRemoveNeighborClicked();
    void onRemoveSpecificNeighborClicked();
    void onRefreshButtonClicked();
    void onApplyButtonClicked();

private:
    void setupUI();
    void populateAdjacencyList();
    void updateGraphFromAdjacencyList();
    void createAdjacencyRow(const QString &vertex, const QList<QPair<QString, int>> &neighbors);
    void removeAdjacencyRow(const QString &vertex);
    void removeNeighborEditor(const QString &vertex, QLineEdit *neighborEdit);
    
    GraphModel *m_model;
    QScrollArea *m_scrollArea;
    QWidget *m_scrollContent;
    QVBoxLayout *m_mainLayout;
    QPushButton *m_refreshButton;
    QPushButton *m_applyButton;
    QLabel *m_statusLabel;
    
    // 存储每个顶点的行组件
    QMap<QString, QWidget*> m_vertexRows;
    QMap<QString, QList<QPair<QLineEdit*, QLineEdit*>>> m_vertexEditors; // 存储 (neighbor, weight) 编辑器对
    QMap<QLineEdit*, QString> m_originalNeighbors; // 存储每个编辑器对应的原始邻居名称，用于跟踪变化
};

#endif // ADJACENCYLISTREPRESENTATIONDIALOG_H

