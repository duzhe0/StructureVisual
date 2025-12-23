QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    VisualItem.cpp \
    GraphModel.cpp \
    SortModel.cpp \
    AlgorithmController.cpp \
    MatrixRepresentationDialog.cpp \
    AdjacencyListRepresentationDialog.cpp \
    simplestring.cpp \
    MyVectorQString.cpp \
    MyVectorInt.cpp \
    MyVectorVertexItemPtr.cpp \
    MyVectorEdgeItemPtr.cpp \
    MyMapQStringToInt.cpp \
    MyMapQStringToQString.cpp \
    MyMapQStringToQPointF.cpp \
    MyMapQStringToVertexItemPtr.cpp \
    MyMapPairToEdgeItemPtr.cpp \
    MySetQString.cpp \
    MySetPairQStringQString.cpp \
    MyMapQStringToSetQString.cpp \
    MyQueueQString.cpp \
    MyQueueAlgorithmStep.cpp \
    MyStackQString.cpp \
    MyVectorPairQStringQString.cpp \
    MyVectorBarItemPtr.cpp \
    MyQueueSortStep.cpp \
    MyVectorBool.cpp \
    MyVectorEdgeInfo.cpp

HEADERS += \
    mainwindow.h \
    model.h \
    simplestring.h \
    VisualItem.h \
    GraphModel.h \
    SortModel.h \
    AlgorithmController.h \
    MatrixRepresentationDialog.h \
    AdjacencyListRepresentationDialog.h \
    MyVectorQString.h \
    MyVectorInt.h \
    MyVectorVertexItemPtr.h \
    MyVectorEdgeItemPtr.h \
    MyMapQStringToInt.h \
    MyMapQStringToQString.h \
    MyMapQStringToQPointF.h \
    MyMapQStringToVertexItemPtr.h \
    MyPairQStringQString.h \
    MyMapPairToEdgeItemPtr.h \
    MySetQString.h \
    MySetPairQStringQString.h \
    MyMapQStringToSetQString.h \
    MyQueueQString.h \
    MyQueueAlgorithmStep.h \
    MyStackQString.h \
    MyVectorPairQStringQString.h \
    MyVectorBarItemPtr.h \
    MyQueueSortStep.h \
    MyVectorBool.h \
    EdgeInfo.h \
    MyVectorEdgeInfo.h \
    VectorException.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    StructureVisual_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
