#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QMainWindow>
#include <QMessageBox>
class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);
public slots:
    void doSomthing();
signals:
};

#endif // MODEL_H
