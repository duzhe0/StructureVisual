#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Model* myModel=new Model;
    //connect(this,&MainWindow::doNow,myModel,&Model::doSomthing);
    connect(ui->GraphPB,&QPushButton::clicked,this,&MainWindow::handleGraphPB);
    connect(this,&MainWindow::doNow,myModel,&Model::doSomthing);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleGraphPB(){
    emit doNow();
}

// void MainWindow::on_GraphPB_clicked()
// {
//     emit doNow();
// }

