#ifndef MANYSLABSATONCE_H
#define MANYSLABSATONCE_H

#include <QWidget>
#include <QDialog>
#include <QSignalMapper>
#include <QSettings>
#include "detectortablemodel.h"
#include "basewidget.h"
//#include "lanconnection.h"

namespace Ui {
class ManySlabsAtOnce;
}

class ManySlabsAtOnce : public QWidget
{
    Q_OBJECT

public:
    explicit ManySlabsAtOnce(LanConnection *lanConnection, QSettings *settings, QWidget *parent = nullptr);
    ~ManySlabsAtOnce() override;

private:
    Ui::ManySlabsAtOnce *ui;
    BaseWidget* base;
    DetectorTableModel *model = nullptr;
    QSettings *settings = nullptr;

    QSignalMapper *setMasterSignalMapper = nullptr;
    QSignalMapper *setSlaveSignalMapper = nullptr;
    QSignalMapper *onSignalMapper = nullptr;
    QSignalMapper *offSignalMapper = nullptr;
    QSignalMapper *setIdSignalMapper = nullptr;

    void addPowerWidgets();
    void addSetWidgets();
    void addIdWidgets();
    void setMasterStatusColor(Slab &slab);
    void setSlaveStatusColor(Slab &slab);
    QList<Slab> takeSlabsIds();
private slots:
    void onAllClicked();
//    void setAllClicked();
    void offAllClicked();
    void appendManySlabsToModel(QList<Slab> slabs);
    void tableUpdate();
    void updateSlabInModel(Slab slab);
    void updateManySlabsInModel(QList<Slab> slab);
    void idEditingFinished(int position);

signals:
    void initializationManySlabsRequired(QList<Slab> slabs);
    void offManySlabsRequired(QList<Slab> slabs);
    void setManySlabsRequired(QList<Slab> slabs);
    void manySlabsUpdateRequired(QList<Slab> slabs);
};

#endif // MANYSLABSATONCE_H
