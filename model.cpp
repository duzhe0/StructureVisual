#include "model.h"

Model::Model(QObject *parent)
    : QObject{parent}
{}

void Model::doSomthing(){
    QMessageBox::information(nullptr,"Hello","Button has been clicked!");
}
