#ifndef DIJKSTRATABLEDIALOG_H
#define DIJKSTRATABLEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMap>

class DijkstraTableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DijkstraTableDialog(const QString &startVertex, QWidget *parent = nullptr);
    ~DijkstraTableDialog() = default;
    
    // 初始化表格，设置所有顶点
    void initializeTable(const QStringList &vertices);
    
    // 更新顶点的距离和前驱
    void updateVertex(const QString &vertex, int distance, const QString &predecessor, bool visited = false);
    
    // 重置表格
    void resetTable();

private slots:
    void onCloseButtonClicked();

private:
    void setupUI();
    int findVertexRow(const QString &vertex) const;
    
    QTableWidget *m_tableWidget;
    QLabel *m_statusLabel;
    QPushButton *m_closeButton;
    QString m_startVertex;
    QMap<QString, int> m_vertexToRow;  // 顶点名称到行号的映射
};

#endif // DIJKSTRATABLEDIALOG_H

