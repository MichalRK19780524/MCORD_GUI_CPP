#include "detectortablemodel.h"
#include "slab.h"
#include "widget.h"

#include <QMessageBox>
//#include <QPainter>

DetectorTableModel::DetectorTableModel(QObject *parent)
        : QAbstractTableModel(parent), slabs(new QList<Slab>), setId(new QSet<quint16>) {
}

DetectorTableModel::DetectorTableModel(const QStringList headers, QObject *parent) : QAbstractTableModel(parent),
                                                                                      headers(headers),
                                                                                      slabs(new QList<Slab>),
                                                                                      setId(new QSet<quint16>) {}

QVariant DetectorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0 :
                return headers[0];
            case 1 :
                return headers[1];
            case 2:
                return headers[2];
            case 3:
                return headers[3];
            case 4:
                return headers[4];
            case 5:
                return headers[5];
            case 6:
                return headers[6];
            case 7:
                return headers[7];
        }
    }
    return QVariant();
}

int DetectorTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return 2 * slabs->size();
}

int DetectorTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return 8;
}

QVariant DetectorTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= 2 * slabs->size()) {
        return QVariant();
    }

    int row = index.row();
    Slab slab;
    if (slabs->size() > 0) {
        slab = slabs->at(row / 2);
    } else {
        qDebug() << "slabs nie powinno być puste";
    }
    shared_ptr<Sipm> sipm;
    if (row % 2 == 0) {
        sipm = slab.getMaster();
    } else {
        sipm = slab.getSlave();
    }

    if (role == Qt::DisplayRole) {
        QVariant var;
        switch (index.column()) {
            int id;
            case 0:
                id = slab.getId();
                if (id == -1) {
                    return QString();
                } else {
                    return slab.getId();
                }
            case 1:
                var.setValue(sipm->getStatusColor());
                return var;
            case 2:
                return QString();
            case 3:
                if (row % 2 == 0) {
                    return QString("Master");
                } else {
                    return QString("Slave");
                }
            case 4:
                return QString();
            case 5:
                return sipm->getMeasuredVoltage();
            case 6:
                return sipm->getCurrent();
            case 7:
                return sipm->getTemperature();
        }
    }
    return QVariant();
}

QString DetectorTableModel::appendSlab(Slab slab) {
    const int rowIndex = 2 * slabs->size();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex + 1);
    int id = slab.getId();
    if (setId->contains(id)) {
        return QString("Slab number %1 has already been added.").arg(id);
    } else {
        slabs->append(slab);
        setId->insert(id);
    }
    endInsertRows();
    emit slabAppended(id);
    return "OK";
}

QString DetectorTableModel::updateSlab(Slab slab) {
    int slabPosition = findSlabPosition(slab.getId());
    slabs->replace(slabPosition, slab);
    emit dataChanged(index(2 * slabPosition, 0), index(2 * slabPosition + 1, columnCount() - 1));
    emit slabUpdated(slab.getId());
    return "OK";
}

int DetectorTableModel::findSlabPosition(quint16 slabId) {
    qsizetype size = slabs->size();
    int slabPosition{0};
    for (int i = 0; i < size; ++i) {
        if (slabs->at(i).getId() == slabId) {
            slabPosition = i;
            break;
        }
    }

    return slabPosition;
}

QModelIndex DetectorTableModel::findIndexOfMasterSlabSetButton(quint16 slabId) {
    int slabPosition = findSlabPosition(slabId);
    return index(2 * slabPosition, Widget::SET_COLUMN_INDEX);
}

QModelIndex DetectorTableModel::findIndexOfSlaveSlabSetButton(quint16 slabId) {
    int slabPosition = findSlabPosition(slabId);
    return index(2 * slabPosition + 1, Widget::SET_COLUMN_INDEX);
}

QModelIndex DetectorTableModel::findIndexOfMasterSlabOnButton(quint16 slabId) {
    int slabPosition = findSlabPosition(slabId);
    return index(2 * slabPosition, Widget::POWER_COLUMN_INDEX);
}

QModelIndex DetectorTableModel::findIndexOfSlaveSlabOnButton(quint16 slabId) {
    int slabPosition = findSlabPosition(slabId);
    return index(2 * slabPosition + 1, Widget::POWER_COLUMN_INDEX);
}

QModelIndex DetectorTableModel::findIndexOfMasterSlabOffButton(quint16 slabId) {
    int slabPosition = findSlabPosition(slabId);
    return index(2 * slabPosition, Widget::POWER_COLUMN_INDEX);
}

QModelIndex DetectorTableModel::findIndexOfSlaveSlabOffButton(quint16 slabId) {
    int slabPosition = findSlabPosition(slabId);
    return index(2 * slabPosition + 1, Widget::POWER_COLUMN_INDEX);
}

Slab DetectorTableModel::findSlab(quint16 slabId) {
    qsizetype size = slabs->size();
    Slab slab;
    for (int i = 0; i < size; ++i) {
        slab = slabs->at(i);
        if (slab.getId() == slabId) {
            break;
        }
    }
    return slab;
}

//QList<Slab> *DetectorTableModel::getSlabs() const { return slabs; }
QSet<quint16> *DetectorTableModel::getSetId() const { return setId; }

//QString DetectorTableModel::reloadMasterSlab(Slab slab)
//{
//    quint16 slabId = slab.getId();
//    int slabPosition = findSlabPosition(slabId);
//    if(!setId->contains(slabId))
//    {
//        return QString("Internal Error: Slab number %1 does not exist in model").arg(slabId);
//    }
//    slabs->replace(slabPosition, slab);
//    emit dataChanged(index(2 * slabPosition, 0), index(2 * slabPosition + 1, columnCount() - 1));
//    return "OK";
//}

//QString DetectorTableModel::reloadSlaveSlab(Slab *slab)
//{
//    quint16 slabId = slab->getId();
//    int slabPosition = findSlabPosition(slabId);
//    if(!setId->contains(slabId))
//    {
//        return QString("Internal Error: Slab number %1 does not exist in model").arg(slabId);
//    }
//    slabs->replace(slabPosition, slab);
//    emit dataChanged(index(2 * slabPosition + 1, 0), index(2 * slabPosition + 1, columnCount() - 1));
//    return "OK";
//}
