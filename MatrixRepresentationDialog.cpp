#include "MatrixRepresentationDialog.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QBrush>
#include <QDebug>

MatrixRepresentationDialog::MatrixRepresentationDialog(GraphModel *model, QWidget *parent)
    : QDialog(parent)
    , m_model(model)
    , m_updating(false)
{
    setupUI();
    populateMatrix();
}

void MatrixRepresentationDialog::setupUI()
{
    setWindowTitle("图的矩阵表示");
    setModal(true);
    resize(600, 500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    m_statusLabel = new QLabel("双击单元格编辑，修改后会自动更新图结构");
    mainLayout->addWidget(m_statusLabel);
    
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    mainLayout->addWidget(m_tableWidget);
    
    connect(m_tableWidget, &QTableWidget::cellChanged, this, &MatrixRepresentationDialog::onCellChanged);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton("刷新", this);
    m_applyButton = new QPushButton("应用更改", this);
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_applyButton);
    
    connect(m_refreshButton, &QPushButton::clicked, this, &MatrixRepresentationDialog::onRefreshButtonClicked);
    connect(m_applyButton, &QPushButton::clicked, this, &MatrixRepresentationDialog::onApplyButtonClicked);
    
    mainLayout->addLayout(buttonLayout);
    
    QPushButton *closeButton = new QPushButton("关闭", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeButton);
}

void MatrixRepresentationDialog::populateMatrix()
{
    if (!m_model || m_model->isEmpty()) {
        m_tableWidget->setRowCount(0);
        m_tableWidget->setColumnCount(0);
        m_statusLabel->setText("图为空，无法显示矩阵");
        return;
    }
    
    m_updating = true;
    
    QMap<QString, QMap<QString, int>> matrix = m_model->getMatrixRepresentation();
    
    // 获取所有顶点名称并排序
    QStringList vertices = matrix.keys();
    vertices.sort();
    
    int size = vertices.size();
    m_tableWidget->setRowCount(size + 1);
    m_tableWidget->setColumnCount(size + 1);
    
    // 设置第一行第一列为空
    QTableWidgetItem *emptyItem = new QTableWidgetItem("");
    emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsEditable);
    m_tableWidget->setItem(0, 0, emptyItem);
    
    // 设置第一行和第一列的顶点名称（不可编辑）
    for (int i = 0; i < size; ++i) {
        QString vertex = vertices[i];
        
        // 第一行（列标题）
        QTableWidgetItem *headerItem = new QTableWidgetItem(vertex);
        headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsEditable);
        headerItem->setBackground(QBrush(QColor(240, 240, 240)));
        m_tableWidget->setItem(0, i + 1, headerItem);
        
        // 第一列（行标题）
        QTableWidgetItem *rowHeaderItem = new QTableWidgetItem(vertex);
        rowHeaderItem->setFlags(rowHeaderItem->flags() & ~Qt::ItemIsEditable);
        rowHeaderItem->setBackground(QBrush(QColor(240, 240, 240)));
        m_tableWidget->setItem(i + 1, 0, rowHeaderItem);
        
        // 填充矩阵数据
        for (int j = 0; j < size; ++j) {
            QString to = vertices[j];
            int weight = matrix[vertex][to];
            
            QTableWidgetItem *item = new QTableWidgetItem();
            if (weight == -1) {
                item->setText("-1");
            } else {
                item->setText(QString::number(weight));
            }
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            m_tableWidget->setItem(i + 1, j + 1, item);
        }
    }
    
    // 调整列宽
    m_tableWidget->resizeColumnsToContents();
    m_tableWidget->resizeRowsToContents();
    
    m_updating = false;
    m_statusLabel->setText("双击单元格编辑，修改后会自动更新图结构");
}

void MatrixRepresentationDialog::onCellChanged(int row, int column)
{
    if (m_updating || row == 0 || column == 0) {
        return;
    }
    
    // 检查算法是否正在运行
    if (m_model && m_model->isAlgorithmRunning()) {
        QMessageBox::warning(this, "操作被禁止", "算法正在执行中，无法修改图结构。请先停止算法。");
        m_updating = true;
        // 恢复原值
        QTableWidgetItem *rowHeader = m_tableWidget->item(row, 0);
        QTableWidgetItem *colHeader = m_tableWidget->item(0, column);
        if (rowHeader && colHeader) {
            QString from = rowHeader->text();
            QString to = colHeader->text();
            QMap<QString, QMap<QString, int>> matrix = m_model->getMatrixRepresentation();
            int weight = matrix[from][to];
            QTableWidgetItem *item = m_tableWidget->item(row, column);
            if (item) {
                if (weight == -1) {
                    item->setText("-1");
                } else {
                    item->setText(QString::number(weight));
                }
            }
        }
        m_updating = false;
        return;
    }
    
    // 获取顶点名称
    QTableWidgetItem *rowHeader = m_tableWidget->item(row, 0);
    QTableWidgetItem *colHeader = m_tableWidget->item(0, column);
    
    if (!rowHeader || !colHeader) {
        return;
    }
    
    QString from = rowHeader->text();
    QString to = colHeader->text();
    
    // 获取新值
    QTableWidgetItem *item = m_tableWidget->item(row, column);
    if (!item) {
        return;
    }
    
    QString text = item->text().trimmed();
    bool ok;
    int value = text.toInt(&ok);
    
    if (!ok) {
        QMessageBox::warning(this, "输入错误", QString("无效的数值: %1").arg(text));
        m_updating = true;
        // 恢复原值
        QMap<QString, QMap<QString, int>> matrix = m_model->getMatrixRepresentation();
        int weight = matrix[from][to];
        if (weight == -1) {
            item->setText("-1");
        } else {
            item->setText(QString::number(weight));
        }
        m_updating = false;
        return;
    }
    
    // 获取当前图结构中的旧值
    QMap<QString, QMap<QString, int>> currentMatrix = m_model->getMatrixRepresentation();
    int oldValue = currentMatrix[from][to];
    
    // 立即更新图结构
    if (value == -1) {
        // 如果新值是-1，删除边（如果存在）
        if (oldValue != -1) {
            m_model->removeEdge(from, to);
            m_statusLabel->setText(QString("已删除边: %1 -> %2").arg(from).arg(to));
        }
    } else {
        // 如果新值不是-1，更新或添加边
        EdgeItem *existingEdge = m_model->getEdge(from, to);
        if (existingEdge) {
            // 边已存在，更新权重
            existingEdge->setWeight(value);
            m_statusLabel->setText(QString("已更新边权重: %1 -> %2 = %3").arg(from).arg(to).arg(value));
        } else {
            // 边不存在，添加新边
            if (m_model->addEdge(from, to, value)) {
                m_statusLabel->setText(QString("已添加边: %1 -> %2 = %3").arg(from).arg(to).arg(value));
            } else {
                m_statusLabel->setText(QString("添加边失败: %1 -> %2").arg(from).arg(to));
            }
        }
    }
    
    // 如果是无向图，同步更新对称位置
    if (!m_model->isDirected() && from != to) {
        // 找到对称位置
        int symRow = -1, symCol = -1;
        for (int i = 1; i < m_tableWidget->rowCount(); ++i) {
            QTableWidgetItem *rHeader = m_tableWidget->item(i, 0);
            if (rHeader && rHeader->text() == to) {
                symRow = i;
                break;
            }
        }
        for (int j = 1; j < m_tableWidget->columnCount(); ++j) {
            QTableWidgetItem *cHeader = m_tableWidget->item(0, j);
            if (cHeader && cHeader->text() == from) {
                symCol = j;
                break;
            }
        }
        
        if (symRow > 0 && symCol > 0) {
            QTableWidgetItem *symItem = m_tableWidget->item(symRow, symCol);
            if (symItem) {
                m_updating = true;
                if (value == -1) {
                    symItem->setText("-1");
                } else {
                    symItem->setText(QString::number(value));
                }
                m_updating = false;
            }
        }
    }
}

void MatrixRepresentationDialog::onRefreshButtonClicked()
{
    populateMatrix();
}

void MatrixRepresentationDialog::onApplyButtonClicked()
{
    // 检查算法是否正在运行
    if (m_model && m_model->isAlgorithmRunning()) {
        QMessageBox::warning(this, "操作被禁止", "算法正在执行中，无法修改图结构。请先停止算法。");
        return;
    }
    
    updateGraphFromMatrix();
}

void MatrixRepresentationDialog::updateGraphFromMatrix()
{
    // 检查算法是否正在运行
    if (m_model && m_model->isAlgorithmRunning()) {
        QMessageBox::warning(this, "操作被禁止", "算法正在执行中，无法修改图结构。请先停止算法。");
        return;
    }
    if (!m_model) {
        return;
    }
    
    QMap<QString, QMap<QString, int>> matrix;
    
    // 从表格读取数据
    int size = m_tableWidget->rowCount() - 1;
    for (int i = 1; i <= size; ++i) {
        QTableWidgetItem *rowHeader = m_tableWidget->item(i, 0);
        if (!rowHeader) continue;
        
        QString from = rowHeader->text();
        QMap<QString, int> row;
        
        for (int j = 1; j <= size; ++j) {
            QTableWidgetItem *colHeader = m_tableWidget->item(0, j);
            if (!colHeader) continue;
            
            QString to = colHeader->text();
            QTableWidgetItem *item = m_tableWidget->item(i, j);
            
            if (item) {
                QString text = item->text().trimmed();
                bool ok;
                int value = text.toInt(&ok);
                
                if (ok) {
                    row[to] = value;
                } else {
                    row[to] = -1;
                }
            } else {
                row[to] = -1;
            }
        }
        
        matrix[from] = row;
    }
    
    // 更新图结构
    m_model->updateFromMatrix(matrix);
    
    m_statusLabel->setText("图结构已更新");
    QMessageBox::information(this, "成功", "图结构已根据矩阵更新");
}

