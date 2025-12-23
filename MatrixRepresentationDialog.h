#ifndef MATRIXREPRESENTATIONDIALOG_H
#define MATRIXREPRESENTATIONDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include "GraphModel.h"

class MatrixRepresentationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MatrixRepresentationDialog(GraphModel *model, QWidget *parent = nullptr);
    ~MatrixRepresentationDialog() = default;

private slots:
    void onCellChanged(int row, int column);
    void onRefreshButtonClicked();
    void onApplyButtonClicked();

private:
    void setupUI();
    void populateMatrix();
    void updateGraphFromMatrix();
    
    GraphModel *m_model;
    QTableWidget *m_tableWidget;
    QPushButton *m_refreshButton;
    QPushButton *m_applyButton;
    QLabel *m_statusLabel;
    bool m_updating;
};

#endif // MATRIXREPRESENTATIONDIALOG_H

