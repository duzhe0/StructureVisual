#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
#include "model.h"

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void doNow();
private slots:
    // void on_GraphPB_clicked();
    void handleGraphPB();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
