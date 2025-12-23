#include "AdjacencyListRepresentationDialog.h"
#include <QMessageBox>
#include <QDebug>

AdjacencyListRepresentationDialog::AdjacencyListRepresentationDialog(GraphModel *model, QWidget *parent)
    : QDialog(parent)
    , m_model(model)
{
    setupUI();
    populateAdjacencyList();
}

void AdjacencyListRepresentationDialog::setupUI()
{
    setWindowTitle("图的邻接表表示");
    setModal(true);
    resize(800, 600);
    
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);
    
    m_statusLabel = new QLabel("编辑文本框后点击'应用更改'更新图结构");
    dialogLayout->addWidget(m_statusLabel);
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollContent = new QWidget();
    m_mainLayout = new QVBoxLayout(m_scrollContent);
    m_scrollArea->setWidget(m_scrollContent);
    dialogLayout->addWidget(m_scrollArea);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton("刷新", this);
    m_applyButton = new QPushButton("应用更改", this);
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_applyButton);
    
    connect(m_refreshButton, &QPushButton::clicked, this, &AdjacencyListRepresentationDialog::onRefreshButtonClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &AdjacencyListRepresentationDialog::onApplyButtonClicked);
    
    dialogLayout->addLayout(buttonLayout);
    
    QPushButton *closeButton = new QPushButton("关闭", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    dialogLayout->addWidget(closeButton);
}

void AdjacencyListRepresentationDialog::populateAdjacencyList()
{
    // 清除现有内容
    for (auto it = m_vertexRows.begin(); it != m_vertexRows.end(); ++it) {
        delete it.value();
    }
    m_vertexRows.clear();
    m_vertexEditors.clear();
    
    if (!m_model || m_model->isEmpty()) {
        m_statusLabel->setText("图为空，无法显示邻接表");
        return;
    }
    
    QMap<QString, QList<QPair<QString, int>>> adjList = m_model->getAdjacencyListRepresentation();
    
    // 获取所有顶点名称并排序
    QStringList vertices = adjList.keys();
    vertices.sort();
    
    // 为每个顶点创建一行
    for (const QString &vertex : vertices) {
        createAdjacencyRow(vertex, adjList[vertex]);
    }
    
    m_mainLayout->addStretch();
    m_statusLabel->setText("编辑文本框后点击'应用更改'更新图结构");
}

void AdjacencyListRepresentationDialog::createAdjacencyRow(const QString &vertex, const QList<QPair<QString, int>> &neighbors)
{
    QWidget *rowWidget = new QWidget();
    QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(5, 5, 5, 5);
    
    // 顶点名称标签（不可编辑）
    QLabel *vertexLabel = new QLabel(vertex + ":", rowWidget);
    vertexLabel->setMinimumWidth(80);
    rowLayout->addWidget(vertexLabel);
    
    // 存储该顶点的编辑器对
    QList<QPair<QLineEdit*, QLineEdit*>> editors;
    
    // 添加现有的邻居
    for (const auto &neighborPair : neighbors) {
        QString neighbor = neighborPair.first;
        int weight = neighborPair.second;
        
        // 箭头标签
        QLabel *arrowLabel = new QLabel("->", rowWidget);
        rowLayout->addWidget(arrowLabel);
        
        // 邻居名称文本框
        QLineEdit *neighborEdit = new QLineEdit(neighbor, rowWidget);
        neighborEdit->setMinimumWidth(60);
        neighborEdit->setPlaceholderText("邻居");
        rowLayout->addWidget(neighborEdit);
        
        // 权重文本框
        QLineEdit *weightEdit = new QLineEdit(QString::number(weight), rowWidget);
        weightEdit->setMinimumWidth(40);
        weightEdit->setPlaceholderText("权重");
        rowLayout->addWidget(weightEdit);
        
        editors.append(qMakePair(neighborEdit, weightEdit));
    }
    
    // 添加按钮
    QPushButton *addButton = new QPushButton("+", rowWidget);
    addButton->setMaximumWidth(30);
    addButton->setProperty("vertex", vertex);
    connect(addButton, &QPushButton::clicked, this, &AdjacencyListRepresentationDialog::onAddNeighborClicked);
    rowLayout->addWidget(addButton);
    
    QPushButton *removeButton = new QPushButton("-", rowWidget);
    removeButton->setMaximumWidth(30);
    removeButton->setProperty("vertex", vertex);
    connect(removeButton, &QPushButton::clicked, this, &AdjacencyListRepresentationDialog::onRemoveNeighborClicked);
    rowLayout->addWidget(removeButton);
    
    rowLayout->addStretch();
    
    m_mainLayout->addWidget(rowWidget);
    m_vertexRows[vertex] = rowWidget;
    m_vertexEditors[vertex] = editors;
}

void AdjacencyListRepresentationDialog::removeAdjacencyRow(const QString &vertex)
{
    if (m_vertexRows.contains(vertex)) {
        delete m_vertexRows[vertex];
        m_vertexRows.remove(vertex);
        m_vertexEditors.remove(vertex);
    }
}

void AdjacencyListRepresentationDialog::onAddNeighborClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QString vertex = button->property("vertex").toString();
    if (vertex.isEmpty()) return;
    
    // 找到对应的行
    QWidget *rowWidget = m_vertexRows[vertex];
    if (!rowWidget) return;
    
    QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(rowWidget->layout());
    if (!rowLayout) return;
    
    // 在最后一个按钮之前插入新的邻居编辑器
    int insertIndex = rowLayout->count() - 3; // 在"+"按钮之前
    
    // 箭头标签
    QLabel *arrowLabel = new QLabel("->", rowWidget);
    rowLayout->insertWidget(insertIndex, arrowLabel);
    
    // 邻居名称文本框
    QLineEdit *neighborEdit = new QLineEdit(rowWidget);
    neighborEdit->setMinimumWidth(60);
    neighborEdit->setPlaceholderText("邻居");
    rowLayout->insertWidget(insertIndex + 1, neighborEdit);
    
    // 权重文本框
    QLineEdit *weightEdit = new QLineEdit("1", rowWidget);
    weightEdit->setMinimumWidth(40);
    weightEdit->setPlaceholderText("权重");
    rowLayout->insertWidget(insertIndex + 2, weightEdit);
    
    // 添加到编辑器列表
    m_vertexEditors[vertex].append(qMakePair(neighborEdit, weightEdit));
}

void AdjacencyListRepresentationDialog::onRemoveNeighborClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QString vertex = button->property("vertex").toString();
    if (vertex.isEmpty()) return;
    
    // 找到对应的行
    QWidget *rowWidget = m_vertexRows[vertex];
    if (!rowWidget) return;
    
    QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(rowWidget->layout());
    if (!rowLayout) return;
    
    // 移除最后一个邻居编辑器（如果存在）
    if (!m_vertexEditors[vertex].isEmpty()) {
        QPair<QLineEdit*, QLineEdit*> editors = m_vertexEditors[vertex].last();
        m_vertexEditors[vertex].removeLast();
        
        // 找到并删除对应的箭头标签
        for (int i = 0; i < rowLayout->count(); ++i) {
            QLayoutItem *item = rowLayout->itemAt(i);
            if (item && item->widget() == editors.first) {
                // 删除箭头标签（在邻居编辑器之前）
                if (i > 0) {
                    QLayoutItem *prevItem = rowLayout->itemAt(i - 1);
                    if (prevItem && prevItem->widget()) {
                        QLabel *arrowLabel = qobject_cast<QLabel*>(prevItem->widget());
                        if (arrowLabel && arrowLabel->text() == "->") {
                            delete prevItem->widget();
                            delete prevItem;
                        }
                    }
                }
                // 删除邻居和权重编辑器
                delete editors.first;
                delete editors.second;
                break;
            }
        }
    }
}

void AdjacencyListRepresentationDialog::onRefreshButtonClicked()
{
    populateAdjacencyList();
}

void AdjacencyListRepresentationDialog::onApplyButtonClicked()
{
    updateGraphFromAdjacencyList();
}

void AdjacencyListRepresentationDialog::updateGraphFromAdjacencyList()
{
    if (!m_model) {
        return;
    }
    
    QMap<QString, QList<QPair<QString, int>>> adjList;
    
    // 从界面读取数据
    for (auto it = m_vertexEditors.begin(); it != m_vertexEditors.end(); ++it) {
        QString vertex = it.key();
        QList<QPair<QString, int>> neighbors;
        
        for (const auto &editorPair : it.value()) {
            QLineEdit *neighborEdit = editorPair.first;
            QLineEdit *weightEdit = editorPair.second;
            
            QString neighborText = neighborEdit->text().trimmed();
            if (neighborText.isEmpty()) {
                continue; // 跳过空的邻居
            }
            
            QString weightText = weightEdit->text().trimmed();
            bool ok;
            int weight = weightText.toInt(&ok);
            
            if (!ok || weight <= 0) {
                weight = 1; // 默认权重为1
            }
            
            neighbors.append(qMakePair(neighborText, weight));
        }
        
        adjList[vertex] = neighbors;
    }
    
    // 更新图结构
    m_model->updateFromAdjacencyList(adjList);
    
    m_statusLabel->setText("图结构已更新");
    QMessageBox::information(this, "成功", "图结构已根据邻接表更新");
}

void AdjacencyListRepresentationDialog::onNeighborTextChanged()
{
    // 可以在这里添加实时验证逻辑
}

