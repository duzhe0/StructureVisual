#include "DijkstraTableDialog.h"
#include <QHeaderView>
#include <QDebug>
#include <climits>

DijkstraTableDialog::DijkstraTableDialog(const QString &startVertex, QWidget *parent)
    : QDialog(parent)
    , m_tableWidget(nullptr)
    , m_statusLabel(nullptr)
    , m_closeButton(nullptr)
    , m_startVertex(startVertex)
{
    setupUI();
}

void DijkstraTableDialog::setupUI()
{
    setWindowTitle(QString("Dijkstra算法执行表 - 起始顶点: %1").arg(m_startVertex));
    setModal(false);  // 非模态对话框，允许同时操作主窗口
    resize(600, 400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    m_statusLabel = new QLabel(QString("显示从顶点 %1 到各顶点的最短路径信息").arg(m_startVertex));
    mainLayout->addWidget(m_statusLabel);
    
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(4);
    QStringList headers;
    headers << "顶点" << "最短距离" << "前驱顶点" << "状态";
    m_tableWidget->setHorizontalHeaderLabels(headers);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 只读
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->verticalHeader()->setVisible(false);
    mainLayout->addWidget(m_tableWidget);
    
    m_closeButton = new QPushButton("关闭", this);
    connect(m_closeButton, &QPushButton::clicked, this, &DijkstraTableDialog::onCloseButtonClicked);
    mainLayout->addWidget(m_closeButton);
}

void DijkstraTableDialog::initializeTable(const QStringList &vertices)
{
    m_tableWidget->setRowCount(vertices.size());
    m_vertexToRow.clear();
    
    for (int i = 0; i < vertices.size(); ++i) {
        const QString &vertex = vertices[i];
        m_vertexToRow[vertex] = i;
        
        // 顶点名称
        QTableWidgetItem *vertexItem = new QTableWidgetItem(vertex);
        m_tableWidget->setItem(i, 0, vertexItem);
        
        // 最短距离（初始为无穷大）
        QTableWidgetItem *distanceItem = new QTableWidgetItem("∞");
        distanceItem->setData(Qt::UserRole, INT_MAX);  // 存储实际数值
        m_tableWidget->setItem(i, 1, distanceItem);
        
        // 前驱顶点（初始为空）
        QTableWidgetItem *predecessorItem = new QTableWidgetItem("-");
        m_tableWidget->setItem(i, 2, predecessorItem);
        
        // 状态（初始为未访问）
        QTableWidgetItem *statusItem = new QTableWidgetItem("未访问");
        m_tableWidget->setItem(i, 3, statusItem);
        
        // 如果是起始顶点，设置距离为0
        if (vertex == m_startVertex) {
            distanceItem->setText("0");
            distanceItem->setData(Qt::UserRole, 0);
            statusItem->setText("起始顶点");
            statusItem->setBackground(QBrush(QColor(200, 255, 200)));  // 浅绿色
        }
    }
    
    m_tableWidget->resizeColumnsToContents();
}

void DijkstraTableDialog::updateVertex(const QString &vertex, int distance, const QString &predecessor, bool visited)
{
    int row = findVertexRow(vertex);
    if (row < 0) {
        return;
    }
    
    // 更新最短距离
    QTableWidgetItem *distanceItem = m_tableWidget->item(row, 1);
    if (distanceItem) {
        if (distance == INT_MAX) {
            distanceItem->setText("∞");
        } else {
            distanceItem->setText(QString::number(distance));
        }
        distanceItem->setData(Qt::UserRole, distance);
    }
    
    // 更新前驱顶点
    QTableWidgetItem *predecessorItem = m_tableWidget->item(row, 2);
    if (predecessorItem) {
        if (predecessor.isEmpty() || predecessor == "-") {
            predecessorItem->setText("-");
        } else {
            predecessorItem->setText(predecessor);
        }
    }
    
    // 更新状态
    QTableWidgetItem *statusItem = m_tableWidget->item(row, 3);
    if (statusItem) {
        if (visited) {
            statusItem->setText("已访问");
            statusItem->setBackground(QBrush(QColor(200, 200, 255)));  // 浅蓝色
        } else {
            statusItem->setText("未访问");
            statusItem->setBackground(QBrush());  // 恢复默认背景
        }
    }
    
    m_tableWidget->resizeColumnsToContents();
}

void DijkstraTableDialog::resetTable()
{
    for (int i = 0; i < m_tableWidget->rowCount(); ++i) {
        const QString &vertex = m_tableWidget->item(i, 0)->text();
        
        // 重置距离
        QTableWidgetItem *distanceItem = m_tableWidget->item(i, 1);
        if (distanceItem) {
            if (vertex == m_startVertex) {
                distanceItem->setText("0");
                distanceItem->setData(Qt::UserRole, 0);
            } else {
                distanceItem->setText("∞");
                distanceItem->setData(Qt::UserRole, INT_MAX);
            }
        }
        
        // 重置前驱
        QTableWidgetItem *predecessorItem = m_tableWidget->item(i, 2);
        if (predecessorItem) {
            predecessorItem->setText("-");
        }
        
        // 重置状态
        QTableWidgetItem *statusItem = m_tableWidget->item(i, 3);
        if (statusItem) {
            if (vertex == m_startVertex) {
                statusItem->setText("起始顶点");
                statusItem->setBackground(QBrush(QColor(200, 255, 200)));
            } else {
                statusItem->setText("未访问");
                statusItem->setBackground(QBrush());
            }
        }
    }
}

int DijkstraTableDialog::findVertexRow(const QString &vertex) const
{
    if (m_vertexToRow.contains(vertex)) {
        return m_vertexToRow[vertex];
    }
    return -1;
}

void DijkstraTableDialog::onCloseButtonClicked()
{
    accept();
}

