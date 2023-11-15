#include "detectortablemodel.h"
#include "slab.h"
#include "widget.h"

#include <QMessageBox>
//#include <QPainter>

DetectorTableModel::DetectorTableModel(QObject *parent)
        : QAbstractTableModel(parent), slabs(new QList<Slab>), setId(new QSet<quint16>) {
}

DetectorTableModel::DetectorTableModel(const QStringList headers, QObject *parent)
    : QAbstractTableModel(parent), headers(headers), slabs(new QList<Slab>), setId(new QSet<quint16>) {

}

DetectorTableModel::DetectorTableModel(const QStringList headers, std::size_t capacity, QObject *parent)
    : QAbstractTableModel(parent), headers(headers), slabs(new QList<Slab>), setId(new QSet<quint16>)
{
    for(std::size_t i = 0; i < capacity; ++i){
        slabs->append(Slab(-1, std::make_shared<Sipm>(), std::make_shared<Sipm>()));
    }
}

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
        float measuredVoltage;
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
                measuredVoltage = sipm->getMeasuredVoltage();
                if(measuredVoltage < 1.0){
                    return QString("Off");
                } else {
                    return QString::number(measuredVoltage, 'f', 2);
                }
            case 6:
                return QString::number(sipm->getCurrent(), 'f', 1);
            case 7:
                return QString::number(sipm->getTemperature(), 'f', 2);
        }
    }

    if(role == Qt::TextAlignmentRole){
        if(index.column() == 0 || index.column() == 3){
            return Qt::AlignCenter;
        } else if(index.column() == 5 || index.column() == 6 || index.column() == 7){
            return Qt::AlignRight;
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
    qDebug() << "Slab in DetectorTableModel::updateSlab: "<< '\n' << "Id:" << slab.getId() << '\t' << "Set Master Voltage: " << slab.getMaster()->getSetVoltage();
    qDebug() << "Slab in DetectorTableModel::updateSlab: "<< '\n' << "Id:" << slab.getId() << '\t' << "Set Slave Voltage: " << slab.getSlave()->getSetVoltage();
    int slabPosition = findSlabPosition(slab.getId());
    if(slabs->size() > 0){
        slabs->replace(slabPosition, slab);
    }
    emit dataChanged(index(2 * slabPosition, 0), index(2 * slabPosition + 1, columnCount() - 1));
    emit slabUpdated(slab.getId());
    return "OK";
}

QString DetectorTableModel::replaceSlab(quint8 position, Slab slab)
{
    slabs->replace(position, slab);
    int id = slab.getId();
    if(id != -1){
        setId->insert(id);
    }
    emit dataChanged(index(2 * position, 0), index(2 * position + 1, columnCount() - 1));
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

QModelIndex DetectorTableModel::findIndexOfMasterRowSetButton(quint8 rowId)
{
    return index(2*(rowId/2), Widget::SET_COLUMN_INDEX);
}

QModelIndex DetectorTableModel::findIndexOfSlaveRowSetButton(quint8 rowId)
{
    return index(2*(rowId/2) + 1, Widget::SET_COLUMN_INDEX);
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
