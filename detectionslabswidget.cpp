#include "detectionslabswidget.h"
#include "slab.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

//DetectionSlabsWidget::DetectionSlabsWidget(QList<Slab*> &slabs, QWidget *parent)
//    : QWidget{parent}
//{
//    signalMapper = new QSignalMapper(this);
////    QList<QHBoxLayout> listOfRaws;
//    vBoxLayout = new QVBoxLayout;
//    for (int i = 0; i < slabs.size(); ++i) {
//        int id = slabs[i]->getId();
//        QHBoxLayout *hBoxLayout = new QHBoxLayout;
//        QPushButton *buttonSet = new QPushButton("Set");
//        QLabel *labelId = new QLabel(QString::number(id));

//        connect(buttonSet, &QPushButton::clicked, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));

//        hBoxLayout->insertWidget(0,buttonSet);
//        hBoxLayout->insertWidget(1,labelId);
//        vBoxLayout->addLayout(hBoxLayout);
//    }
//    setLayout(vBoxLayout);
//}

DetectionSlabsWidget::DetectionSlabsWidget(QWidget *parent): QWidget(parent), signalMapper(new QSignalMapper), slabs(new QList<Slab>), vBoxLayout(new QVBoxLayout)
{
}

DetectionSlabsWidget::~DetectionSlabsWidget()
{
    delete signalMapper;
    signalMapper = nullptr;
}

void DetectionSlabsWidget::addDetectionSlab(Slab* slab)
{
    int id = slab->getId();
    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    QPushButton *buttonSet = new QPushButton("Set");
    QLabel *labelId = new QLabel(QString::number(id));

    connect(buttonSet, &QPushButton::clicked, signalMapper, static_cast<void(QSignalMapper::*)()>(&QSignalMapper::map));

    hBoxLayout->insertWidget(0,buttonSet);
    hBoxLayout->insertWidget(1,labelId);
    vBoxLayout->addLayout(hBoxLayout);
}
