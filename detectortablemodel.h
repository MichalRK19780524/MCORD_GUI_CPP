#ifndef DETECTORTABLEMODEL_H
#define DETECTORTABLEMODEL_H

#include <QAbstractTableModel>
//#include <QMessageBox>
#include "slab.h"

class DetectorTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DetectorTableModel(QObject *parent = nullptr);
    DetectorTableModel(const QStringList *headers, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

//    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
//    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Add data:
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QString appendSlab(Slab* slab);

    QModelIndex findIndexOfMasterSlabSetButton(quint16 slabId);
    QModelIndex findIndexOfSlaveSlabSetButton(quint16 slabId);

    QModelIndex findIndexOfMasterSlabOnButton(quint16 slabId);
    QModelIndex findIndexOfSlaveSlabOnButton(quint16 slabId);

    QModelIndex findIndexOfMasterSlabOffButton(quint16 slabId);
    QModelIndex findIndexOfSlaveSlabOffButton(quint16 slabId);

    Slab* findSlab(quint16 slabId);
    QString reloadMasterSlab(Slab* slab);
    QString reloadSlaveSlab(Slab* slab);

    int findSlabPosition(quint16 slabId);

private:
    QList<Slab*> *slabs = nullptr;
    QSet<quint16> *setId = nullptr;
    const QStringList *headers = nullptr;

};

#endif // DETECTORTABLEMODEL_H
