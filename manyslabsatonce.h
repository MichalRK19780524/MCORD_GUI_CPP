#ifndef MANYSLABSATONCE_H
#define MANYSLABSATONCE_H

#include <QWidget>
#include <QDialog>
#include <QSignalMapper>
//#include <QSettings>
#include <QFile>
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
    explicit ManySlabsAtOnce(LanConnection *lanConnection,  QString ipAddress, QWidget *parent = nullptr);
    ~ManySlabsAtOnce() override;

    static QFile file;
    static QTextStream textIds;
    static QHash<QString, std::tuple<QString, unsigned int, QList<int>>> *const hubsComentsAndIds;

private:
    Ui::ManySlabsAtOnce *ui;
    BaseWidget* base;
    DetectorTableModel *model = nullptr;
//    QSettings *settings = nullptr;


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
    QString getIpAddress();

    bool saveId(QString ipAddress, int position, QString id);
private slots:
    void onAllClicked();
    void onClicked(int slabId);
    void setMasterVoltageClicked(int slabId);
    void setSlaveVoltageClicked(int slabId);
    void setAllClicked();
    void offClicked(int slabId);
    void offAllClicked();
    void appendManySlabsToModel(QList<Slab> slabs);
    void tableUpdate();
    void updateSlabInModel(Slab slab);
    void updateManySlabsInModel(QList<Slab> slab);
    void idEditingFinished(int position);
    void loadIdNumbers(QString ipAddress);
    void loadSetVoltages(QPair<QVariantHash, QVariantHash> voltages);

signals:
    void initializationManySlabsRequired(QList<Slab> slabs);
    void offManySlabsRequired(QList<Slab> slabs);
    void setManySlabsRequired(QList<Slab> slabs);
    void manySlabsUpdateRequired(QList<Slab> slabs);
    void closeLanConnection();
    void readIdsFromFileError();
    void onRequired(Slab slab);
    void offRequired(Slab slab);
    void setMasterVoltageRequired(Slab slab);
    void setSlaveVoltageRequired(Slab slab);
    void loadIdNumbersSucceded();
};


#endif // MANYSLABSATONCE_H
