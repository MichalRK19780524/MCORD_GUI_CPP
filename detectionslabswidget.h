#ifndef DETECTIONSLABSWIDGET_H
#define DETECTIONSLABSWIDGET_H

#include <QWidget>
#include <QSignalMapper>
#include <QVBoxLayout>

#include "slab.h"

class DetectionSlabsWidget : public QWidget
{
    Q_OBJECT
public:
//    explicit DetectionSlabsWidget(QList<Slab*> &slabs, QWidget *parent = nullptr);
    explicit DetectionSlabsWidget(QWidget *parent = nullptr);
    virtual ~DetectionSlabsWidget();
    void addDetectionSlab(Slab* slab);

signals:
    void clicked(int id);

private:
    QList<Slab> *slabs = nullptr;
    QSignalMapper *signalMapper = nullptr;
    QVBoxLayout *vBoxLayout = nullptr;

};

#endif // DETECTIONSLABSWIDGET_H
