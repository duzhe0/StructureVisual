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
    
    m_statusLabel = new QLabel("编辑文本框后会自动更新图结构");
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
    m_originalNeighbors.clear();
    
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
    m_statusLabel->setText("编辑文本框后会自动更新图结构");
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
        neighborEdit->setProperty("vertex", vertex); // 存储所属顶点
        connect(neighborEdit, &QLineEdit::editingFinished, this, &AdjacencyListRepresentationDialog::onNeighborTextChanged);
        rowLayout->addWidget(neighborEdit);
        
        // 权重文本框
        QLineEdit *weightEdit = new QLineEdit(QString::number(weight), rowWidget);
        weightEdit->setMinimumWidth(40);
        weightEdit->setPlaceholderText("权重");
        weightEdit->setProperty("vertex", vertex); // 存储所属顶点
        connect(weightEdit, &QLineEdit::editingFinished, this, &AdjacencyListRepresentationDialog::onNeighborTextChanged);
        rowLayout->addWidget(weightEdit);
        
        // 为每个邻居添加删除按钮
        QPushButton *removeNeighborButton = new QPushButton("×", rowWidget);
        removeNeighborButton->setMaximumWidth(25);
        removeNeighborButton->setMaximumHeight(25);
        removeNeighborButton->setProperty("vertex", vertex);
        // 使用setProperty存储指针地址（作为quintptr），避免QVariant::fromValue的问题
        removeNeighborButton->setProperty("neighborEditPtr", reinterpret_cast<quintptr>(neighborEdit));
        connect(removeNeighborButton, &QPushButton::clicked, this, &AdjacencyListRepresentationDialog::onRemoveSpecificNeighborClicked);
        rowLayout->addWidget(removeNeighborButton);
        
        editors.append(qMakePair(neighborEdit, weightEdit));
        m_originalNeighbors[neighborEdit] = neighbor; // 记录原始邻居名称
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
    // 检查算法是否正在运行
    if (m_model && m_model->isAlgorithmRunning()) {
        QMessageBox::warning(this, "操作被禁止", "算法正在执行中，无法修改图结构。请先停止算法。");
        return;
    }
    
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
    neighborEdit->setProperty("vertex", vertex); // 存储所属顶点
    connect(neighborEdit, &QLineEdit::editingFinished, this, &AdjacencyListRepresentationDialog::onNeighborTextChanged);
    rowLayout->insertWidget(insertIndex + 1, neighborEdit);
    
    // 权重文本框
    QLineEdit *weightEdit = new QLineEdit("1", rowWidget);
    weightEdit->setMinimumWidth(40);
    weightEdit->setPlaceholderText("权重");
    weightEdit->setProperty("vertex", vertex); // 存储所属顶点
    connect(weightEdit, &QLineEdit::editingFinished, this, &AdjacencyListRepresentationDialog::onNeighborTextChanged);
    rowLayout->insertWidget(insertIndex + 2, weightEdit);
    
    // 为每个邻居添加删除按钮
    QPushButton *removeNeighborButton = new QPushButton("×", rowWidget);
    removeNeighborButton->setMaximumWidth(25);
    removeNeighborButton->setMaximumHeight(25);
    removeNeighborButton->setProperty("vertex", vertex);
    // 使用setProperty存储指针地址（作为quintptr），避免QVariant::fromValue的问题
    removeNeighborButton->setProperty("neighborEditPtr", reinterpret_cast<quintptr>(neighborEdit));
    connect(removeNeighborButton, &QPushButton::clicked, this, &AdjacencyListRepresentationDialog::onRemoveSpecificNeighborClicked);
    rowLayout->insertWidget(insertIndex + 3, removeNeighborButton);
    
    // 添加到编辑器列表
    m_vertexEditors[vertex].append(qMakePair(neighborEdit, weightEdit));
    m_originalNeighbors[neighborEdit] = ""; // 新添加的，原始为空
    
    // 提示用户输入邻居名称
    m_statusLabel->setText(QString("已添加新的邻居编辑器，请输入邻居名称和权重"));
}

void AdjacencyListRepresentationDialog::onRemoveNeighborClicked()
{
    // 检查算法是否正在运行
    if (m_model && m_model->isAlgorithmRunning()) {
        QMessageBox::warning(this, "操作被禁止", "算法正在执行中，无法修改图结构。请先停止算法。");
        return;
    }
    
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    QString vertex = button->property("vertex").toString();
    if (vertex.isEmpty()) return;
    
    // 移除最后一个邻居编辑器（如果存在）
    if (!m_vertexEditors[vertex].isEmpty()) {
        QPair<QLineEdit*, QLineEdit*> editors = m_vertexEditors[vertex].last();
        QString oldNeighbor = editors.first->text().trimmed();
        
        // 如果邻居名称不为空，从图中删除对应的边
        if (!oldNeighbor.isEmpty() && m_model) {
            m_model->removeEdge(vertex, oldNeighbor);
            m_statusLabel->setText(QString("已删除边: %1 -> %2").arg(vertex).arg(oldNeighbor));
        }
        
        // 删除UI组件
        removeNeighborEditor(vertex, editors.first);
    }
}

void AdjacencyListRepresentationDialog::onRemoveSpecificNeighborClicked()
{
    // 检查算法是否正在运行
    if (m_model && m_model->isAlgorithmRunning()) {
        QMessageBox::warning(this, "操作被禁止", "算法正在执行中，无法修改图结构。请先停止算法。");
        return;
    }
    
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button || !m_model) return;
    
    QString vertex = button->property("vertex").toString();
    if (vertex.isEmpty()) return;
    
    // 找到对应的行
    QWidget *rowWidget = m_vertexRows.value(vertex);
    if (!rowWidget) return;
    
    QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(rowWidget->layout());
    if (!rowLayout) return;
    
    // 找到按钮在布局中的位置
    int buttonIndex = -1;
    for (int i = 0; i < rowLayout->count(); ++i) {
        QLayoutItem *item = rowLayout->itemAt(i);
        if (item && item->widget() == button) {
            buttonIndex = i;
            break;
        }
    }
    
    if (buttonIndex < 2) {
        qDebug() << "按钮位置无效";
        return;
    }
    
    // 按钮前面应该是权重编辑器，再前面是邻居编辑器，再前面是箭头标签
    // 布局顺序：箭头 -> 邻居编辑器 -> 权重编辑器 -> 删除按钮
    QLayoutItem *weightItem = rowLayout->itemAt(buttonIndex - 1);
    QLayoutItem *neighborItem = rowLayout->itemAt(buttonIndex - 2);
    
    if (!neighborItem || !neighborItem->widget()) {
        qDebug() << "无法找到邻居编辑器";
        return;
    }
    
    QLineEdit *neighborEdit = qobject_cast<QLineEdit*>(neighborItem->widget());
    if (!neighborEdit) {
        qDebug() << "邻居编辑器类型错误";
        return;
    }
    
    // 获取要删除的邻居名称
    QString oldNeighbor = neighborEdit->text().trimmed();
    if (oldNeighbor.isEmpty()) {
        // 如果邻居名称为空，从m_originalNeighbors中获取
        oldNeighbor = m_originalNeighbors.value(neighborEdit, "");
    }
    
    // 如果邻居名称不为空，从图中删除对应的边
    if (!oldNeighbor.isEmpty()) {
        m_model->removeEdge(vertex, oldNeighbor);
        m_statusLabel->setText(QString("已删除边: %1 -> %2").arg(vertex).arg(oldNeighbor));
    } else {
        m_statusLabel->setText(QString("已删除空的邻居编辑器"));
    }
    
    // 删除UI组件
    removeNeighborEditor(vertex, neighborEdit);
}

void AdjacencyListRepresentationDialog::removeNeighborEditor(const QString &vertex, QLineEdit *neighborEdit)
{
    // 找到对应的行
    QWidget *rowWidget = m_vertexRows.value(vertex);
    if (!rowWidget) return;
    
    QHBoxLayout *rowLayout = qobject_cast<QHBoxLayout*>(rowWidget->layout());
    if (!rowLayout) return;
    
    // 找到对应的权重编辑器
    QLineEdit *weightEdit = nullptr;
    int editorIndex = -1;
    for (int i = 0; i < m_vertexEditors[vertex].size(); ++i) {
        if (m_vertexEditors[vertex][i].first == neighborEdit) {
            weightEdit = m_vertexEditors[vertex][i].second;
            editorIndex = i;
            break;
        }
    }
    
    if (editorIndex == -1) {
        qDebug() << "无法在编辑器列表中找到邻居编辑器";
        return;
    }
    
    // 先找到所有要删除的组件在布局中的位置
    int neighborEditIndex = -1;
    int weightEditIndex = -1;
    int removeButtonIndex = -1;
    int arrowLabelIndex = -1;
    
    // 首先找到邻居编辑器和权重编辑器的位置
    for (int i = 0; i < rowLayout->count(); ++i) {
        QLayoutItem *item = rowLayout->itemAt(i);
        if (!item || !item->widget()) continue;
        
        QWidget *widget = item->widget();
        if (widget == neighborEdit) {
            neighborEditIndex = i;
        } else if (widget == weightEdit) {
            weightEditIndex = i;
        }
    }
    
    // 找到箭头标签（在邻居编辑器之前）
    if (neighborEditIndex > 0) {
        QLayoutItem *prevItem = rowLayout->itemAt(neighborEditIndex - 1);
        if (prevItem && prevItem->widget()) {
            QLabel *label = qobject_cast<QLabel*>(prevItem->widget());
            if (label && label->text() == "->") {
                arrowLabelIndex = neighborEditIndex - 1;
            }
        }
    }
    
    // 找到删除按钮（在权重编辑器之后）
    if (weightEditIndex >= 0 && weightEditIndex + 1 < rowLayout->count()) {
        QLayoutItem *nextItem = rowLayout->itemAt(weightEditIndex + 1);
        if (nextItem && nextItem->widget()) {
            QPushButton *button = qobject_cast<QPushButton*>(nextItem->widget());
            if (button && button->text() == "×") {
                removeButtonIndex = weightEditIndex + 1;
            }
        }
    }
    
    // 从编辑器列表中移除（在删除UI之前）
    m_vertexEditors[vertex].removeAt(editorIndex);
    m_originalNeighbors.remove(neighborEdit);
    
    // 按从后往前的顺序删除UI组件，避免索引变化
    // 删除删除按钮
    if (removeButtonIndex >= 0) {
        QLayoutItem *item = rowLayout->takeAt(removeButtonIndex);
        if (item && item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    // 删除权重编辑器
    if (weightEditIndex >= 0) {
        QLayoutItem *item = rowLayout->takeAt(weightEditIndex);
        if (item && item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    // 删除邻居编辑器
    if (neighborEditIndex >= 0) {
        QLayoutItem *item = rowLayout->takeAt(neighborEditIndex);
        if (item && item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    // 删除箭头标签
    if (arrowLabelIndex >= 0) {
        QLayoutItem *item = rowLayout->takeAt(arrowLabelIndex);
        if (item && item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
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
    if (!m_model) {
        return;
    }
    
    QLineEdit *senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit) {
        return;
    }
    
    QString vertex = senderEdit->property("vertex").toString();
    if (vertex.isEmpty()) {
        return;
    }
    
    // 找到对应的编辑器对
    QLineEdit *neighborEdit = nullptr;
    QLineEdit *weightEdit = nullptr;
    
    // 遍历该顶点的所有编辑器对，找到包含senderEdit的对
    for (const auto &editorPair : m_vertexEditors[vertex]) {
        if (editorPair.first == senderEdit) {
            // 发送者是邻居编辑器
            neighborEdit = editorPair.first;
            weightEdit = editorPair.second;
            break;
        } else if (editorPair.second == senderEdit) {
            // 发送者是权重编辑器
            neighborEdit = editorPair.first;
            weightEdit = editorPair.second;
            break;
        }
    }
    
    if (!neighborEdit || !weightEdit) {
        return;
    }
    
    // 检查算法是否正在运行
    if (m_model->isAlgorithmRunning()) {
        QMessageBox::warning(this, "操作被禁止", "算法正在执行中，无法修改图结构。请先停止算法。");
        // 恢复原值
        QString originalNeighbor = m_originalNeighbors.value(neighborEdit, "");
        if (senderEdit == neighborEdit) {
            // 如果是邻居编辑器，恢复原始邻居名称
            neighborEdit->setText(originalNeighbor);
        } else if (senderEdit == weightEdit) {
            // 如果是权重编辑器，恢复原始权重（从图中获取）
            if (!originalNeighbor.isEmpty()) {
                EdgeItem *edge = m_model->getEdge(vertex, originalNeighbor);
                if (edge) {
                    weightEdit->setText(QString::number(edge->getWeight()));
                }
            }
        }
        return;
    }
    
    QString newNeighbor = neighborEdit->text().trimmed();
    QString weightText = weightEdit->text().trimmed();
    bool ok;
    int weight = weightText.toInt(&ok);
    
    if (!ok || weight <= 0) {
        weight = 1; // 默认权重为1
    }
    
    // 获取原始邻居名称
    QString oldNeighbor = m_originalNeighbors.value(neighborEdit, "");
    
    // 如果新邻居名称为空，删除边（如果存在）
    if (newNeighbor.isEmpty()) {
        if (!oldNeighbor.isEmpty()) {
            m_model->removeEdge(vertex, oldNeighbor);
            m_statusLabel->setText(QString("已删除边: %1 -> %2").arg(vertex).arg(oldNeighbor));
            m_originalNeighbors[neighborEdit] = ""; // 更新原始邻居
        }
        return;
    }
    
    // 检查新邻居顶点是否存在
    if (!m_model->getVertex(newNeighbor)) {
        m_statusLabel->setText(QString("警告: 顶点 '%1' 不存在").arg(newNeighbor));
        // 不更新图结构，但允许用户继续编辑
        return;
    }
    
    // 如果邻居名称改变了，需要删除旧边，添加新边
    if (oldNeighbor != newNeighbor) {
        // 删除旧边（如果存在且不为空）
        if (!oldNeighbor.isEmpty()) {
            m_model->removeEdge(vertex, oldNeighbor);
        }
        
        // 添加新边
        if (m_model->addEdge(vertex, newNeighbor, weight)) {
            m_statusLabel->setText(QString("已添加边: %1 -> %2 = %3").arg(vertex).arg(newNeighbor).arg(weight));
            m_originalNeighbors[neighborEdit] = newNeighbor; // 更新原始邻居
        } else {
            m_statusLabel->setText(QString("添加边失败: %1 -> %2").arg(vertex).arg(newNeighbor));
        }
    } else {
        // 邻居名称没变，只更新权重
        EdgeItem *existingEdge = m_model->getEdge(vertex, newNeighbor);
        if (existingEdge) {
            existingEdge->setWeight(weight);
            m_statusLabel->setText(QString("已更新边权重: %1 -> %2 = %3").arg(vertex).arg(newNeighbor).arg(weight));
        } else {
            // 边不存在，添加新边
            if (m_model->addEdge(vertex, newNeighbor, weight)) {
                m_statusLabel->setText(QString("已添加边: %1 -> %2 = %3").arg(vertex).arg(newNeighbor).arg(weight));
                m_originalNeighbors[neighborEdit] = newNeighbor;
            }
        }
    }
}

