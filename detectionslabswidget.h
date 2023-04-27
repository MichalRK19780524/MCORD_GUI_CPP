#ifndef DETECTIONSLABSWIDGET_H
#define DETECTIONSLABSWIDGET_H

#include <QWidget>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QPushButton>

#include "slab.h"
#include "lanconnection.h"
#include "afetype.h"
#include <QApplication>

class DetectionSlabsWidget : public QWidget
{
    Q_OBJECT
public:
//    explicit DetectionSlabsWidget(QList<Slab*> &slabs, QWidget *parent = nullptr);
    explicit DetectionSlabsWidget(LanConnection *lanConnection, QWidget *parent = nullptr);
    virtual ~DetectionSlabsWidget();
    void addDetectionSlab(Slab* slab, AfeType afeType);


signals:
    void clicked(QObject* button);

private:
    QList<Slab*> *slabs = nullptr;
    QSignalMapper *signalMapper = nullptr;
    QVBoxLayout *vBoxLayout = nullptr;
    QSet<quint16> *setId = nullptr;
    LanConnection *lanConnection = nullptr;

};

#endif // DETECTIONSLABSWIDGET_H
