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
    QList<Slab> *slabs;
    QSignalMapper *signalMapper;
    QVBoxLayout *vBoxLayout;

};

#endif // DETECTIONSLABSWIDGET_H
